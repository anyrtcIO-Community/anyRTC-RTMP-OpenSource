/// @defgroup ArLivePlayerObserver_cplusplus ArLivePlayerObserver
/// anyRTC开源直播的播放器回调通知。<br/>
/// 可以接收 ArLivePlayer 播放器的一些回调通知，包括播放器状态、播放音量回调、音视频首帧回调、统计数据、警告和错误信息等。
/// @{

#ifndef ANYRTC_CPP_ARLIVEPLAYEROBSERVER_H_
#define ANYRTC_CPP_ARLIVEPLAYEROBSERVER_H_

#include "ArLiveDef.hpp"

namespace anyrtc {
class IArLivePlayer;

class ArLivePlayerObserver {
   public:
    virtual ~ArLivePlayerObserver(){};

    /**
     * 直播播放器错误通知，播放器出现错误时，会回调该通知
     *
     * @param player    回调该通知的播放器对象
     * @param code      错误码 {@link ArLiveCode}
     * @param msg       错误信息
     * @param extraInfo 扩展信息
     */
    virtual void onError(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo){};

    /**
     * 直播播放器警告通知
     *
     * @param player    回调该通知的播放器对象
     * @param code      警告码 {@link ArLiveCode}
     * @param msg       警告信息
     * @param extraInfo 扩展信息
     */
    virtual void onWarning(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo){};

    /**
     * 直播播放器视频状态变化通知
     *
     * @param player    回调该通知的播放器对象
     * @param status    状态码 {@link ArLivePlayStatus}
     * @param reason    状态对应的原因 {@link ArLiveStatusChangeReason}
     * @param extraInfo 扩展信息
     */
    virtual void onVideoPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo){};

    /**
     * 直播播放器音频状态变化通知
     *
     * @param player    回调该通知的播放器对象
     * @param status    状态码 {@link ArLivePlayStatus}
     * @param reason    状态对应的原因 {@link ArLiveStatusChangeReason}
     * @param extraInfo 扩展信息
     */
    virtual void onAudioPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo){};

    /**
     * 播放器音量大小回调
     *
     * @param player 回调该通知的播放器对象
     * @param volume 音量大小
     * @note  调用 {@link ArLivePlayer#enableVolumeEvaluation(int)} 开启播放音量大小提示之后，会收到这个回调通知。
     */
    virtual void onPlayoutVolumeUpdate(IArLivePlayer* player, int32_t volume){};

    /**
    * 播放器点播的进度回调
    *
    * @param player 回调该通知的播放器对象
    * @param allDuration 总时长，单位毫秒
    * @param currentPlaybackTime 当前已播放时长，单位毫秒
    * @param bufferDuration 已缓冲时长，单位毫秒
    * @note  只有点播才会收到这个回调通知。
    */
    virtual void onVodPlaybackProcess(IArLivePlayer* player, int allDuration, int currentPlaybackTime, int bufferDuration) {};

    /**
     * 直播播放器统计数据回调
     *
     * @param player     回调该通知的播放器对象
     * @param statistics 播放器统计数据 {@link ArLivePlayerStatistics}
     */
    virtual void onStatisticsUpdate(IArLivePlayer* player, ArLivePlayerStatistics statistics){};

    /**
     * 截图回调
     *
     * @note  调用 [snapshot](@ref ArLivePlayer#snapshot) 截图之后，会收到这个回调通知
     *
     * @param player 回调该通知的播放器对象
     * @param image  已截取的视频画面
     * @param length 截图数据长度，对于BGRA32而言，length = width * height * 4
     * @param width  截图画面的宽度
     * @param height 截图画面的高度
     * @param format 截图数据格式，目前只支持 ArLivePixelFormatBGRA32
     */
    virtual void onSnapshotComplete(IArLivePlayer* player, const char* image, int length, int width, int height, ArLivePixelFormat format){};

    /**
     * 自定义视频渲染回调
     *
     * @param player     回调该通知的播放器对象
     * @param videoFrame 视频帧数据 {@link ArLiveVideoFrame}
     * @note  调用 [enableCustomRendering](@ref ArLivePlayer#enableCustomRendering:pixelFormat:bufferType:) 开启自定义渲染之后，会收到这个回调通知
     */
    virtual void onRenderVideoFrame(IArLivePlayer* player, const ArLiveVideoFrame* videoFrame){};

    /**
     * 收到 SEI 消息的回调，发送端通过 {@link ArLivePusher} 中的 `sendSeiMessage` 来发送 SEI 消息。
     *
     * @note  调用 {@link ArLivePlayer} 中的 `enableReceiveSeiMessage` 开启接收 SEI 消息之后，会收到这个回调通知
     *
     * @param player   回调该通知的播放器对象
     * @param payloadType   收到 SEI 消息的 payloadType
     * @param data     数据
     * @param dataSize 数据大小
     */
    virtual void onReceiveSeiMessage(IArLivePlayer* player, int payloadType, const uint8_t* data, uint32_t dataSize){};

    /**
     * 收到第一帧音频渲染回调 
     */
    virtual void onFirstAudioFrameRender(IArLivePlayer* player, int64_t nTimeUsed, const char* extraInfo) {};
    /**
     * 收到第一帧视频渲染回调
     */
    virtual void onFirstVideoFrameRender(IArLivePlayer* player, int64_t nTimeUsed, const char* extraInfo) {};
};

}  // namespace anyrtc

#endif  // ANYRTC_CPP_ARLIVEPLAYEROBSERVER_H_
/// @}
