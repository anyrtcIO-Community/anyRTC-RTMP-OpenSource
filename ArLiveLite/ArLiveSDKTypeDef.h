#ifndef __AR_LIVE_SDK_TYPE_DEF_H__
#define __AR_LIVE_SDK_TYPE_DEF_H__


/////////////////////////////////////////////////////////////////////////////////
//
//                    【状态通知字段名 onNetStatus】
//                   
/////////////////////////////////////////////////////////////////////////////////

/**
 * ArLivePushListener 和 ArLivePlayListener 的 onNetStatus() 会以 2s 一次的时间间隔，定时通知网络状态和内部指标，
 * 这些数值采用 key-value 的组织格式，其中 key 值的定义如下：
 */

#define NET_STATUS_CPU_USAGE             "CPU_USAGE"              ///> 进程 CPU 占用率
#define NET_STATUS_CPU_USAGE_D           "CPU_USAGE_DEVICE"       ///> 系统 CPU 占用率

#define NET_STATUS_VIDEO_WIDTH           "VIDEO_WIDTH"            ///> 视频分辨率宽度
#define NET_STATUS_VIDEO_HEIGHT          "VIDEO_HEIGHT"           ///> 视频分辨率高度
#define NET_STATUS_VIDEO_FPS             "VIDEO_FPS"              ///> 视频帧率：也就是视频编码器每秒生产了多少帧画面。
#define NET_STATUS_VIDEO_GOP             "VIDEO_GOP"              ///> 关键帧间隔：即每两个关键帧(I帧)间隔时长，单位：秒。
#define NET_STATUS_VIDEO_BITRATE         "VIDEO_BITRATE"          ///> 视频码率：即视频编码器每秒生产了多少视频数据，单位：kbps。
#define NET_STATUS_AUDIO_BITRATE         "AUDIO_BITRATE"          ///> 音频码率：即音频编码器每秒生产了多少音频数据，单位：kbps。
#define NET_STATUS_NET_SPEED             "NET_SPEED"              ///> 传输速度：即每秒钟发送或接收了多少字节的数据。

#define NET_STATUS_VIDEO_CACHE           "VIDEO_CACHE"            ///> ArLivePusher：主播端堆积的视频帧数；ArLivePlayer：播放端缓冲的视频总时长。
#define NET_STATUS_AUDIO_CACHE           "AUDIO_CACHE"            ///> ArLivePusher：主播端堆积的音频帧数；ArLivePlayer：播放端缓冲的音频总时长。
#define NET_STATUS_VIDEO_DROP            "VIDEO_DROP"             ///> ArLivePusher：主播端主动丢弃的视频帧数；ArLivePlayer: N/A。
#define NET_STATUS_AUDIO_DROP            "AUDIO_DROP"             ///> 暂未使用

#define NET_STATUS_V_DEC_CACHE_SIZE      "V_DEC_CACHE_SIZE"       ///> ArLivePlayer：播放端解码器中缓存的视频帧数（Android 端硬解码时存在）。
#define NET_STATUS_V_SUM_CACHE_SIZE      "V_SUM_CACHE_SIZE"       ///> ArLivePlayer：播放端缓冲的总视频帧数，该数值越大，播放延迟越高。
#define NET_STATUS_AV_PLAY_INTERVAL      "AV_PLAY_INTERVAL"       ///> ArLivePlayer：音画同步错位时间（播放），单位 ms，此数值越小，音画同步越好。
#define NET_STATUS_AV_RECV_INTERVAL      "AV_RECV_INTERVAL"       ///> ArLivePlayer：音画同步错位时间（网络），单位 ms，此数值越小，音画同步越好。
#define NET_STATUS_AUDIO_CACHE_THRESHOLD "AUDIO_CACHE_THRESHOLD"  ///> ArLivePlayer：音频缓冲时长阈值，缓冲超过该阈值后，播放器会开始调控延时。

#define NET_STATUS_AUDIO_INFO            "AUDIO_INFO"             ///> 音频信息：包括采样率信息和声道数信息
#define NET_STATUS_NET_JITTER            "NET_JITTER"             ///> 网络抖动：数值越大表示抖动越大，网络越不稳定
#define NET_STATUS_QUALITY_LEVEL         "NET_QUALITY_LEVEL"      ///> 网络质量：0：未定义 1：最好 2：好 3：一般 4：差 5：很差 6：不可用
#define NET_STATUS_SERVER_IP             "SERVER_IP"              ///> 连接的Server IP地址


/////////////////////////////////////////////////////////////////////////////////
//
//                    【事件通知字段名 onPushEvent onPlayEvent】
//                   
/////////////////////////////////////////////////////////////////////////////////


/**
 * anyRTC 通过 ArLivePushListener 中的 onPushEvent()，ArLivePlayListener 中的 onPlayEvent() 向您通知内部错误、警告和事件：
 * - 错误：严重且不可恢复的错误，会中断 SDK 的正常逻辑。
 * - 警告：非致命性的提醒和警告，可以不理会。
 * - 事件：SDK 的流程和状态通知，比如开始推流，开始播放，等等。
 *  
 * 这些数值采用 key-value 的组织格式，其中 key 值的定义如下：
 */
#define EVT_MSG                          "EVT_MSG"                 ///> 事件ID
#define EVT_TIME                         "EVT_TIME"                ///> 事件发生的UTC毫秒时间戳
#define EVT_UTC_TIME                     "EVT_UTC_TIME"            ///> 事件发生的UTC毫秒时间戳(兼容性)
#define EVT_BLOCK_DURATION               "EVT_BLOCK_DURATION"      ///> 卡顿时间（毫秒）
#define EVT_PARAM1                       "EVT_PARAM1"              ///> 事件参数1
#define EVT_PARAM2                       "EVT_PARAM2"              ///> 事件参数2
#define EVT_GET_MSG                      "EVT_GET_MSG"             ///> 消息内容，收到PLAY_EVT_GET_MESSAGE事件时，通过该字段获取消息内容
#define EVT_PLAY_PROGRESS                "EVT_PLAY_PROGRESS"       ///> 点播：视频播放进度
#define EVT_PLAY_DURATION                "EVT_PLAY_DURATION"       ///> 点播：视频总时长
#define EVT_PLAYABLE_DURATION            "PLAYABLE_DURATION"       ///> 点播：视频可播放时长
#define EVT_PLAY_COVER_URL               "EVT_PLAY_COVER_URL"      ///> 点播：视频封面
#define EVT_PLAY_URL                     "EVT_PLAY_URL"            ///> 点播：视频播放地址
#define EVT_PLAY_NAME                    "EVT_PLAY_NAME"           ///> 点播：视频名称
#define EVT_PLAY_DESCRIPTION             "EVT_PLAY_DESCRIPTION"    ///> 点播：视频简介

#define STREAM_ID                        "STREAM_ID"

#endif // __AR_LIVE_SDK_TYPE_DEF_H__
