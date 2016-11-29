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
#include "anyrtmpush.h"
#include "srs_librtmp.h"
#include <assert.h>
#include "webrtc/base/logging.h"

#define MAX_RETRY_TIME	3
AnyRtmpPush::AnyRtmpPush(AnyRtmpushCallback&callback, const std::string&url)
: callback_(callback)
, running_(false)
, need_keyframe_(true)
, only_audio_mode_(false)
, retrys_(0)
, stat_time_(0)
, net_band_(0)
, sound_format_(10)
, sound_rate_(3)	// 3 = 44 kHz
, sound_size_(1)	// 1 = 16-bit samples
, sound_type_(1)	// 0 = Mono sound; 1 = Stereo sound
, rtmp_status_(RS_STM_Init)
, rtmp_(NULL)
{
	str_url_ = url;
	rtmp_ = srs_rtmp_create(str_url_.c_str());

	running_ = true;
	rtc::Thread::Start();
}

AnyRtmpPush::~AnyRtmpPush(void)
{
	running_ = false;
	rtmp_status_ = RS_STM_Closed;
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

	std::list<EncData*>::iterator iter = lst_enc_data_.begin();
	while (iter != lst_enc_data_.end()) {
		EncData* ptr = *iter;
		lst_enc_data_.erase(iter++);
		delete[] ptr->_data;
		delete ptr;
	}
}

void AnyRtmpPush::EnableOnlyAudioMode()
{
	only_audio_mode_ = true;
}

void AnyRtmpPush::SetVideoParameter(int width, int height, int videodatarate, int framerate){
    video_width_ = width;
    video_height_ = height;
    video_framerate_ = framerate;
    video_datarate_ = videodatarate;
}

void AnyRtmpPush::SetAudioParameter(int samplerate, int pcmbitsize, int channel)
{
	sound_samplerate_ = samplerate;
	switch(samplerate)
	{
	case 44100:
		sound_rate_ = 3;
		break;
	case 22050:
		sound_rate_ = 2;
		break;
	case 11025:
		sound_rate_ = 1;
		break;
	default:
		assert(false);
		break;
	};

	switch(pcmbitsize)
	{
	case 8:
		sound_size_ = 0;
		break;
	case 16:
		sound_size_ = 1;
		break;
	default:
		assert(false);
		break;
	}

	switch(channel)
	{
	case 1:
		sound_type_ = 0;
		break;
	case 2:
		sound_type_ = 1;
		break;
	default:
		assert(false);
		break;
	}
}

void AnyRtmpPush::SetH264Data(uint8_t* pData, int len, uint32_t ts)
{
    
	uint8_t *p = pData;
	int nal_type = p[4] & 0x1f;
	if(nal_type == 7)
		need_keyframe_ = false;
	if(need_keyframe_)
		return;
	if(nal_type == 7)
	{// keyframe
		int find7 = 0;
		uint8_t* ptr7 = NULL;
		int size7 = 0;
		int find8 = 0;
		uint8_t* ptr8 = NULL;
		int size8 = 0;
		uint8_t* ptr5 = NULL;
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
				  ptr8 = p + find7;
				  size8 = find8 - find7;
				  unsigned char* ptr = p + i;
				  if ((ptr[head01] & 0x1f) == 5)
				  {
					  ptr5 = p + find8;
					  size5 = len - find8;
					  break;
				  }
			  }
			  else
			  {
				  ptr5 = p + i;
				  size5 = len - i;
				  break;
			  }
			}
		}
		
		GotH264Nal(ptr7, size7, ts);
		GotH264Nal(ptr8, size8, ts);
		GotH264Nal(ptr5, size5, ts);
	}
	else 
	{
		GotH264Nal(pData, len, ts);
    }
}

void AnyRtmpPush::SetAacData(uint8_t* pData, int nLen, uint32_t ts)
{
	if(need_keyframe_ && !only_audio_mode_)
		return;
	EncData* pdata = new EncData();
	pdata->_data = new uint8_t[nLen];
	memcpy(pdata->_data, pData, nLen);
	pdata->_dataLen = nLen;
	pdata->_bVideo = false;
	pdata->_type = AUDIO_DATA;
	pdata->_dts = ts;
	rtc::CritScope l(&cs_list_enc_);
	lst_enc_data_.push_back(pdata);
}

uint8_t * put_byte( uint8_t *output, uint8_t nVal )
{
    output[0] = nVal;
    return output+1;
}

uint8_t * put_be16(uint8_t *output, uint16_t nVal )
{
    output[1] = nVal & 0xff;
    output[0] = nVal >> 8;
    return output+2;
}

uint8_t * put_be24(uint8_t *output,uint32_t nVal )
{
    output[2] = nVal & 0xff;
    output[1] = nVal >> 8;
    output[0] = nVal >> 16;
    return output+3;
}

