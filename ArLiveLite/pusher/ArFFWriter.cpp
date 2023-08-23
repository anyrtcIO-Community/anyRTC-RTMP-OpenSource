#include "ArWriter.h"
#include <memory>
#include <string>
#include <vector>
#include "ByteOder.h"
#include "codec/pluginaac.h"
#include "webrtc/rtc_base/time_utils.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

static inline int GetStartPatternSize(const void* data, size_t length, int first_pattern_size)
{
	if (data != nullptr)
	{
		auto buffer = static_cast<const uint8_t*>(data);

		if (first_pattern_size > 0)
		{
			if ((buffer[0] == 0x00) && (buffer[1] == 0x00))
			{
				if (
					(first_pattern_size == 4) &&
					((length >= 4) && (buffer[2] == 0x00) && (buffer[3] == 0x01)))
				{
					// 0x00 0x00 0x00 0x01 pattern
					return 4;
				}
				else if (
					(first_pattern_size == 3) &&
					((length >= 3) && (buffer[2] == 0x01)))
				{
					// 0x00 0x00 0x01 pattern
					return 3;
				}
			}

			// pattern_size must be the same with the first_pattern_size
			return -1;
		}
		else
		{
			// probe mode

			if ((length >= 4) && ((buffer[0] == 0x00) && (buffer[1] == 0x00) && (buffer[2] == 0x00) && (buffer[3] == 0x01)))
			{
				// 0x00 0x00 0x00 0x01 pattern
				return 4;
			}
			if ((length >= 3) && ((buffer[0] == 0x00) && (buffer[1] == 0x00) && (buffer[2] == 0x01)))
			{
				// 0x00 0x00 0x01 pattern
				return 3;
			}
		}
	}

	return -1;
}

int ConvertAnnexbToAvcc(const char* pData, int nLen, char* outData, int outSize)
{
	// size_t total_pattern_length = 0;

	auto buffer = pData;
	size_t remained = nLen;
	off_t offset = 0;
	off_t last_offset = 0;

	char* avcc_data = outData;
	char* byte_stream = avcc_data;
	int avcc_len = 0;

	// This code assumes that (NALULengthSizeMinusOne == 3)
	while (remained > 0)
	{
		if (*buffer == 0x00)
		{
			auto pattern_size = GetStartPatternSize(buffer, remained, 0);

			if (pattern_size > 0)
			{
				if (last_offset < offset)
				{
					SetBE32(byte_stream, offset - last_offset);
					byte_stream += sizeof(int32_t);
					memcpy(byte_stream, pData + last_offset, offset - last_offset);
					byte_stream += offset - last_offset;

					avcc_len += (sizeof(int32_t) + (offset - last_offset));
					last_offset = offset;
				}

				buffer += pattern_size;
				offset += pattern_size;
				last_offset += pattern_size;
				remained -= pattern_size;

				continue;
			}
		}

		buffer++;
		offset++;
		remained--;
	}

	if (last_offset < offset)
	{
		// Append remained data
		SetBE32(byte_stream, offset - last_offset);
		byte_stream += sizeof(int32_t);
		memcpy(byte_stream, pData + last_offset, offset - last_offset);
		byte_stream += offset - last_offset;

		avcc_len += (sizeof(int32_t) + (offset - last_offset));
		last_offset = offset;
	}

	return avcc_len;
}
#if 0
static struct AVFrameDeleter {
	void operator()(AVFrame* ptr) const { av_frame_free(&ptr); }
};
#endif
class ArFFWriter : public ArWriter
{
public:
	ArFFWriter(void);
	virtual ~ArFFWriter(void);



	//* For ArWriter
	virtual bool StartTask(const char* strFormat, const char* strUrl);
	virtual void StopTask();

	virtual void RunOnce();

	virtual void SetAudioEnable(bool bEnable);
	virtual void SetVideoEnable(bool bEnable);
	virtual void SetAutoRetry(bool bAuto);

