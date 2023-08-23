#include "ARFFPlayer.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "rtc_base/internal/default_socket_server.h"
#include "common_audio/signal_processing/include/signal_processing_library.h"

static uint8_t startcode[4] = { 00,00,00,01 };
#define SAMPLE_SIZE			262144
static const size_t kMaxDataSizeSamples = 3840;
static const int64_t kBitrateControllerUpdateIntervalMs = 10;
// 内部常量定义
static const AVRational TIMEBASE_MS = { 1, 1000 };
#define fftime_to_milliseconds(ts) (av_rescale(ts, 1000, AV_TIME_BASE))
#define milliseconds_to_fftime(ms) (av_rescale(ms, AV_TIME_BASE, 1000))

int16_t WebRtcSpl_MaxAbsValueW16_I(const int16_t* vector, size_t length) {
	size_t i = 0;
	int absolute = 0, maximum = 0;

	RTC_DCHECK_GT(length, 0);

	for (i = 0; i < length; i++) {
		absolute = abs((int)vector[i]);

		if (absolute > maximum) {
			maximum = absolute;
		}
	}

	// Guard the case for abs(-32768).
	if (maximum > WEBRTC_SPL_WORD16_MAX) {
		maximum = WEBRTC_SPL_WORD16_MAX;
	}

	return (int16_t)maximum;
}

class FFContex
{// 单例，兼容老版本FFmpeg的变量全局只要初始化一次即可。新的FFmpeg版本，将不需要再做类似初始化操作
protected:
	FFContex(void){
		//av_register_all();	// 新版本已经不需要
		avformat_network_init();

	};
public:
	static FFContex& Inst() {
		static FFContex gInst;
		return gInst;
	}
	virtual ~FFContex(void) {

		avformat_network_deinit();
	};
};

#if (defined(__APPLE__) && TARGET_OS_MAC || TARGET_OS_IPHONE)
//@derek AVMMediaType replace FFMAVMediaType
static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMMediaType type)
#else
static int open_codec_context(int *stream_idx,
	AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
#endif
{
	int ret, stream_index;
	AVStream *st;
	const AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		return ret;
	}
	else {
		stream_index = ret;
		st = fmt_ctx->streams[stream_index];

		/* find decoder for the stream */
		dec = avcodec_find_decoder(st->codecpar->codec_id);
		if (!dec) {
			fprintf(stderr, "Failed to find %s codec\n",
				av_get_media_type_string(type));
			return AVERROR(EINVAL);
		}

		/* Allocate a codec context for the decoder */
		*dec_ctx = avcodec_alloc_context3(dec);
		if (!*dec_ctx) {
			fprintf(stderr, "Failed to allocate the %s codec context\n",
				av_get_media_type_string(type));
			return AVERROR(ENOMEM);
		}

		/* Copy codec parameters from input stream to output codec context */
		if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(type));
			return ret;
		}

		/* Init the decoders, with or without reference counting */
		av_dict_set(&opts, "refcounted_frames", "1", 0);
		if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
			fprintf(stderr, "Failed to open %s codec\n",
				av_get_media_type_string(type));
			return ret;
		}
		*stream_idx = stream_index;
	}

	return 0;
}

static int custom_interrupt_callback(void *arg) {
	ARFFPlayer* splayer = (ARFFPlayer*)arg;
	return splayer->Timeout();
}

ARPlayer* ARP_CALL createARPlayer(ARPlayerEvent&callback)
{
	return new ARFFPlayer(callback);
}

ARFFPlayer::ARFFPlayer(ARPlayerEvent&callback)
	: ARPlayer(callback)
	, rtc::Thread(rtc::CreateDefaultSocketServer())
	, fmt_ctx_(NULL)
	, n_video_stream_idx_(-1)
	, n_audio_stream_idx_(-1)
	, n_seek_to_(0)
	, n_all_duration_(0)
	, n_cur_aud_duration_(0)
	, n_cur_vid_duration_(0)
	, n_total_track_(0)
	, b_running_(false)
	, b_open_stream_success_(false)
	, b_abort_(false)
	, b_got_eof_(false)
	, b_use_tcp_(false)
	, b_no_buffer_(false)
	, b_re_play_(false)
	, b_notify_closed_(false)
	, b_decode_video_(true)
	, n_reconnect_time_(0)
	, n_last_recv_data_time_(0)
	, n_stat_time_(0)
	, n_net_aud_band_(0)
	, n_net_vid_band_(0)
	, n_conn_pkt_time_(0)
	, n_vid_fps_(0)
	, n_vid_width_(0)
	, n_vid_height_(0)
	, video_dec_ctx_(NULL)
	, audio_dec_ctx_(NULL)
	, avframe_(NULL)
	, audio_convert_ctx_(NULL)
	, p_resamp_buffer_(NULL)
	, n_resmap_size_(0)
	, n_sample_hz_(0)
	, n_channels_(0)
	, p_audio_sample_(NULL)
	, n_audio_size_(0)
	, n_out_sample_hz_(48000)
	, n_out_channels_(2)
	, p_aud_sonic_(NULL)
{
	p_audio_sample_ = new char[SAMPLE_SIZE];

	FFContex::Inst();
}

