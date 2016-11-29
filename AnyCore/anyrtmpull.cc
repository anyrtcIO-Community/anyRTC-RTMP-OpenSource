/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#include "anyrtmpull.h"
#include "srs_librtmp.h"
#include "webrtc/base/logging.h"

#ifndef _WIN32
#define ERROR_SUCCESS   0
#endif
#define MAX_RETRY_TIME  3
static u_int8_t fresh_nalu_header[] = { 0x00, 0x00, 0x00, 0x01 };
static u_int8_t cont_nalu_header[] = { 0x00, 0x00, 0x01 };

AnyRtmpPull::AnyRtmpPull(AnyRtmpPullCallback&callback, const std::string&url)
	: callback_(callback)
	, srs_codec_(NULL)
	, running_(false)
    , connected_(false)
	, retry_ct_(0)
	, rtmp_status_(RS_PLY_Init)
	, rtmp_(NULL)
	, audio_payload_(NULL)
	, video_payload_(NULL)
{
	str_url_ = url;
	rtmp_ = srs_rtmp_create(url.c_str());
	srs_codec_ = new SrsAvcAacCodec();

	audio_payload_ = new DemuxData(1024);
	video_payload_ = new DemuxData(384 * 1024);

	running_ = true;
	rtc::Thread::Start();
}

AnyRtmpPull::~AnyRtmpPull(void)
{
	running_ = false;
	rtmp_status_ = RS_PLY_Closed;
	rtc::Thread::SleepMs(100);
	{
		rtc::CritScope l(&cs_rtmp_);
		if (rtmp_) {
			srs_rtmp_disconnect_server(rtmp_);
		}
	}
	rtc::Thread::Stop();
	if (rtmp_) {
		srs_rtmp_destroy(rtmp_);
		rtmp_ = NULL;
	}

	if (srs_codec_) {
		delete srs_codec_;
		srs_codec_ = NULL;
	}
	if (audio_payload_) {
		delete audio_payload_;
		audio_payload_ = NULL;
	}
	if (video_payload_) {
		delete video_payload_;
		video_payload_ = NULL;
	}
}

//* For Thread
void AnyRtmpPull::Run()
{
	while (running_)
	{
		{// ProcessMessages
			this->ProcessMessages(10);
		}

		if (rtmp_ != NULL)
		{
			switch (rtmp_status_) {
			case RS_PLY_Init:
			{
				if (srs_rtmp_handshake(rtmp_) == 0) {
					srs_human_trace("SRS: simple handshake ok.");
					rtmp_status_ = RS_PLY_Handshaked;
				}
                else {
                    CallDisconnect();
                }
			}
				break;
			case RS_PLY_Handshaked:
			{
				if (srs_rtmp_connect_app(rtmp_) == 0) {
					srs_human_trace("SRS: connect vhost/app ok.");
					rtmp_status_ = RS_PLY_Connected;
				}
                else {
                    CallDisconnect();
                }
			}
				break;
			case RS_PLY_Connected:
			{
				if (srs_rtmp_play_stream(rtmp_) == 0) {
					srs_human_trace("SRS: play stream ok.");
					rtmp_status_ = RS_PLY_Played;
					CallConnect();
				}
                else {
                    CallDisconnect();
                }
			}
				break;
			case RS_PLY_Played:
			{
				DoReadData();
			}
				break;
			}
		}
	}
}

void AnyRtmpPull::DoReadData()
{
	int size;
	char type;
	char* data;
	u_int32_t timestamp;

	if (srs_rtmp_read_packet(rtmp_, &type, &timestamp, &data, &size) != 0) {

	}
	if (type == SRS_RTMP_TYPE_VIDEO) {
		SrsCodecSample sample;
		if (srs_codec_->video_avc_demux(data, size, &sample) == ERROR_SUCCESS) {
			if (srs_codec_->video_codec_id == SrsCodecVideoAVC) {	// Jus support H264
				GotVideoSample(timestamp, &sample);
			}
			else {
				LOG(LS_ERROR) << "Don't support video format!";
			}
		}
	}
	else if (type == SRS_RTMP_TYPE_AUDIO) {
		SrsCodecSample sample;
		if (srs_codec_->audio_aac_demux(data, size, &sample) != ERROR_SUCCESS) {
			if (sample.acodec == SrsCodecAudioMP3 && srs_codec_->audio_mp3_demux(data, size, &sample) != ERROR_SUCCESS) {
				free(data);
				return;
			}
			free(data);
			return;	// Just support AAC.
		}
		SrsCodecAudio acodec = (SrsCodecAudio)srs_codec_->audio_codec_id;

		// ts support audio codec: aac/mp3
		if (acodec != SrsCodecAudioAAC && acodec != SrsCodecAudioMP3) {
			free(data);
			return;
		}
		// for aac: ignore sequence header
		if (acodec == SrsCodecAudioAAC && sample.aac_packet_type == SrsCodecAudioTypeSequenceHeader 
			|| srs_codec_->aac_object == SrsAacObjectTypeReserved) {
			free(data);
			return;
		}
		GotAudioSample(timestamp, &sample);
	}
	else if (type == SRS_RTMP_TYPE_SCRIPT) {
		if (!srs_rtmp_is_onMetaData(type, data, size)) {
			LOG(LS_ERROR) << "No flv";
			srs_human_trace("drop message type=%#x, size=%dB", type, size);
		}
	}

	//if (srs_human_print_rtmp_packet(type, timestamp, data, size) != 0) {	
	//}
	free(data);
}

