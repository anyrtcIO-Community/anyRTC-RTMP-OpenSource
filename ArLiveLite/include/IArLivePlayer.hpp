/// @defgroup ArLivePlayer_cplusplus ArLivePlayer
/// anyRTC开源直播播放器。<br/>
/// 主要负责从指定的直播流地址拉取音视频数据，并进行解码和本地渲染播放。
///
/// 播放器包含如下能力:
/// - 支持RTMP, HTTP-FLV，Webrtc, HLS, etc；
/// - 延时调节，可以设置播放器缓存自动调整的最小和最大时间；
/// - 自定义的视频数据处理，让您可以根据项目需要处理直播流中的视频数据后，进行渲染以及播放。
///
/// @{

#ifndef ANYRTC_CPP_I_ARLIVEPLAYER_H_
#define ANYRTC_CPP_I_ARLIVEPLAYER_H_

#include "ArLiveDef.hpp"
#include "ArLivePlayerObserver.hpp"

namespace anyrtc {
class ArLivePlayer;
}

namespace anyrtc {

class IArLivePlayer {
   public:
    /**
     * 设置播放器回调。
     *
     * 通过设置回调，可以监听 ArLivePlayer 播放器的一些回调事件，
     * 包括播放器状态、播放音量回调、音视频首帧回调、统计数据、警告和错误信息等。
     *
     * @param observer 播放器的回调目标对象，更多信息请查看 {@link ArLivePlayerObserver}
     */
    virtual void setObserver(ArLivePlayerObserver* observer) = 0;


    /**
     * 设置播放器的视频渲染 View。 该控件负责显示视频内容。
     *
     * @param view 播放器渲染 View
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK：成功
     */
    virtual int32_t setRenderView(void* view) = 0;

    /**
     * 设置播放器画面的旋转角度。
     *
     * @param rotation 旋转角度 {@link ArLiveRotation}
     *         - ArLiveRotation0【默认值】: 0度, 不旋转
     *         - ArLiveRotation90:  顺时针旋转90度
     *         - ArLiveRotation180: 顺时针旋转180度
     *         - ArLiveRotation270: 顺时针旋转270度
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setRenderRotation(ArLiveRotation rotation) = 0;

    /**
     * 设置画面的填充模式。
     *
     * @param mode 画面填充模式 {@link ArLiveFillMode}。
     *         - ArLiveFillModeFill 【默认值】: 图像铺满屏幕，不留黑边，如果图像宽高比不同于屏幕宽高比，部分画面内容会被裁剪掉
     *         - ArLiveFillModeFit: 图像适应屏幕，保持画面完整，但如果图像宽高比不同于屏幕宽高比，会有黑边的存在
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setRenderFillMode(ArLiveFillMode mode) = 0;

    /**
     * 设置播放模式。
     *
     * @param mode 播放模式 {@link ArLivePlayMode}。
     *         - ArLivePlayModeLive 【默认值】: 直播模式 - 暂停的过程中，数据会丢失，保证实时性
     *         - ArLivePlayModeVod: 点播模式 - 暂停的过程中，数据不会丢失，恢复后会继续播放
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setPlayMode(ArLivePlayMode mode) = 0;

    /**
     * 开始播放音视频流。
     *
     * @param url 音视频流的播放地址，支持 RTMP, HTTP-FLV, WebRTC。
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 操作成功，开始连接并播放
     *         - ArLIVE_ERROR_INVALID_PARAMETER: 操作失败，url 不合法
     *         - ArLIVE_ERROR_REFUSED: RTC 不支持同一设备上同时推拉同一个 StreamId。
     */
    virtual int32_t startPlay(const char* url) = 0;

    /**
     * 停止播放音视频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t stopPlay() = 0;

    /**
     * 播放器是否正在播放中。
     *
     * @return 是否正在播放
     *         - 1: 正在播放中
     *         - 0: 已经停止播放
     */
    virtual int32_t isPlaying() = 0;

    /**
     * 播放进度，单位秒。仅适用于点播，直播或WebRTC等实时流不会生效。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t seekTo(int seekTimeS) = 0;


    /**
     * 倍速播放。0.5   0.75  1.0  1.25  1.5 1.75 2.0 3.0
     * 仅适用于点播，直播或WebRTC等实时流不会生效。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setSpeed(float speed) = 0;

    /**
     * 重新开始播放。一般用于点播场景
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t rePlay() = 0;

    /**
     * 暂停播放器的音频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t pauseAudio() = 0;

    /**
     * 恢复播放器的音频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t resumeAudio() = 0;

    /**
     * 暂停播放器的视频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t pauseVideo() = 0;

    /**
     * 恢复播放器的视频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t resumeVideo() = 0;

    /**
     * 设置播放器音量。
     *
     * @param volume 音量大小，取值范围0 - 100。【默认值】: 100
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setPlayoutVolume(int32_t volume) = 0;

    /**
     * 设置播放器缓存自动调整的最小和最大时间 ( 单位：秒 )。
     *
     * @param minTime 缓存自动调整的最小时间，取值需要大于0。【默认值】：1
     * @param maxTime 缓存自动调整的最大时间，取值需要大于0。【默认值】：5
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_INVALID_PARAMETER: 操作失败，minTime 和 maxTime 需要大于0
     *         - ArLIVE_ERROR_REFUSED: 播放器处于播放状态，不支持修改缓存策略
     */
    virtual int32_t setCacheParams(float minTime, float maxTime) = 0;

    /**
     * 启用播放音量大小提示。
     *
     * 开启后可以在 {@link ArLivePlayerObserver#onPlayoutVolumeUpdate(ArLivePlayer, int)} 回调中获取到 SDK 对音量大小值的评估。
     *
     * @param intervalMs 决定了 onPlayoutVolumeUpdate 回调的触发间隔，单位为ms，最小间隔为100ms，如果小于等于0则会关闭回调，建议设置为300ms；【默认值】：0，不开启
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t enableVolumeEvaluation(int32_t intervalMs) = 0;

    /**
     * 截取播放过程中的视频画面。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_REFUSED: 播放器处于停止状态，不允许调用截图操作
     */
    virtual int32_t snapshot() = 0;

    /**
     * 设置视频自定义渲染回调。
     *
     * 通过该方法，可以获取解码后的每一帧视频画面，进行自定义渲染处理，添加自定义显示效果。
     *
     * @param enable      是否开启自定义渲染。【默认值】：false
     * @param pixelFormat 自定义渲染回调的视频像素格式 {@link ArLivePixelFormat}。
     * @param bufferType  自定义渲染回调的视频数据格式 {@link ArLiveBufferType}。
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_NOT_SUPPORTED: 像素格式或者数据格式不支持
     */
    virtual int32_t enableCustomRendering(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType) = 0;

    /**
     * 开启接收 SEI 消息
     *
     * @param enable      true: 开启接收 SEI 消息; false: 关闭接收 SEI 消息。【默认值】: false
     * @param payloadType 指定接收 SEI 消息的 payloadType，支持 5、242，请与发送端的 payloadType 保持一致。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t enableReceiveSeiMessage(bool enable, int payloadType) = 0;

    /**
     * 是否显示播放器状态信息的调试浮层。
     *
     * @param isShow 是否显示。【默认值】：false
     */
    virtual void showDebugView(bool isShow) = 0;

   protected:
    virtual ~IArLivePlayer(){};
};

}  // namespace anyrtc

#endif  // ANYRTC_CPP_I_ARLIVEPLAYER_H_
/// @}