	virtual void SetEncType(CodecType vidType, CodecType audType);
	virtual int SetAudioEncData(const char* pData, int nLen, int64_t pts, int64_t dts);
	virtual int SetAudioRawData(char* pData, int len, int sample_hz, int channels, int64_t pts, int64_t dts);
	virtual int SetVideoEncData(const char* pData, int nLen, bool isKeyframe, int64_t pts, int64_t dts);

protected:
	bool Connect();
	void Release();
private:
	CodecType vid_codec_type_;
	CodecType aud_codec_type_;
	bool b_vid_enable_;
	bool b_aud_enable_;

	char* avcc_data_;
	int avcc_size_;

private:
	AVFormatContext* format_context_;

	AVStream* vid_stream_;
	AVStream* aud_stream_;

	aac_enc_t aac_enc_;

private:
	ArWriterState now_state_;
	bool b_auto_retry_;
	uint32_t n_next_retry_time_;

	std::string str_format_;
	std::string str_url_;
};

ArWriter* createArWriter()
{
	return new ArFFWriter();
}

ArFFWriter::ArFFWriter(void)
	: vid_codec_type_(CT_H264)
	, aud_codec_type_(CT_AAC)
	, b_vid_enable_(true)
	, b_aud_enable_(true)
	, avcc_data_(NULL)
	, avcc_size_(0)
	, format_context_(NULL)
	, vid_stream_(NULL)
	, aud_stream_(NULL)
	, aac_enc_(NULL)
	, now_state_(WS_Init)
	, b_auto_retry_(false)
	, n_next_retry_time_(0)
{
	aac_enc_ = aac_encoder_open(2, 44100, 16, 64000, true);
}
ArFFWriter::~ArFFWriter(void)
{
	if (aac_enc_ != NULL) {
		aac_encoder_close(aac_enc_);
		aac_enc_ = NULL;
	}
}



//* For ArWriter
bool ArFFWriter::StartTask(const char* strFormat, const char* strUrl)
{
	str_format_ = strFormat;
	str_url_ = strUrl;
	n_next_retry_time_ = rtc::Time32();
	return true;
}
void ArFFWriter::StopTask()
{
	Release();
}

void ArFFWriter::RunOnce()
{
	if (n_next_retry_time_ != 0 && n_next_retry_time_ <= rtc::Time32()) {
		n_next_retry_time_ = 0;
		ArWriterState oldState = now_state_;
		now_state_ = WS_Connecting;
		if (callback_ != NULL) {
			callback_->OnArWriterStateChange(oldState, now_state_);
		}
		bool bRet = Connect();

		oldState = now_state_;

		if (!bRet) {
			now_state_ = WS_Failed;
			if (callback_ != NULL) {
				callback_->OnArWriterStateChange(oldState, now_state_);
			}
			Release();

			if (b_auto_retry_) {
				n_next_retry_time_ = rtc::Time32() + 1500;
			}
			else {
				oldState = now_state_;
				now_state_ = WS_Ended;
				if (callback_ != NULL) {
					callback_->OnArWriterStateChange(oldState, now_state_);
				}
			}
		}
		else {
			now_state_ = WS_Connected;
			if (callback_ != NULL) {
				callback_->OnArWriterStateChange(oldState, now_state_);
			}
		}
	}
}

void ArFFWriter::SetAudioEnable(bool bEnable)
{
	b_aud_enable_ = bEnable;
}
void ArFFWriter::SetVideoEnable(bool bEnable)
{
	b_vid_enable_ = bEnable;
}
void ArFFWriter::SetAutoRetry(bool bAuto)
{
	b_auto_retry_ = bAuto;
}

void ArFFWriter::SetEncType(CodecType vidType, CodecType audType)
{
	vid_codec_type_ = vidType;
	aud_codec_type_ = audType;
}