int AnyRtmpPull::GotVideoSample(u_int32_t timestamp, SrsCodecSample *sample)
{
	int ret = ERROR_SUCCESS;
	// ignore info frame,
	// @see https://github.com/simple-rtmp-server/srs/issues/288#issuecomment-69863909
	if (sample->frame_type == SrsCodecVideoAVCFrameVideoInfoFrame) {
		return ret;
	}

	// ignore sequence header
	if (sample->frame_type == SrsCodecVideoAVCFrameKeyFrame
		&& sample->avc_packet_type == SrsCodecVideoAVCTypeSequenceHeader) {
		return ret;
	}

	// when ts message(samples) contains IDR, insert sps+pps.
	if (sample->has_idr) {
		// fresh nalu header before sps.
		if (srs_codec_->sequenceParameterSetLength > 0) {
			video_payload_->append((const char*)fresh_nalu_header, 4);
			// sps
			video_payload_->append(srs_codec_->sequenceParameterSetNALUnit, srs_codec_->sequenceParameterSetLength);
		}
		// cont nalu header before pps.
		if (srs_codec_->pictureParameterSetLength > 0) {
			video_payload_->append((const char*)fresh_nalu_header, 4);
			// pps
			video_payload_->append(srs_codec_->pictureParameterSetNALUnit, srs_codec_->pictureParameterSetLength);
		}
	}

	// all sample use cont nalu header, except the sps-pps before IDR frame.
	for (int i = 0; i < sample->nb_sample_units; i++) {
		SrsCodecSampleUnit* sample_unit = &sample->sample_units[i];
		int32_t size = sample_unit->size;

		if (!sample_unit->bytes || size <= 0) {
			ret = -1;
			return ret;
		}
        

		// 5bits, 7.3.1 NAL unit syntax,
		// H.264-AVC-ISO_IEC_14496-10-2012.pdf, page 83.
		SrsAvcNaluType nal_unit_type = (SrsAvcNaluType)(sample_unit->bytes[0] & 0x1f);

		// ignore SPS/PPS/AUD
		switch (nal_unit_type) {
		case SrsAvcNaluTypeSPS:
		case SrsAvcNaluTypePPS:
		case SrsAvcNaluTypeSEI:
		case SrsAvcNaluTypeAccessUnitDelimiter:
			continue;
		default: {
            if (nal_unit_type == SrsAvcNaluTypeReserved) {
                RescanVideoframe(sample_unit->bytes, sample_unit->size, timestamp);
                continue;
            }
        }
			break;
		}

		if (nal_unit_type == SrsAvcNaluTypeIDR) {
			// insert cont nalu header before frame.
#ifdef WEBRTC_IOS
            video_payload_->append((const char*)fresh_nalu_header, 4);
#else
			video_payload_->append((const char*)cont_nalu_header, 3);
#endif
		}
		else {
			video_payload_->append((const char*)fresh_nalu_header, 4);
		}

		// sample data
		video_payload_->append(sample_unit->bytes, sample_unit->size);

		callback_.OnRtmpullH264Data((uint8_t*)video_payload_->_data, video_payload_->_data_len, timestamp);
		video_payload_->reset();
	}

	return ret;
}
int AnyRtmpPull::GotAudioSample(u_int32_t timestamp, SrsCodecSample *sample)
{
	int ret = ERROR_SUCCESS;
	for (int i = 0; i < sample->nb_sample_units; i++) {
		SrsCodecSampleUnit* sample_unit = &sample->sample_units[i];
		int32_t size = sample_unit->size;

		if (!sample_unit->bytes || size <= 0 || size > 0x1fff) {
			ret = -1;
			return ret;
		}

		// the frame length is the AAC raw data plus the adts header size.
		int32_t frame_length = size + 7;

		// AAC-ADTS
		// 6.2 Audio Data Transport Stream, ADTS
		// in aac-iso-13818-7.pdf, page 26.
		// fixed 7bytes header
		u_int8_t adts_header[7] = { 0xff, 0xf9, 0x00, 0x00, 0x00, 0x0f, 0xfc };
		/*
		// adts_fixed_header
		// 2B, 16bits
		int16_t syncword; //12bits, '1111 1111 1111'
		int8_t ID; //1bit, '1'
		int8_t layer; //2bits, '00'
		int8_t protection_absent; //1bit, can be '1'
		// 12bits
		int8_t profile; //2bit, 7.1 Profiles, page 40
		TSAacSampleFrequency sampling_frequency_index; //4bits, Table 35, page 46
		int8_t private_bit; //1bit, can be '0'
		int8_t channel_configuration; //3bits, Table 8
		int8_t original_or_copy; //1bit, can be '0'
		int8_t home; //1bit, can be '0'

		// adts_variable_header
		// 28bits
		int8_t copyright_identification_bit; //1bit, can be '0'
		int8_t copyright_identification_start; //1bit, can be '0'
		int16_t frame_length; //13bits
		int16_t adts_buffer_fullness; //11bits, 7FF signals that the bitstream is a variable rate bitstream.
		int8_t number_of_raw_data_blocks_in_frame; //2bits, 0 indicating 1 raw_data_block()
		*/
		// profile, 2bits
		SrsAacProfile aac_profile = srs_codec_aac_rtmp2ts(srs_codec_->aac_object);
		adts_header[2] = (aac_profile << 6) & 0xc0;
		// sampling_frequency_index 4bits
		adts_header[2] |= (srs_codec_->aac_sample_rate << 2) & 0x3c;
		// channel_configuration 3bits
		adts_header[2] |= (srs_codec_->aac_channels >> 2) & 0x01;
		adts_header[3] = (srs_codec_->aac_channels << 6) & 0xc0;
		// frame_length 13bits
		adts_header[3] |= (frame_length >> 11) & 0x03;
		adts_header[4] = (frame_length >> 3) & 0xff;
		adts_header[5] = ((frame_length << 5) & 0xe0);
		// adts_buffer_fullness; //11bits
		adts_header[5] |= 0x1f;

		// copy to audio buffer
		audio_payload_->append((const char*)adts_header, sizeof(adts_header));
		audio_payload_->append(sample_unit->bytes, sample_unit->size);

		callback_.OnRtmpullAACData((uint8_t*)audio_payload_->_data, audio_payload_->_data_len, timestamp);
		audio_payload_->reset();
	}

	return ret;
}