ARFFPlayer::~ARFFPlayer()
{
	StopTask();

	delete[] p_audio_sample_;
}

int ARFFPlayer::Timeout()
{
	/*if (read_pkt_time_ != 0 && read_pkt_time_ <= rtc::Time32()) {
		return EAGAIN;
	}*/
	if ((n_conn_pkt_time_ != 0 &&n_conn_pkt_time_ <= rtc::Time32()) || b_abort_) {
		return AVERROR_EOF;
	}

	return 0;
}

//* For rtc::Thread
void ARFFPlayer::Run()
{
	int ret = 0;

	while (b_running_) {
		if (b_re_play_) {
			b_re_play_ = false;
			CloseFFDecode();
			n_seek_to_ = 0;
			n_cur_aud_duration_ = 0;
			n_cur_vid_duration_ = 0;
			n_reconnect_time_ = rtc::Time32();
		}
		if (fmt_ctx_ == NULL) {
			if (n_reconnect_time_ != 0 && n_reconnect_time_ <= rtc::Time32()) {
				n_reconnect_time_ = rtc::Time32() +  15000;	//@Eric - 1500 => 15000
			}
			else {
				rtc::Thread::SleepMs(1);
				continue;
			}
			OpenFFDecode();
			//n_seek_to_ = 0;	//@Bug - 这里不能清0，否则seek功能失效
			n_stat_time_ = 0;
			if (fmt_ctx_ != NULL) {
				if (!b_open_stream_success_) {
					b_open_stream_success_ = true;
					callback_.OnArPlyOK(this);
					if (n_all_duration_ > 0 && (n_audio_stream_idx_ != -1 || n_video_stream_idx_ != -1)) {
						callback_.OnArPlyVodProcess(this, n_all_duration_, 0, 0);
					}
				}
				n_stat_time_ = rtc::Time32() + 1000;
				if (n_seek_to_ != 0) {
					//将秒单位 转为 微秒单位
					int64_t seek = milliseconds_to_fftime(n_seek_to_ * 1000);
					n_seek_to_ = 0;
					int ret = av_seek_frame(fmt_ctx_, -1, seek, AVSEEK_FLAG_BACKWARD);
					n_stat_time_ = rtc::Time32() + 1000;
				}
			}
			else {
				if (!b_open_stream_success_) {
					CloseFFDecode();
					n_reconnect_time_ = 0;
					if (user_set_.b_repeat_ || user_set_.n_repeat_count_ > 0) {
						if (user_set_.n_repeat_count_ > 0) {
							user_set_.n_repeat_count_--;
						}
						n_reconnect_time_ = rtc::Time32();
					}
					if (n_reconnect_time_ == 0) {
						callback_.OnArPlyClose(this, -1);	// 无法打开源
						b_notify_closed_ = true;
					}
				}
			}
		}
		else {
			ReadThreadProcess();
		}

		if (n_stat_time_ != 0 && n_stat_time_ <= rtc::Time32()) {
			n_stat_time_ = rtc::Time32() + 1000;
			//callback_.OnArPlyStatus(lst_size * 20, n_net_aud_band_*(8 + 1));
			callback_.OnArPlyStatistics(this, n_vid_width_, n_vid_height_, n_vid_fps_, n_net_aud_band_, n_net_vid_band_);

			if (n_all_duration_ > 0) {
				if (n_audio_stream_idx_ != -1 || n_video_stream_idx_ != -1) {
					int nAudCacheTime = FFBuffer::AudioCacheTime();
					int nVidCacheTime = FFBuffer::VideoCacheTime();
					callback_.OnArPlyVodProcess(this, n_all_duration_, n_cur_vid_duration_> n_cur_aud_duration_? n_cur_vid_duration_ : n_cur_aud_duration_, nVidCacheTime> nAudCacheTime ? nVidCacheTime: nAudCacheTime);
				}
			}
			n_vid_fps_ = 0;
			n_net_aud_band_ = 0;
			n_net_vid_band_ = 0;
		}
		rtc::Thread::SleepMs(5);
	}

	CloseFFDecode();
	if (!b_notify_closed_) {
		callback_.OnArPlyClose(this, b_open_stream_success_ ? 0 : -1);
	}
}