int ArFFWriter::SetAudioEncData(const char* pData, int nLen, int64_t pts, int64_t dts)
{
	if (aud_stream_ == NULL) {
		return -1;
	}
	if (!b_aud_enable_) {
		return -2;
	}
	if (now_state_ != WS_Connected) {
		return -3;
	}
	// Make avpacket
	AVPacket av_packet;
	av_init_packet(&av_packet);	//@Eric - 20230323 - 不能使用av_packet = { 0 };初始化，必须使用av_init_packet初始化 - 否则录制的文件兼容性差
	av_packet.stream_index = aud_stream_->index;
	av_packet.flags = 0;// (flag == MediaPacketFlag::Key) ? AV_PKT_FLAG_KEY : 0;
	//av_packet.pts = av_packet.dts = AV_NOPTS_VALUE;
	//av_packet.pts = av_rescale_q(pts, AVRational{ 1, 1000 }, aud_stream_->time_base); //pts;// 
	//av_packet.dts = av_rescale_q(dts, AVRational{ 1, 1000 }, aud_stream_->time_base);//dts;// 
	//printf("aud pts: %lld dts: %lld\r\n", pts, dts);

	av_packet.pts = pts;
	av_packet_rescale_ts(&av_packet, AVRational{ 1, 1000 }, aud_stream_->time_base);
	//av_packet.duration = 0;

	if (strcmp(format_context_->oformat->name, "flv") == 0)
	{
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
		av_packet.pts = pts;
		av_packet.dts = dts;
	}
	else if (strcmp(format_context_->oformat->name, "mp4") == 0)
	{
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}
	else if (strcmp(format_context_->oformat->name, "mpegts") == 0 || strcmp(format_context_->oformat->name, "rtp_mpegts") == 0)
	{
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}
	else {
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}

	int error = av_interleaved_write_frame(format_context_, &av_packet);
	if (error != 0)
	{
		char errbuf[256];
		av_strerror(error, errbuf, sizeof(errbuf));
		printf("Send audio packet error(%d:%s) \r\n", error, errbuf);

		ArWriterState oldState = now_state_;
		now_state_ = WS_Failed;
		if (callback_ != NULL) {
			callback_->OnArWriterStateChange(oldState, now_state_);
		}
		if (b_auto_retry_) {
			n_next_retry_time_ = rtc::Time32() + 1500;
		}
		else {
			oldState = now_state_;
			now_state_ = WS_Ended;
			if (callback_ != NULL) {
				callback_->OnArWriterStateChange(oldState, now_state_);
			}
		}
		Release();
		return false;
	}
	return 0;
}
int ArFFWriter::SetAudioRawData(char* pData, int len, int sample_hz, int channels, int64_t pts, int64_t dts)
{
	const int kMaxDataSizeSamples = 4196;
	if (44100 != sample_hz || 2 != channels) {
		return -1;
	}
	else {
		int status = 0;
		if (aac_enc_)
		{
			unsigned int outlen = 0;
			uint32_t curtime = rtc::Time32();
			uint8_t encoded[1024];
			status = aac_encoder_encode_frame(aac_enc_, (uint8_t*)pData, len, encoded, &outlen);
			if (outlen > 0)
			{
				SetAudioEncData((char*)encoded, outlen, pts, dts);
			}
		}
	}
	return 0;
}
int ArFFWriter::SetVideoEncData(const char* pData, int nLen, bool isKeyframe, int64_t pts, int64_t dts)
{
	if (vid_stream_ == NULL) {
		return -1;
	}
	if (!b_vid_enable_) {
		return -2;
	}
	if (now_state_ != WS_Connected) {
		return -3;
	}
	// Make avpacket
	AVPacket av_packet;
	av_init_packet(&av_packet);	//@Eric - 20230323 - 不能使用av_packet = { 0 };初始化，必须使用av_init_packet初始化
	av_packet.stream_index = vid_stream_->index;
	av_packet.flags = isKeyframe ? AV_PKT_FLAG_KEY : 0;
	//av_packet.pts = av_packet.dts = AV_NOPTS_VALUE;
	//av_packet.pts = av_rescale_q(pts, AVRational{ 1, 1000 }, vid_stream_->time_base);	//pts;// 
	//av_packet.dts = av_rescale_q(dts, AVRational{ 1, 1000 }, vid_stream_->time_base);	//dts;// 
	//printf("vid pts: %lld dts: %lld\r\n", pts, dts);

	av_packet.pts = pts;
	av_packet_rescale_ts(&av_packet, AVRational{ 1, 1000 }, vid_stream_->time_base);
	//av_packet.duration = 0;
	
	if (strcmp(format_context_->oformat->name, "flv") == 0)
	{
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
		//* 如果是FLV，时间戳用原始的就行了
		av_packet.pts = pts;	
		av_packet.dts = dts;
	}
	else if (strcmp(format_context_->oformat->name, "mp4") == 0)
	{
		if (avcc_size_ <= nLen + 32) {
			if (avcc_data_ != NULL) {
				delete[] avcc_data_;
				avcc_data_ = NULL;
			}
			avcc_size_ = 0;
		}
		if (avcc_data_ == NULL) {
			avcc_size_ = nLen + 32;
			avcc_data_ = new char[avcc_size_];
		}
#if 0
		int avccLen = ConvertAnnexbToAvcc(pData, nLen, avcc_data_, avcc_size_);
		av_packet.size = avccLen;
		av_packet.data = (uint8_t*)avcc_data_;
#endif
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}
	else if (strcmp(format_context_->oformat->name, "mpegts") == 0 || strcmp(format_context_->oformat->name, "rtp_mpegts") == 0)
	{
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}
	else {
		av_packet.size = nLen;
		av_packet.data = (uint8_t*)pData;
	}

	int error = av_interleaved_write_frame(format_context_, &av_packet);
	if (error != 0)
	{
		char errbuf[256];
		av_strerror(error, errbuf, sizeof(errbuf));

		printf("Send video packet error(%d:%s) \r\n", error, errbuf);
		ArWriterState oldState = now_state_;
		now_state_ = WS_Failed;
		if (callback_ != NULL) {
			callback_->OnArWriterStateChange(oldState, now_state_);
		}
		if (b_auto_retry_) {
			n_next_retry_time_ = rtc::Time32() + 1500;
		}
		else {
			oldState = now_state_;
			now_state_ = WS_Ended;
			if (callback_ != NULL) {
				callback_->OnArWriterStateChange(oldState, now_state_);
			}
		}
		Release();
		return false;
	}
	return 0;
}