uint8_t * put_be32(uint8_t *output, uint32_t nVal )
{
    output[3] = nVal & 0xff;
    output[2] = nVal >> 8;
    output[1] = nVal >> 16;
    output[0] = nVal >> 24;
    return output+4;
}

uint8_t *  put_be64( uint8_t *output, uint64_t nVal )
{
    output=put_be32( output, nVal >> 32 );
    output=put_be32( output, nVal );
    return output;
}

uint8_t * put_amf_string( uint8_t *c, const char *str )
{
    uint16_t len = strlen( str );
    c=put_be16( c, len );
    memcpy(c,str,len);
    return c+len;
}

uint8_t * put_amf_double( uint8_t *c, double d )
{
   uint8_t *ci, *co;
   ci = (uint8_t *)&d;
   co = (unsigned char *)c;
   co[0] = ci[7];
   co[1] = ci[6];
   co[2] = ci[5];
   co[3] = ci[4];
   co[4] = ci[3];
   co[5] = ci[2];
   co[6] = ci[1];
   co[7] = ci[0];

   return c+8;
}

#define AMF_STRING 0x02
#define AMF_STRING_LEN 0x0A
#define AMF_TYPE_ARRAY 0x08
#define AMF_OBJECT 0x03
#define AMF_OBJECT_END 0x09
enum
{
    FLV_CODECID_H264 = 7,
};

void AnyRtmpPush::setMetaData(){
    uint8_t body[512] = {0};
    uint8_t* p = (uint8_t*)body;
    p = put_byte(p, AMF_STRING);
    //p = put_be16(p, AMF_STRING_LEN);
    p = put_amf_string(p, "onMetaData");
    p = put_byte(p, AMF_TYPE_ARRAY);
    p = put_be32(p, 9);

    //p = put_be16(p, 5);
    p = put_amf_string(p, "width");
    p = put_byte(p, 0);
    p = put_amf_double(p, video_width_);

    //p = put_be16(p, 6);
    p = put_amf_string(p, "height");
    p = put_byte(p, 0);
    p = put_amf_double(p, video_height_);

    //p = put_be16(p, 9);
    p = put_amf_string(p, "videodatarate");
    p = put_byte(p, 0);
    p = put_amf_double(p, video_datarate_);

    p = put_amf_string(p, "framerate");
    p = put_byte(p, 0);
    p = put_amf_double(p, video_framerate_);

    p = put_amf_string(p, "videocodecid");
    p = put_byte(p, 0);
    p = put_amf_double(p, 7);

    //audiosamplerate
    p = put_amf_string(p, "audiosamplerate");
    p = put_byte(p, 0);
    p = put_amf_double(p, sound_samplerate_);
    //pcmbitsize
    p = put_amf_string(p, "audiosamplesize");
    p = put_byte(p, 0);
    p = put_amf_double(p, sound_size_);
    //channel
    p = put_amf_string(p, "stereo");
    p = put_byte(p, 0);
    p = put_amf_double(p, sound_type_);

    //sound_format_
    p = put_amf_string(p, "audiocodecid");
    p = put_byte(p, 0);
    p = put_amf_double(p, sound_format_);

    int len = p-body;
    setMetaData((uint8_t*)body, len, 0);
}

void AnyRtmpPush::setMetaData(uint8_t* pData, int nLen, uint32_t ts)
{
	EncData* pdata = new EncData();
	pdata->_data = new uint8_t[nLen];
	memcpy(pdata->_data, pData, nLen);
	pdata->_dataLen = nLen;
	pdata->_bVideo = false;
	pdata->_type = META_DATA;
	pdata->_dts = ts;
	rtc::CritScope l(&cs_list_enc_);
	lst_enc_data_.push_back(pdata);
}

void AnyRtmpPush::GotH264Nal(uint8_t* pData, int nLen, uint32_t ts)
{
	EncData* pdata = new EncData();
	pdata->_data = new uint8_t[nLen];
	memcpy(pdata->_data, pData, nLen);
	pdata->_dataLen = nLen;
	pdata->_bVideo = true;
	pdata->_type = VIDEO_DATA;
	pdata->_dts = ts;
	rtc::CritScope l(&cs_list_enc_);
	lst_enc_data_.push_back(pdata);
}