bool ARFFPlayer::ReadThreadProcess()
{
	int ii = 0;
	bool needSeek = false;
	if (n_seek_to_ != 0) {
		if (n_all_duration_ > 0) {
			// 跳转核心方法 , 跳转到距离时间戳最近的关键帧位置
			if (!b_got_eof_) {
				//将秒单位 转为 微秒单位
				int64_t seek = milliseconds_to_fftime(n_seek_to_ * 1000);
				n_seek_to_ = 0;
				int ret = av_seek_frame(fmt_ctx_, -1, seek, AVSEEK_FLAG_BACKWARD);
				n_stat_time_ = rtc::Time32() + 1000;
			}
			FFBuffer::DoClear();
			needSeek = true;
		}
		else {
			n_seek_to_ = 0;
		}
	}
	if (n_last_recv_data_time_ != 0 && (n_last_recv_data_time_ + 5000) < rtc::Time32()) {
		n_last_recv_data_time_ = 0;	
		b_got_eof_ = true;	// 长时间收不到数据，模拟GetEof错误
	}
	if (b_got_eof_)
	{
		if (FFBuffer::BufferIsEmpty()) {
			CloseFFDecode();

			n_reconnect_time_ = 0;
			if (needSeek || user_set_.b_repeat_ || user_set_.n_repeat_count_ > 0) {
				if (user_set_.n_repeat_count_ > 0) {
					user_set_.n_repeat_count_--;
				}
				n_reconnect_time_ = rtc::Time32();
			}
			if (n_reconnect_time_ == 0) {
				callback_.OnArPlyClose(this, 1);
				b_notify_closed_ = true;
			}
		}
		else if (!FFBuffer::IsPlaying()) {
			//* 如果播放EOF了并且缓冲区已停止播放，此时应该清空缓冲，否则降无法继续播放
			FFBuffer::DoClear();
		}
		return false;
	}
	while (FFBuffer::NeedMoreData())
	{
		if (fmt_ctx_ != NULL) {
			int ret = 0;
			ii++;
			{
				AVPacket *packet = new AVPacket();
				av_init_packet(packet);
				packet->data = NULL;
				packet->size = 0;
				//n_conn_pkt_time_ = rtc::Time32() + 15000;
				n_conn_pkt_time_ = 0;	// 读取数据的时候不设置超时，比如m3u8会一直等待，否认会造成播放卡顿
				/* read frames from the file */
				ret = av_read_frame(fmt_ctx_, packet);
				if (ret >= 0) {
					n_last_recv_data_time_ = rtc::Time32();
					if (packet->stream_index == n_video_stream_idx_) {
						n_net_vid_band_ += packet->size;
						int64_t pts = 0;
						int64_t dts = 0;
						if (packet->dts == AV_NOPTS_VALUE) {
							dts = 0;
						}
						else {
							dts = av_rescale_q(packet->dts, astream_timebase_, TIMEBASE_MS);
						}
						if (packet->pts == AV_NOPTS_VALUE) {
							pts = 0;
						}
						else {
							pts = av_rescale_q(packet->pts, astream_timebase_, TIMEBASE_MS);
						}
						if (dts == 0 && pts != 0) {
							dts = pts;
						}
						if (pts == 0 && dts != 0) {
							pts = dts;
						}
						if (b_no_buffer_) {
							OnBufferDecodeVideoData(packet);
							av_packet_unref(packet);
							delete packet;
						}
						else {
							FFBuffer::RecvVideoData(packet, dts, pts, av_rescale_q(packet->duration, vstream_timebase_, TIMEBASE_MS));
						}
					}
					else if (packet->stream_index == n_audio_stream_idx_) {
						n_net_aud_band_ += packet->size;
						int64_t pts = 0;
						int64_t dts = 0;
						if (packet->dts == AV_NOPTS_VALUE) {
							dts = 0;
						}
						else {
							dts = av_rescale_q(packet->dts, astream_timebase_, TIMEBASE_MS);
						}
						if (packet->pts == AV_NOPTS_VALUE) {
							pts = 0;
						}
						else {
							pts = av_rescale_q(packet->pts, astream_timebase_, TIMEBASE_MS);
						}
						if (dts == 0 && pts != 0) {
							dts = pts;
						}
						if (pts == 0 && dts != 0) {
							pts = dts;
						}
						if (b_no_buffer_) {
							OnBufferDecodeAudioData(packet);
							av_packet_unref(packet);
							delete packet;
						}
						else {
							FFBuffer::RecvAudioData(packet, dts, pts, av_rescale_q(packet->duration, astream_timebase_, TIMEBASE_MS));
						}
					}
				}
				else {
					delete packet;
					if (ret == AVERROR_EOF) {
						b_got_eof_ = true;
						return false;
					}
				}
			}

			if (ii > 5) {//* 线程执行时间 (windows不是实时操作系统)所以sleep 函数精度也是在15ms左右。所以单次读取需要多一点，否则会导致FFMpeg的缓冲不能及时读出，导致卡顿
				break;
			}
		}
		else {
			break;
		}
	}
	return true;
}

int ARFFPlayer::StartTask(const char*strUrl)
{
	if (!b_running_) {
		str_play_url_ = strUrl;
		b_abort_ = false;
		n_cur_aud_duration_ = 0;
		n_cur_vid_duration_ = 0;
		b_running_ = true;
		n_reconnect_time_ = rtc::Time32();
		rtc::Thread::SetName("ARFFPlayer", this);
		rtc::Thread::Start();
	}

	return 0;
}

