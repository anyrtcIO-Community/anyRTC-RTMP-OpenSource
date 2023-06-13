/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
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
#ifndef __AR_FF_PLAYER_H__
#define __AR_FF_PLAYER_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ARPlayer.h"
#include "rtc_base/thread.h"
#include "rtc_base/synchronization/mutex.h"
#include "FFBuffer.h"
#include "sonic.h"
extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}



class ARFFPlayer : public ARPlayer, public FFBuffer, public rtc::Thread
{
public:
	ARFFPlayer(ARPlayerEvent&callback);
	virtual ~ARFFPlayer();

	int Timeout();

	//* For rtc::Thread
	virtual void Run();
	bool ReadThreadProcess();

	//* For ARPlayer
	virtual int StartTask(const char*strUrl);
	virtual void StopTask();
	virtual void RunOnce();
	virtual void Play();
	virtual void Pause();
	virtual void SetOutputAudioParam(int nSampleHz, int nChannels);
	virtual void SetAudioEnable(bool bAudioEnable);
	virtual void SetVideoEnable(bool bVideoEnable);
	virtual void SetRepeat(bool bEnable);
	virtual void SetUseTcp(bool bUseTcp);
	virtual void SetNoBuffer(bool bNoBuffer);
	virtual void SetRepeatCount(int loopCount);
	virtual void SeekTo(int nSeconds);
	virtual void SetSpeed(float fSpeed);
	virtual void SetVolume(float fVolume);
	virtual void EnableVolumeEvaluation(int32_t intervalMs);
	virtual int GetTotalDuration();
	virtual void RePlay();
	virtual void Config(bool bAuto, int nCacheTime, int nMinCacheTime, int nMaxCacheTime, int nVideoBlockThreshold);
	virtual void selectAudioTrack(int index);
	virtual int getAudioTrackCount();

	//* For PlayerBuffer
	virtual void OnBufferDecodeAudioData(AVPacket* pkt);
	virtual void OnBufferDecodeVideoData(AVPacket* pkt);
	virtual void OnBufferStatusChanged(PlayStatus playStatus);
	virtual bool OnBufferIsKeyFrame(AVPacket* pkt);
	virtual bool OnBufferGetPuased();
	virtual float OnBufferGetSpeed();

	void GotAudioFrame(char* pdata, int len, int sample_hz, int channels, int64_t timestamp);
	void ParseVideoSei(char* pData, int nLen, int64_t pts);

protected:
	void OpenFFDecode();
	void CloseFFDecode();

private:
	AVFormatContext *fmt_ctx_;	
	int n_video_stream_idx_;		// 视频流ID
	int n_audio_stream_idx_;		// 音频流ID
	int n_seek_to_;					// 播放进度控制
	int	n_all_duration_;			// 总时长
	int n_cur_aud_duration_;		// 当前播放音频时长
	int n_cur_vid_duration_;		// 当前播放视频时长
	int n_total_track_;
	bool b_running_;					
	bool b_open_stream_success_;	// 是否成功打开了流
	bool b_abort_;					// 用户主动中断
	bool b_got_eof_;				// 播放中发生EOF中断
	bool b_use_tcp_;				// 是否强制使用TCP-常用于RTSP的流播放
	bool b_no_buffer_;				// 是否无缓冲播放-有些视频源的时间戳非标准，可以使用此方法
	bool b_re_play_;				// 是否重新播放
	bool b_notify_closed_;
	bool b_decode_video_;
	uint32_t	n_reconnect_time_;	// 下次重连时间
	uint32_t	n_last_recv_data_time_;	//最近一次收到数据的时间
	uint32_t	n_stat_time_;		// 统计的定时器
	uint32_t	n_net_aud_band_;	// 音频流量
	uint32_t	n_net_vid_band_;	// 视频流量
	uint32_t	n_conn_pkt_time_;	// 连接超时时间
	int			n_vid_fps_;
	int			n_vid_width_;
	int			n_vid_height_;

	webrtc::Mutex	cs_ff_ctx_;
	AVCodecContext *video_dec_ctx_;
	AVCodecContext *audio_dec_ctx_;
	AVStream *video_stream_;
	AVStream *audio_stream_;
	AVFrame *avframe_;

	std::string		str_play_url_;
	AVRational		vstream_timebase_;
	AVRational		astream_timebase_;

	//* For resample
	SwrContext		*audio_convert_ctx_;
	uint8_t			*p_resamp_buffer_;
	int				n_resmap_size_;
	int				n_sample_hz_;
	int				n_channels_;
	char*			p_audio_sample_;
	int				n_audio_size_;
	int				n_out_sample_hz_;
	int				n_out_channels_;

	sonicStream		p_aud_sonic_;

};

#endif	// __AR_FF_PLAYER_H__