//* For Thread
void AnyRtmpPush::Run()
{
	while(running_)
	{
		{// ProcessMessages
			this->ProcessMessages(10);
		}

		if(rtmp_ != NULL)
		{
			switch (rtmp_status_) {
			case RS_STM_Init:
			{
				if (srs_rtmp_handshake(rtmp_) == 0) {
					srs_human_trace("SRS: simple handshake ok.");
					rtmp_status_ = RS_STM_Handshaked;
				}
				else {
					CallDisconnect();
				}
			}
				break;
			case RS_STM_Handshaked:
			{
				if (srs_rtmp_connect_app(rtmp_) == 0) {
					srs_human_trace("SRS: connect vhost/app ok.");
					rtmp_status_ = RS_STM_Connected;
				}
				else {
					CallDisconnect();
				}
			}
				break;
			case RS_STM_Connected:
			{
				if (srs_rtmp_publish_stream(rtmp_) == 0) {
					srs_human_trace("SRS: publish stream ok.");
					rtmp_status_ = RS_STM_Published;
					CallConnect();
				}
				else {
					CallDisconnect();
				}
			}
				break;
			case RS_STM_Published:
			{
				DoSendData();
			}
				break;
			}
		}
	}
}

void AnyRtmpPush::CallConnect()
{
	need_keyframe_ = true;
	retrys_ = 0;
	{
		rtc::CritScope l(&cs_list_enc_);
		std::list<EncData*>::iterator iter = lst_enc_data_.begin();
		while (iter != lst_enc_data_.end()) {
			EncData* ptr = *iter;
			lst_enc_data_.erase(iter++);
			delete[] ptr->_data;
			delete ptr;
		}
	}
	callback_.OnRtmpConnected();
}

void AnyRtmpPush::CallDisconnect()
{
	need_keyframe_ = true;
    {
        rtc::CritScope l(&cs_rtmp_);
        if (rtmp_) {
            srs_rtmp_destroy(rtmp_);
            rtmp_ = NULL;
        }
        if(rtmp_status_ != RS_STM_Closed) {
            rtmp_status_ = RS_STM_Init;
            retrys_ ++;
            if(retrys_ <= MAX_RETRY_TIME)
            {
                rtmp_ = srs_rtmp_create(str_url_.c_str());
                callback_.OnRtmpReconnecting(retrys_);
            } else {
                callback_.OnRtmpDisconnect();
            }
        }
    }
}

void AnyRtmpPush::CallStatusEvent(int delayMs, int netBand)
{
	callback_.OnRtmpStatusEvent(delayMs, netBand);
}

void AnyRtmpPush::DoSendData()
{
	EncData* dataPtr = NULL;
	{
		rtc::CritScope l(&cs_list_enc_);
		if (lst_enc_data_.size() > 0) {
			dataPtr = lst_enc_data_.front();
			lst_enc_data_.pop_front();
		}
	}

	if (dataPtr != NULL) {
		if (dataPtr->_type == VIDEO_DATA) {

			char *ptr = (char*)dataPtr->_data;
			int len = dataPtr->_dataLen;
			int ret = 0;
			ret = srs_h264_write_raw_frames(rtmp_, ptr, len, dataPtr->_dts, dataPtr->_dts);

			if (ret != 0) {
				if (srs_h264_is_dvbsp_error(ret)) {
					srs_human_trace("ignore drop video error, code=%d", ret);
				}
				else if (srs_h264_is_duplicated_sps_error(ret)) {
					srs_human_trace("ignore duplicated sps, code=%d", ret);
				}
				else if (srs_h264_is_duplicated_pps_error(ret)) {
					srs_human_trace("ignore duplicated pps, code=%d", ret);
				}
				else {
					srs_human_trace("send h264 raw data failed. ret=%d", ret);
					CallDisconnect();
					return;
				}
			}
		}
		else if(dataPtr->_type == AUDIO_DATA){
			int ret = 0;
			if ((ret = srs_audio_write_raw_frame(rtmp_,
				sound_format_, sound_rate_, sound_size_, sound_type_,
				(char*)dataPtr->_data, dataPtr->_dataLen, dataPtr->_dts)) != 0) {
				srs_human_trace("send audio raw data failed. ret=%d", ret);
				CallDisconnect();
				return;
			}
		}
		else if(dataPtr->_type == META_DATA){
            int ret = srs_rtmp_write_packet(rtmp_, SRS_RTMP_TYPE_SCRIPT, dataPtr->_dts, (char*)dataPtr->_data, dataPtr->_dataLen);
			if (ret != 0) {
				srs_human_trace("send metadata failed. ret=%d", ret);
			}
		    return;
		}

		net_band_ += dataPtr->_dataLen;
		delete[] dataPtr->_data;
		delete dataPtr;
	}

	//* Statics
	if(stat_time_ <= rtc::Time())
	{
		stat_time_ = rtc::Time() + 1000;
		uint32_t delayMs = 0;

		rtc::CritScope l(&cs_list_enc_);
		if (lst_enc_data_.size() > 0) {
			EncData* frontprt = lst_enc_data_.front();
			EncData* backptr = lst_enc_data_.back();
			delayMs = backptr->_dts - frontprt->_dts;
		}

		CallStatusEvent(delayMs, net_band_*(8+1));
		net_band_ = 0;
	}
}