void ARFFPlayer::RunOnce()
{
	FFBuffer::DoTick();

	while (callback_.OnArPlyNeedMoreAudioData(this)) {
		if (!FFBuffer::DoDecodeAudio()) {
			break;
		}
	}
	while (callback_.OnArPlyNeedMoreVideoData(this)) {
		if (!FFBuffer::DoDecodeVideo(callback_.OnArPlyAppIsBackground(this))) {
			break;
		}
	}
}
void ARFFPlayer::Play()
{
}
void ARFFPlayer::Pause()
{
}
void ARFFPlayer::StopTask()
{
	if (b_running_) {
		b_running_ = false;
		b_abort_ = true;
		n_reconnect_time_ = 0;
		user_set_.b_repeat_ = false;
		rtc::Thread::Stop();
	}

	FFBuffer::DoClear();

	if (p_aud_sonic_ != NULL) {
		sonicDestroyStream(p_aud_sonic_);
		p_aud_sonic_ = NULL;
	}
}

void ARFFPlayer::SetOutputAudioParam(int nSampleHz, int nChannels)
{
	n_out_sample_hz_ = nSampleHz;
	n_out_channels_ = nChannels;
}
void ARFFPlayer::SetAudioEnable(bool bAudioEnable)
{
	user_set_.b_audio_enabled_ = bAudioEnable;
}
void ARFFPlayer::SetVideoEnable(bool bVideoEnable)
{
	user_set_.b_video_enabled_ = bVideoEnable;
}
void ARFFPlayer::SetRepeat(bool bEnable)
{
	user_set_.b_repeat_ = bEnable;
}
void ARFFPlayer::SetUseTcp(bool bUseTcp)
{
	b_use_tcp_ = bUseTcp;
}
void ARFFPlayer::SetNoBuffer(bool bNoBuffer)
{
	b_no_buffer_ = bNoBuffer;
}
void ARFFPlayer::SetRepeatCount(int loopCount)
{
	user_set_.n_repeat_count_ = 0;
	if (loopCount < 0) {
		SetRepeat(true);
	}
	else if(loopCount >= 0){
		user_set_.n_repeat_count_ = loopCount;
	}
}
void ARFFPlayer::SeekTo(int nSeconds)
{
	n_seek_to_ = nSeconds;
}
void ARFFPlayer::SetSpeed(float fSpeed)
{
}
void ARFFPlayer::SetVolume(float fVolume)
{
	user_set_.f_aud_volume_ = fVolume;
}
void ARFFPlayer::EnableVolumeEvaluation(int32_t intervalMs)
{
	user_set_.n_volume_evaluation_interval_ms_ = intervalMs;
	user_set_.n_volume_evaluation_used_ms_ = 0;
}
int ARFFPlayer::GetTotalDuration()
{
	return n_all_duration_;
}
void ARFFPlayer::RePlay()
{
	b_re_play_ = true;
}
void ARFFPlayer::Config(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold)
{
	FFBuffer::SetPlaySetting(bAuto, nCacheTime, nMinCacheTime, nMaxCacheTime, nVideoBlockThreshold);
}
void ARFFPlayer::selectAudioTrack(int index)
{
	int total = 0;
	AVFormatContext *ic = fmt_ctx_;
	int start_index = 0, stream_index = 0;
	AVStream *st;
	int codec_type = AVMEDIA_TYPE_AUDIO;

	if (codec_type == AVMEDIA_TYPE_VIDEO)
		start_index = n_video_stream_idx_;
	else if (codec_type == AVMEDIA_TYPE_AUDIO)
		start_index = n_audio_stream_idx_;
	/*else
	 start_index = is->subtitle_stream;
	 if (start_index < (codec_type == AVMEDIA_TYPE_SUBTITLE ? -1 : 0))
	 return;*/

	for (stream_index = 0; stream_index < ic->nb_streams; stream_index++)
	{
		st = ic->streams[stream_index];
		if (st->codecpar->codec_type == codec_type) {
			/* check that parameters are OK */
			switch (codec_type) {
			case AVMEDIA_TYPE_AUDIO: {
				if (st->codecpar->sample_rate != 0 && st->codecpar->channels != 0) {
					total++;
				}

				if (total == index)
				{
					//stream_component_close(ffp, start_index);
					//传递两个参数tracksNum：音轨的个数，stream_index：第几个音轨
					//ffp_set_stream_selected(ffp, tracksNum, stream_index);
					return;
				}
			}break;
			}
		}
	}

	return;
}
int ARFFPlayer::getAudioTrackCount()
{
	return 0;
}