bool ArFFWriter::Connect()
{
	{//1, 
		//const AVOutputFormat* output_format = nullptr;
		AVOutputFormat* output_format = nullptr;
		output_format = av_guess_format(str_format_.c_str(), nullptr, nullptr);
		if (output_format == nullptr)
		{
			Release();
			return false;
		}
		int error = avformat_alloc_output_context2(&format_context_, output_format, nullptr, str_url_.c_str());
		if (error < 0)
		{
			char errbuf[256];
			av_strerror(error, errbuf, sizeof(errbuf));

			printf("Could not create output context. error(%d:%s), path(%s) \r\n", error, errbuf, str_url_.c_str());
			Release();
			return false;
		}
	}

	{//2, 
		if (vid_codec_type_ != CT_None)
		{//* Video
			AVCodecID codecId = (vid_codec_type_ == CT_H264) ? AV_CODEC_ID_H264 : (vid_codec_type_ == CT_H265) ? AV_CODEC_ID_H265
				: AV_CODEC_ID_NONE;
			const AVCodec* codec = avcodec_find_encoder(codecId);
			vid_stream_ = avformat_new_stream(format_context_, codec);
			AVCodecParameters* codecpar = vid_stream_->codecpar;
			int w = 1920;
			int h = 1080;
			int nFps = 30;
			int nBitrate = 1024;

			codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
			codecpar->codec_id = codecId;

			codecpar->codec_tag = 0;
			codecpar->bit_rate = nBitrate * 1000;
			codecpar->width = w;
			codecpar->height = h;
			codecpar->format = AV_PIX_FMT_YUV420P;
			codecpar->sample_aspect_ratio = AVRational{ 1, 1 };

#if 0
			if (vid_codec_type_ == CT_H264)
			{
				//* 推流成功，但是B站播放不了： https://www.cnblogs.com/subo_peng/p/7800658.html
				unsigned char sps_pps[23] = { 0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0x00, 0x0a, 0xf8, 0x0f, 0x00, 0x44, 0xbe, 0x8,
					  0x00, 0x00, 0x00, 0x01, 0x68, 0xce, 0x38, 0x80 };
				codecpar->extradata_size = 23;
				codecpar->extradata = (uint8_t*)av_malloc(23 + AV_INPUT_BUFFER_PADDING_SIZE);
				if (codecpar->extradata == NULL) {
					printf("could not av_malloc the video params extradata! \r\n");
					return -1;
				}
				memcpy(codecpar->extradata, sps_pps, 23);
			}
			else if (vid_codec_type_ == CT_H265) {
			}
#else
			AVCodecContext* vidEncodeCtx = NULL;
			vidEncodeCtx = avcodec_alloc_context3(codec);
			if (vidEncodeCtx != NULL) {
				vidEncodeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
				vidEncodeCtx->codec_id = codecId;
				vidEncodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;
				vidEncodeCtx->extradata = nullptr;
				vidEncodeCtx->extradata_size = 0;

				// If this is ever increased, look at |vidEncodeCtx->thread_safe_callbacks| and
				// make it possible to disable the thread checker in the frame buffer pool.
				vidEncodeCtx->thread_count = 1;
				vidEncodeCtx->thread_type = FF_THREAD_SLICE;

				// Function used by FFmpeg to get buffers to store decoded frames in.
				//vidEncodeCtx->get_buffer2 = AVGetBuffer2;
				// |get_buffer2| is called with the context, there |opaque| can be used to get
				// a pointer |this|.
				//vidEncodeCtx->opaque = this;
				{

					vidEncodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;    //编码的原始数据格式
					vidEncodeCtx->codec_type = AVMEDIA_TYPE_VIDEO; //指定为视频编码
					vidEncodeCtx->width = w;      //分辨率宽
					vidEncodeCtx->height = h;     //分辨率高
					vidEncodeCtx->channels = 0;           //音频通道数
					vidEncodeCtx->time_base = { 1, nFps };  //时间基，表示每个时间刻度是多少秒。
					vidEncodeCtx->framerate = { nFps, 1 };  //帧率，没秒
					vidEncodeCtx->gop_size = 3 * nFps;    //图像组两个关键帧（I帧）的距离，也就是一组帧的数量 原来是10
					vidEncodeCtx->max_b_frames = 0;            //指定B帧数量，B帧是双向参考帧，填充更多B帧，压缩率更高但是延迟也高
					// some formats want stream headers to be separate
					if (vidEncodeCtx->flags & AVFMT_GLOBALHEADER)
					{
						vidEncodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
					}

					// 视频编码器常用的码率控制方式包括abr(平均码率)，crf(恒定码率)，cqp(恒定质量)，
					// ffmpeg中AVCodecContext显示提供了码率大小的控制参数，但是并没有提供其他的控制方式。
					// ffmpeg中码率控制方式分为以下几种情况：
					// 1.如果设置了AVCodecContext中bit_rate的大小，则采用abr的控制方式；
					// 2.如果没有设置AVCodecContext中的bit_rate，则默认按照crf方式编码，crf默认大小为23（此值类似于qp值，同样表示视频质量）；
					// 3.如果用户想自己设置，则需要借助av_opt_set函数设置AVCodecContext的priv_data参数。下面给出三种控制方式的实现代码：
					vidEncodeCtx->bit_rate = nBitrate * 1000; //码率。这里设置固定码率应该没啥用。
					vidEncodeCtx->rc_max_rate = vidEncodeCtx->bit_rate + 10000;
					vidEncodeCtx->rc_min_rate = vidEncodeCtx->bit_rate - 10000;
					///恒定码率
					// 量化比例的范围为0~51，其中0为无损模式，23为缺省值，51可能是最差的。该数字越小，图像质量越好。从主观上讲，18~28是一个合理的范围。18往往被认为从视觉上看是无损的，它的输出视频质量和输入视频一模一样或者说相差无几。但从技术的角度来讲，它依然是有损压缩。
					// 若Crf值加6，输出码率大概减少一半；若Crf值减6，输出码率翻倍。通常是在保证可接受视频质量的前提下选择一个最大的Crf值，如果输出视频质量很好，那就尝试一个更大的值，如果看起来很糟，那就尝试一个小一点值。
					//av_opt_set(vidEncodeCtx->priv_data, "crf", "21.000", AV_OPT_SEARCH_CHILDREN);

					//av_opt_set(vidEncodeCtx->priv_data, "preset", "slow", 0);       //慢速压缩编码，慢的可以保证视频质量
					//av_opt_set(vidEncodeCtx->priv_data, "preset", "veryfast", 0);
					//av_opt_set(vidEncodeCtx->priv_data, "preset", "ultrafast", 0);    //快速编码，但会损失质量
					//av_opt_set(vidEncodeCtx->priv_data, "tune", "zerolatency", 0);  //适用于快速编码和低延迟流式传输,但是会出现绿屏
				}

				//编码器预设
				AVDictionary* param = 0;
				if (vidEncodeCtx->codec_id == AV_CODEC_ID_H264)
				{
					av_dict_set(&param, "preset", "ultrafast", 0);
					//av_dict_set(&param, "preset", "superfast", 0);
					av_dict_set(&param, "tune", "zerolatency", 0); //实现实时编码
					av_dict_set(&param, "profile", "baseline", 0);
				}
				else if (vidEncodeCtx->codec_id == AV_CODEC_ID_H265)
				{
					av_dict_set(&param, "preset", "ultrafast", 0);
					av_dict_set(&param, "tune", "zerolatency", 0);
					av_dict_set(&param, "profile", "main", 0);	// main
				}
				vidEncodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;	//@Eric - 如果要求open编码器以后AVCodecContext extradata存有 SPS,PPS 信息需要加上
				int res = avcodec_open2(vidEncodeCtx, codec, &param);
				if (res < 0) {
					char buff[128] = { 0 };
					//av_strerror(res, buff, 128);
					if (vid_codec_type_ == CT_H264)
					{
						//* 推流成功，但是B站播放不了： https://www.cnblogs.com/subo_peng/p/7800658.html
						unsigned char sps_pps[23] = { 0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0x00, 0x0a, 0xf8, 0x0f, 0x00, 0x44, 0xbe, 0x8,
							  0x00, 0x00, 0x00, 0x01, 0x68, 0xce, 0x38, 0x80 };
						codecpar->extradata_size = 23;
						codecpar->extradata = (uint8_t*)av_malloc(23 + AV_INPUT_BUFFER_PADDING_SIZE);
						if (codecpar->extradata == NULL) {
							printf("could not av_malloc the video params extradata! \r\n");
							return -1;
						}
						memcpy(codecpar->extradata, sps_pps, 23);
					}
					else if (vid_codec_type_ == CT_H265) {
					}
				}
				else {
					if (vidEncodeCtx->extradata_size > 0 && vidEncodeCtx->extradata != NULL) {
						codecpar->extradata_size = vidEncodeCtx->extradata_size;
						codecpar->extradata = (uint8_t*)av_malloc(vidEncodeCtx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
						if (codecpar->extradata == NULL) {
							printf("could not av_malloc the video params extradata! \r\n");
							return -1;
						}
						memcpy(codecpar->extradata, vidEncodeCtx->extradata, vidEncodeCtx->extradata_size);
					}
				}

				avcodec_free_context(&vidEncodeCtx);
				vidEncodeCtx = NULL;
			}
#endif
			vid_stream_->time_base = AVRational{ 1, 90000 };
		}

		if (aud_codec_type_ != CT_None)
		{//* Audio
			AVCodecID codecId = (aud_codec_type_ == CT_AAC) ? AV_CODEC_ID_AAC :
				(aud_codec_type_ == CT_MP3) ? AV_CODEC_ID_MP3 :
				(aud_codec_type_ == CT_OPUS) ? AV_CODEC_ID_OPUS : (aud_codec_type_ == CT_G711A) ? AV_CODEC_ID_PCM_ALAW : AV_CODEC_ID_NONE;
			const AVCodec* codec = avcodec_find_encoder(codecId);
			aud_stream_ = avformat_new_stream(format_context_, codec);
			AVCodecParameters* codecpar = aud_stream_->codecpar;

			codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
			codecpar->codec_id = codecId;
			codecpar->bit_rate = 64000;
			if (codecId == AV_CODEC_ID_PCM_ALAW) {
				codecpar->channels = static_cast<int>(1);
				codecpar->channel_layout = AV_CH_LAYOUT_MONO;
				codecpar->sample_rate = 8000;
				codecpar->frame_size = 4096;  // TODO: Need to Frame Size
			}
			else {
				codecpar->channels = static_cast<int>(2);
				codecpar->channel_layout = AV_CH_LAYOUT_STEREO;//AV_CH_LAYOUT_MONO
				codecpar->sample_rate = 44100;
				codecpar->frame_size = 1024;  // TODO: Need to Frame Size
			}
			codecpar->codec_tag = 0;

			// set extradata for aac_specific_config
#if 0
			if (track_info->GetExtradata() != nullptr)
			{
				codecpar->extradata_size = track_info->GetExtradata()->GetLength();
				codecpar->extradata = (uint8_t*)av_malloc(codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
				memset(codecpar->extradata, 0, codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
				memcpy(codecpar->extradata, track_info->GetExtradata()->GetDataAs<uint8_t>(), codecpar->extradata_size);
			}
#else
			AVCodecContext* audEncodeCtx = NULL;
			audEncodeCtx = avcodec_alloc_context3(codec);
			if (audEncodeCtx != NULL) {
				audEncodeCtx->sample_fmt = codec->sample_fmts[0];	// 必须一致，否则avcodec_open2会返回-22：Invalid agument
				audEncodeCtx->bit_rate = 64000;
				if (codecId == AV_CODEC_ID_PCM_ALAW) {
					audEncodeCtx->time_base = AVRational{ 1, 48000 };
					audEncodeCtx->channels = static_cast<int>(1);
					audEncodeCtx->channel_layout = AV_CH_LAYOUT_MONO;
					codecpar->sample_rate = 8000;
					codecpar->frame_size = 4096;  // TODO: Need to Frame Size
				}
				else {
					audEncodeCtx->channels = static_cast<int>(2);
					audEncodeCtx->channel_layout = AV_CH_LAYOUT_STEREO;//AV_CH_LAYOUT_MONO
					audEncodeCtx->sample_rate = 44100;
					audEncodeCtx->frame_size = 1024;  // TODO: Need to Frame Size
				}
				AVDictionary* param = 0;
				audEncodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;	//@Eric - 如果要求open编码器以后AVCodecContext extradata存有 SPS,PPS 信息需要加上
				int res = avcodec_open2(audEncodeCtx, codec, NULL);
				if (res < 0) {
					char buff[128] = { 0 };
					av_strerror(res, buff, 128);
					printf("%s\r\n", buff);
				}
				else {
					if (audEncodeCtx->extradata_size > 0 && audEncodeCtx->extradata != NULL) {
						codecpar->extradata_size = audEncodeCtx->extradata_size;
						codecpar->extradata = (uint8_t*)av_malloc(audEncodeCtx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
						if (codecpar->extradata == NULL) {
							printf("could not av_malloc the video params extradata! \r\n");
							return -1;
						}
						memcpy(codecpar->extradata, audEncodeCtx->extradata, audEncodeCtx->extradata_size);
					}
				}

				avcodec_free_context(&audEncodeCtx);
				audEncodeCtx = NULL;
			}
#endif
			aud_stream_->time_base = AVRational{ 1, 48000 };
		}
	}

	{//3, 
		AVDictionary* options = nullptr;
		// Compatibility with specific RTMP servers
		// tc_url : rtmp://[host]:[port]/[app_name]
		if (str_url_.find("rtmp://") != std::string::npos || str_url_.find("rtmps://") != std::string::npos) {
			char tc_url[1024];
			sprintf(tc_url, "%.*s", strrchr(str_url_.c_str(), '/') - str_url_.c_str(), str_url_.c_str());
			av_dict_set(&options, "rtmp_tcurl", tc_url, 0);
			av_dict_set(&options, "fflags", "flush_packets", 0);
			av_dict_set(&options, "rtmp_flashver", "FMLE/3.0 (compatible; FMSc/1.0)", 0);
		}
		else if (str_url_.find("rtsp://") != std::string::npos){
			av_dict_set(&options, "buffer_size", "4096000", 0); //设置缓存大小
			av_dict_set(&options, "rtsp_transport", "tcp", 0);  //以tcp的方式打开,
			av_dict_set(&options, "stimeout", "5000000", 0);    //设置超时断开链接时间，单位us,   5s
			av_dict_set(&options, "max_delay", "500000", 0);    //设置最大时延
		}
		else if (str_url_.find("rtp://") != std::string::npos) {
			av_dict_set(&options, "buffer_size", "4096000", 0); //设置缓存大小
			av_dict_set(&options, "rtp_transport", "tcp", 0);  //以tcp的方式打开,
			av_dict_set(&options, "stimeout", "5000000", 0);    //设置超时断开链接时间，单位us,   5s
			av_dict_set(&options, "max_delay", "500000", 0);    //设置最大时延
		}
		else {
			av_dict_set(&options, "fflags", "flush_packets", 0);
		}

		if (!(format_context_->oformat->flags & AVFMT_NOFILE))
		{
			int error = avio_open2(&format_context_->pb, format_context_->url, AVIO_FLAG_WRITE, nullptr, nullptr);
			if (error < 0)
			{
				char errbuf[256];
				av_strerror(error, errbuf, sizeof(errbuf));

				printf("Error opening file. error(%d:%s), url(%s) \r\n", error, errbuf, format_context_->url);
				Release();
				return false;
			}
		}
		int error = avformat_write_header(format_context_, &options);
		if (error < 0)
		{
			char errbuf[256];
			av_strerror(error, errbuf, sizeof(errbuf));
			printf("Could not create header \r\n");
			Release();
			return false;
		}

		av_dump_format(format_context_, 0, format_context_->url, 1);
#if 0
		if (format_context_->oformat != nullptr)
		{
			[[maybe_unused]] auto oformat = format_context_->oformat;
			printf("name : %s \r\n", oformat->name);
			printf("long_name : %s \r\n", oformat->long_name);
			printf("mime_type : %s \r\n", oformat->mime_type);
			printf("audio_codec : %d \r\n", oformat->audio_codec);
			printf("video_codec : %d \r\n", oformat->video_codec);
		}
#endif
	}

	return true;
}
void ArFFWriter::Release()
{
	if (format_context_ != nullptr)
	{
		if (format_context_->pb != nullptr)
		{
			av_write_trailer(format_context_);
			avformat_close_input(&format_context_);
		}

		avformat_free_context(format_context_);

		format_context_ = nullptr;
	}

	vid_stream_ = NULL;
	aud_stream_ = NULL;

	if (avcc_data_ != NULL) {
		delete avcc_data_;
		avcc_data_ = NULL;
	}
}