void AnyRtmpPull::RescanVideoframe(const char*pdata, int len, uint32_t timestamp)
{
    int nal_type = pdata[4] & 0x1f;
    const char *p = pdata;
    if (nal_type == 7)
    {// keyframe
        int find7 = 0;
        const char* ptr7 = NULL;
        int size7 = 0;
        int find8 = 0;
        const char* ptr8 = NULL;
        int size8 = 0;
        const char* ptr5 = NULL;
        int size5 = 0;
        int head01 = 4;
        for (int i = 4; i < len - 4; i++)
        {
            if ((p[i] == 0x0 && p[i + 1] == 0x0 && p[i + 2] == 0x0 && p[i + 3] == 0x1) || (p[i] == 0x0 && p[i + 1] == 0x0 && p[i + 2] == 0x1))
            {
                if (p[i + 2] == 0x01)
                    head01 = 3;
                else
                    head01 = 4;
                if (find7 == 0)
                {
                    find7 = i;
                    ptr7 = p;
                    size7 = find7;
                    i++;
                }
                else if (find8 == 0)
                {
                    find8 = i;
                    ptr8 = p + find7 ;
                    size8 = find8 - find7;
                    const char* ptr = p + i;
                    if ((ptr[head01] & 0x1f) == 5)
                    {
                        ptr5 = p + find8 + head01;
                        size5 = len - find8 - head01;
                        break;
                    }
                }
                else
                {
                    ptr5 = p + i + head01;
                    size5 = len - i - head01;
                    break;
                }
            }
        }
        video_payload_->append(ptr7, size7);
        video_payload_->append(ptr8, size8);
        video_payload_->append((const char*)fresh_nalu_header, 4);
        video_payload_->append(ptr5, size5);
        callback_.OnRtmpullH264Data((uint8_t*)video_payload_->_data, video_payload_->_data_len, timestamp);
        video_payload_->reset();
    }
    else 
    {
        video_payload_->append(pdata, len);
        callback_.OnRtmpullH264Data((uint8_t*)video_payload_->_data, video_payload_->_data_len, timestamp);
        video_payload_->reset();
    }

}

void AnyRtmpPull::CallConnect()
{
	retry_ct_ = 0;
    connected_ = true;
    callback_.OnRtmpullConnected();
}

void AnyRtmpPull::CallDisconnect()
{
    rtc::CritScope l(&cs_rtmp_);
    if (rtmp_) {
        srs_rtmp_destroy(rtmp_);
        rtmp_ = NULL;
    }
    if(rtmp_status_ != RS_PLY_Closed) {
        rtmp_status_ = RS_PLY_Init;
        retry_ct_ ++;
        if(retry_ct_ <= MAX_RETRY_TIME)
        {
            rtmp_ = srs_rtmp_create(str_url_.c_str());
        } else {
            if(connected_)
                callback_.OnRtmpullDisconnect();
            else
                callback_.OnRtmpullFailed();
        }
    }
}