//* For FFBuffer
void ARFFPlayer::OnBufferDecodeAudioData(AVPacket* aud_packet)
{
	AVPacket &pkt = *aud_packet;
	uint8_t*ptr = pkt.data;
	int len = pkt.size;
	int frameFinished = 0;

	{
		webrtc::MutexLock l(&cs_ff_ctx_);
		if (audio_dec_ctx_ != NULL) {
#if 0
			int ret = avcodec_decode_audio4(audio_dec_ctx_, avframe_, &frameFinished, &pkt);
#else
			int ret = avcodec_send_packet(audio_dec_ctx_, &pkt);
			if (ret < 0) {
				RTC_LOG(LS_ERROR) << "avcodec_send_packet error: " << ret;
			}
			else {
				ret = avcodec_receive_frame(audio_dec_ctx_, avframe_);
				if (ret < 0) {
					RTC_LOG(LS_ERROR) << "avcodec_receive_frame error: " << ret;
				}
				else {
					frameFinished = 1;
				}
			}
#endif 
			if (ret >= 0) {
				int64_t pts = 0;
				if (frameFinished) {
					//@Eric - 20230602 - 实际解码出来的音频采样率与StreamInfo中的不一样，需要重新创建SwrContext
					if (avframe_->sample_rate != n_sample_hz_ || avframe_->channels != n_channels_) {
						n_sample_hz_ = avframe_->sample_rate;
						n_channels_ = avframe_->channels;
						int channel_layout = av_get_default_channel_layout(n_channels_);//设置声道布局
						if (audio_convert_ctx_ != NULL) {
							swr_free(&audio_convert_ctx_);
							audio_convert_ctx_ = NULL;
						}
						audio_convert_ctx_ = swr_alloc();
						audio_convert_ctx_ = swr_alloc_set_opts(audio_convert_ctx_, av_get_default_channel_layout(n_out_channels_), AV_SAMPLE_FMT_S16, n_out_sample_hz_,
							channel_layout, audio_dec_ctx_->sample_fmt, n_sample_hz_, 0, NULL);//配置源音频参数和目标音频参数 
						swr_init(audio_convert_ctx_);
					}
					int out_channels = n_out_channels_;// av_get_channel_layout_nb_channels(audio_dec_ctx_->channel_layout);
					int need_size = (n_out_sample_hz_* out_channels * sizeof(int16_t)) / 100;
					//avframe_->pts = av_rescale_q(av_frame_get_best_effort_timestamp(avframe_), astream_timebase_, TIMEBASE_MS);
					avframe_->pts = av_rescale_q(avframe_->best_effort_timestamp, astream_timebase_, TIMEBASE_MS);
					//RTC_LOG(LS_ERROR) << "Audio pts: " << pkt.pts;
					pts = avframe_->pts;
					/* if a frame has been decoded, output it */
					int data_size = av_get_bytes_per_sample(audio_dec_ctx_->sample_fmt);
					if (data_size > 0) {
						int samples = swr_convert(audio_convert_ctx_, &p_resamp_buffer_, n_resmap_size_, (const uint8_t **)avframe_->data, avframe_->nb_samples);
						if (samples > 0) {
							int resampled_data_size = samples * out_channels  * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);//每声道采样数 x 声道数 x 每个采样字节数   

							memcpy(p_audio_sample_ + n_audio_size_, p_resamp_buffer_, resampled_data_size);
							pts -= (n_audio_size_ * 10) / need_size;
							n_audio_size_ += resampled_data_size;
						}
					}
					{
						//获取当前音频的相对播放时间 , 相对 : 即从播放开始到现在的时间
						//  该值大多数情况下 , 与 pts 值是相同的
						//  该值比 pts 更加精准 , 参考了更多的信息
						//  转换成秒 : 这里要注意 pts 需要转成 秒 , 需要乘以 time_base 时间单位
						//  其中 av_q2d 是将 AVRational 转为 double 类型
						double audio_best_effort_timestamp_second = avframe_->best_effort_timestamp * av_q2d(astream_timebase_);
						n_cur_aud_duration_ = audio_best_effort_timestamp_second * 1000;
					}
					av_frame_unref(avframe_);

					while (n_audio_size_ >= need_size) {
						GotAudioFrame(p_audio_sample_, need_size, n_out_sample_hz_, out_channels, pts);
						pts += 10;
						n_audio_size_ -= need_size;
						if (n_audio_size_ > 0) {
							memmove(p_audio_sample_, p_audio_sample_ + need_size, n_audio_size_);
						}
					}
				}
			}
			else {
				char errBuf[1024] = { 0 };
				av_strerror(ret, errBuf, 1024);
			}
		}
	}
}
void ARFFPlayer::OnBufferDecodeVideoData(AVPacket* vid_packet)
{
	{
		int64_t pts = av_rescale_q(vid_packet->dts, vstream_timebase_, TIMEBASE_MS);
		//printf("Time: %d pts:%lld \r\n", rtc::Time32(), pts);
		if (vid_packet->flags & AV_PKT_FLAG_KEY) {
			ParseVideoSei((char*)vid_packet->data, vid_packet->size, pts);
		}
	}
	webrtc::MutexLock l(&cs_ff_ctx_);
	if (video_dec_ctx_ != NULL) {
		{//* Decode?
			int frameFinished = 0;
#if 0
			int ret = avcodec_decode_video2(video_dec_ctx_, avframe_, &frameFinished, vid_packet);
#else
			int ret = avcodec_send_packet(video_dec_ctx_, vid_packet);
			if (ret < 0) {
				RTC_LOG(LS_ERROR) << "avcodec_send_packet error: " << ret;
			}
			else {
				ret = avcodec_receive_frame(video_dec_ctx_, avframe_);
				if (ret < 0) {
					RTC_LOG(LS_ERROR) << "avcodec_receive_frame error: " << ret;
				}
				else {
					frameFinished = 1;
				}
			}
#endif
			if (ret >= 0) {
				if (frameFinished) {
					//avframe_->pts = av_rescale_q(av_frame_get_best_effort_timestamp(avframe_), vstream_timebase_, TIMEBASE_MS);
					avframe_->pts = av_rescale_q(avframe_->best_effort_timestamp, vstream_timebase_, TIMEBASE_MS);
					if (b_no_buffer_) {
						avframe_->pts = 0;
					}
					{
						//获取当前画面的相对播放时间 , 相对 : 即从播放开始到现在的时间
						//  该值大多数情况下 , 与 pts 值是相同的
						//  该值比 pts 更加精准 , 参考了更多的信息
						//  转换成秒 : 这里要注意 pts 需要转成 秒 , 需要乘以 time_base 时间单位
						//  其中 av_q2d 是将 AVRational 转为 double 类型
						double video_best_effort_timestamp_second = avframe_->best_effort_timestamp * av_q2d(vstream_timebase_);
						n_cur_vid_duration_ = video_best_effort_timestamp_second * 1000;
					}
					n_vid_fps_++;
					n_vid_width_ = avframe_->width;
					n_vid_height_ = avframe_->height;
					callback_.OnArPlyVideo(this, avframe_->format, avframe_->width, avframe_->height, avframe_->data, avframe_->linesize, avframe_->pts);
				}
			}
		}
	}
}
void ARFFPlayer::OnBufferStatusChanged(PlayStatus playStatus)
{
	if (playStatus == PS_Caching) {
		callback_.OnArPlyCacheing(this, user_set_.b_audio_enabled_, user_set_. b_video_enabled_);
	}
	else if (playStatus == PS_Playing) {
		callback_.OnArPlyPlaying(this, user_set_.b_audio_enabled_, user_set_.b_video_enabled_);
	}
}
bool ARFFPlayer::OnBufferIsKeyFrame(AVPacket* pkt)
{
	if (pkt->flags & AV_PKT_FLAG_KEY) {
		return true;
	}
	return false;
}
bool ARFFPlayer::OnBufferGetPuased()
{
	return !user_set_.b_audio_enabled_ && !user_set_.b_video_enabled_;
}
float ARFFPlayer::OnBufferGetSpeed()
{
	if (n_all_duration_ <= 0) {// 必须是点播才能处理倍速
		return 1.0;
	}
	return 1.0;
}

void ARFFPlayer::GotAudioFrame(char* pdata, int len, int sample_hz, int channels, int64_t timestamp)
{
	if (user_set_.f_aud_volume_ != 1.0) {
		if (p_aud_sonic_ == NULL) {
			p_aud_sonic_ = sonicCreateStream(sample_hz, channels);
		}
	}
	else {
		if (p_aud_sonic_ != NULL) {
			sonicDestroyStream(p_aud_sonic_);
			p_aud_sonic_ = NULL;
		}
	}
	if (user_set_.n_volume_evaluation_interval_ms_ > 0) {
		user_set_.n_volume_evaluation_used_ms_ += 10;
		if (user_set_.n_volume_evaluation_used_ms_ >= user_set_.n_volume_evaluation_interval_ms_) {
			user_set_.n_volume_evaluation_used_ms_ = 0;
			int max_abs = WebRtcSpl_MaxAbsValueW16_I((int16_t*)(pdata), len / sizeof(int16_t));
			max_abs = (max_abs * 100) / 32767;
			callback_.OnArPlyVolumeUpdate(this, max_abs);
		}
	}
	// 必须是点播才能处理倍速
	if (p_aud_sonic_ != NULL) {
		sonicSetVolume(p_aud_sonic_, user_set_.f_aud_volume_);
		int audFrame10ms = len / sizeof(short) / channels;
		sonicWriteShortToStream(p_aud_sonic_, (short*)pdata, audFrame10ms);
		int ava = sonicSamplesAvailable(p_aud_sonic_);
		//RTC_LOG(LS_INFO) << "sonicWriteShortToStream: " << audFrame10ms << " ava: " << ava;
		while (sonicSamplesAvailable(p_aud_sonic_) >= audFrame10ms) {
			memset(pdata, 0, len);
			int rd = sonicReadShortFromStream(p_aud_sonic_, (short*)pdata, audFrame10ms);
			//RTC_LOG(LS_INFO) << "sonicReadShortFromStream: " << rd;
			if (rd > 0) {
				callback_.OnArPlyAudio(this, pdata, sample_hz, channels, timestamp);
			}
			else {
				break;
			}
		}
	}
	else {
		callback_.OnArPlyAudio(this, pdata, sample_hz, channels, timestamp);
	}
}

void ARFFPlayer::ParseVideoSei(char* pData, int nLen, int64_t pts)
{
	char* ptr = pData;
	if (ptr[0] == 0x00 && ptr[1] == 0x00 && ptr[2] == 0x00 && ptr[3] == 0x01) {
		int limLen = 0;
		int ii = 0;
		char* ptrFind6 = NULL;
		while (ii <= nLen - 4) {
			limLen = 0;
			if (ptr[ii] == 0 && ptr[ii + 1] == 0 && ptr[ii + 2] == 0 && ptr[ii + 3] == 1) {
				limLen = 4;
			}
			else if (ptr[ii] == 0 && ptr[ii + 1] == 0 && ptr[ii + 2] == 1) {
				limLen = 3;
			}
			if (limLen > 0) {
				ii += limLen;
				int nType = ptr[ii] & 0x1f;
				if (nType == 6) {
					ptrFind6 = ptr + ii - limLen;
				}
				else {
					char* ptrFindN = ptr + ii - limLen;
					if (ptrFind6 != NULL) {
						callback_.OnArPlySeiData(this, ptrFind6, ptrFindN - ptrFind6, pts);
						break;
					}
				}
			}
			else {
				ii++;
			}
		}
		if (ptrFind6 != NULL) {
			callback_.OnArPlySeiData(this, ptrFind6, (pData + nLen) - ptrFind6, pts);
		}
	}
	else {
		int nPtr = 0;
		while (nPtr < nLen) {
			// 1. first we check NAL's size, valid size should less than 192K = 0x030000
			if (ptr[0] != 0x00 || ptr[1] >= 0x03)
			{
				return;
			}
			int n1 = (int)(ptr[1] & 0xff) << 16;
			int n2 = (int)(ptr[2] & 0xff) << 8;
			int n3 = (int)(ptr[3] & 0xff);
			unsigned int vpkg_len = n1 + n2 + n3;	//

			// 2. check NAL header including NAL start and type,
			//    only nal_unit_type = 1 and 5 are selected
			//    nal_ref_idc > 0
			int nType = ptr[4] & 0x1f;
			if (nType == 6) {
				char naluHdr[4] = { 0x00, 0x00, 0x00, 0x01 };

				int nRawLen = 0;
				char* pH264Raw = new char[vpkg_len + 4];
				memcpy(pH264Raw + nRawLen, naluHdr, 4);
				nRawLen += 4;
				memcpy(pH264Raw + nRawLen, ptr + 4, vpkg_len);
				nRawLen += vpkg_len;

				callback_.OnArPlySeiData(this, pH264Raw, vpkg_len + 4, pts);
				break;
			}
			//LOG(LS_ERROR) << "Pkt type: " << (ptr[4] & 0x1f) << " len: " << vpkg_len;
			//callback_.OnH264RawData((char*)ptr, 4 + vpkg_len);
			ptr += (4 + vpkg_len);
			nPtr += (4 + vpkg_len);
		}
	}
}

void ARFFPlayer::OpenFFDecode()
{
	webrtc::MutexLock l(&cs_ff_ctx_);
	if (fmt_ctx_ == NULL) {
		fmt_ctx_ = avformat_alloc_context();
		fmt_ctx_->interrupt_callback.callback = custom_interrupt_callback;
		fmt_ctx_->interrupt_callback.opaque = this;

		int ret = 0;
		/* open input file, and allocate format context */
		n_conn_pkt_time_ = rtc::Time32() + 10000;
		AVDictionary* options = NULL;
		av_dict_set(&options, "nobuffer", "1", 0);
		if (str_play_url_.find("rtmp://") != std::string::npos) {
			av_dict_set(&options, "timeout", NULL, 0);
		}
		if (str_play_url_.find("rtsp://") != std::string::npos) {
			if (b_use_tcp_) {
				av_dict_set(&options, "rtsp_transport", "tcp", 0);
			}
			else {
				av_dict_set(&options, "rtsp_transport", "udp", 0);
			}
			//av_dict_set(&options, "timeout", NULL, 0);
			av_dict_set(&options, "stimeout", "3000000", 0);//设置超时3秒
		}

		if (str_play_url_.find("http://") != std::string::npos || str_play_url_.find("https://") != std::string::npos) {
			// av_read_frame返回-5： https://blog.csdn.net/qq_42956179/article/details/124453345
			//av_dict_set(&options, "stimeout", std::to_string(1000000).c_str(), 0);
			av_dict_set(&options, "timeout", "3000000", 0);//设置超时3秒
			av_dict_set_int(&options, "multiple_requests", 1, 0);
			av_dict_set_int(&options, "read_ahead_limit", INT_MAX, 0);
		}

		if ((ret = avformat_open_input(&fmt_ctx_, str_play_url_.c_str(), NULL, &options)) < 0) {
			char strErr[1024];
			av_strerror(ret, strErr, 1024);
			printf("Could not open source (%d) url %s\n", ret, str_play_url_.c_str());
			return;
		}
		n_all_duration_ = fftime_to_milliseconds(fmt_ctx_->duration);	//duration单位是us，转化为毫秒
		if (n_all_duration_ < 0) {
			n_all_duration_ = 0;
		}
        fmt_ctx_->probesize = 128 *1024;
        fmt_ctx_->max_analyze_duration = 1 * AV_TIME_BASE;
		//在avformat_find_stream_info方法中，会判断AVFMT_FLAG_NOBUFFER，而是否添加到buffer中
		//fmt_ctx_->flags |= AVFMT_FLAG_NOBUFFER;
		//fmt_ctx_->flags |= AVFMT_FLAG_DISCARD_CORRUPT;
		/* retrieve stream information */
		if (avformat_find_stream_info(fmt_ctx_, NULL) < 0) {
			printf("Could not find stream information\n");
			avformat_close_input(&fmt_ctx_);
			fmt_ctx_ = NULL;
			return;
		}
		{// Get audio Track total
			int total = 0;
			int codec_type = AVMEDIA_TYPE_AUDIO;
			for (int stream_index = 0; stream_index < fmt_ctx_->nb_streams; stream_index++)
			{
				AVStream* st = fmt_ctx_->streams[stream_index];
				if (st->codecpar->codec_type == codec_type) {
					/* check that parameters are OK */
					switch (codec_type) {
					case AVMEDIA_TYPE_AUDIO:
					{
						if (st->codecpar->sample_rate != 0 && st->codecpar->channels != 0) {
							total++;
						}
					}break;
					}
				}
			}
			n_total_track_ = total;
		}

		if (open_codec_context(&n_video_stream_idx_, &video_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_VIDEO) >= 0) {
			//video_stream_ = fmt_ctx_->streams[n_video_stream_idx_];
			vstream_timebase_ = fmt_ctx_->streams[n_video_stream_idx_]->time_base;
		}
        else {
            n_video_stream_idx_ = -1;
        }

		if (open_codec_context(&n_audio_stream_idx_, &audio_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_AUDIO) >= 0) {
			//audio_stream_ = fmt_ctx_->streams[n_audio_stream_idx_];
			astream_timebase_ = fmt_ctx_->streams[n_audio_stream_idx_]->time_base;

			n_sample_hz_ = audio_dec_ctx_->sample_rate;
			n_channels_ = audio_dec_ctx_->channels;
			/*if (audio_dec_ctx_->channel_layout == 0) {
				audio_dec_ctx_->channel_layout = AV_CH_LAYOUT_MONO;
			}*/
			//@Bug: FFMPEG 解码WAV 提取不出数据
			if (audio_dec_ctx_->channels > 0 && audio_dec_ctx_->channel_layout == 0) { //有声道数没有声道布局，所以要设置声道布局
				audio_dec_ctx_->channel_layout = av_get_default_channel_layout(audio_dec_ctx_->channels);//设置声道布局
			}
			else if (audio_dec_ctx_->channels == 0 && audio_dec_ctx_->channel_layout > 0) {//有声道布局没有声道数，所以要设置声道数
				audio_dec_ctx_->channels = av_get_channel_layout_nb_channels(audio_dec_ctx_->channel_layout);
			}
			//Swr  
			audio_convert_ctx_ = swr_alloc();
			//audio_convert_ctx_ = swr_alloc_set_opts(audio_convert_ctx_, audio_dec_ctx_->channel_layout, AV_SAMPLE_FMT_S16, n_out_sample_hz_,
			//	audio_dec_ctx_->channel_layout, audio_dec_ctx_->sample_fmt, audio_dec_ctx_->sample_rate, 0, NULL);//配置源音频参数和目标音频参数 
			audio_convert_ctx_ = swr_alloc_set_opts(audio_convert_ctx_, av_get_default_channel_layout(n_out_channels_), AV_SAMPLE_FMT_S16, n_out_sample_hz_,
				audio_dec_ctx_->channel_layout, audio_dec_ctx_->sample_fmt, audio_dec_ctx_->sample_rate, 0, NULL);//配置源音频参数和目标音频参数 
			swr_init(audio_convert_ctx_);
			int frame_size = (audio_dec_ctx_->frame_size == 0 ? 4096 : audio_dec_ctx_->frame_size) * 8;
			n_resmap_size_ = av_samples_get_buffer_size(NULL, av_get_channel_layout_nb_channels(audio_dec_ctx_->channel_layout), frame_size, audio_dec_ctx_->sample_fmt/*AV_SAMPLE_FMT_S16*/, 1);//计算转换后数据大小  
			p_resamp_buffer_ = (uint8_t *)av_malloc(n_resmap_size_);//申请输出缓冲区  
		}
        else {
            n_audio_stream_idx_ = -1;
        }

		/* dump input information to stderr */
		av_dump_format(fmt_ctx_, 0, str_play_url_.c_str(), 0);

		if (avframe_ == NULL) {
			avframe_ = av_frame_alloc();
		}

		b_got_eof_ = false;
	}
}
void ARFFPlayer::CloseFFDecode()
{
	FFBuffer::DoClear();
	n_reconnect_time_ = 0;
	n_last_recv_data_time_ = 0;

	webrtc::MutexLock l(&cs_ff_ctx_);
	if (video_dec_ctx_ != NULL) {
		avcodec_close(video_dec_ctx_);
		video_dec_ctx_ = NULL;
	}
	if (audio_dec_ctx_ != NULL) {
		avcodec_close(audio_dec_ctx_);
		audio_dec_ctx_ = NULL;
	}

	if (fmt_ctx_ != NULL) {
		avformat_close_input(&fmt_ctx_);
		fmt_ctx_ = NULL;
	}

	if (avframe_ != NULL) {
		av_frame_free(&avframe_);
		avframe_ = NULL;
	}
	if (audio_convert_ctx_ != NULL) {
		swr_free(&audio_convert_ctx_);
		audio_convert_ctx_ = NULL;
	}
}
