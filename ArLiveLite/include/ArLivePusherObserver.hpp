/// @defgroup ArLivePusherObserver_cplusplus ArLivePusherObserver
/// anyRTC开源直播推流的回调通知。<br/>
/// ArLivePusher 的一些回调事件，包括推流器状态，推流音量，统计信息，警告以及错误信息。
/// @{

#ifndef ANYRTC_CPP_ARLIVEPUSHEROBSERVER_H_
#define ANYRTC_CPP_ARLIVEPUSHEROBSERVER_H_

#include "ArLiveDef.hpp"

namespace anyrtc {

class ArLivePusherObserver {
   public:
    virtual ~ArLivePusherObserver(){};

    /**
     * 直播推流器错误通知，推流器出现错误时，会回调该通知
     *
     * @param code      错误码 {@link ArLiveCode}
     * @param msg       错误信息
     * @param extraInfo 扩展信息
     */
    virtual void onError(int32_t code, const char* msg, void* extraInfo){};

    /**
     * 直播推流器警告通知
     *
     * @param code      警告码 {@link ArLiveCode}
     * @param msg       警告信息
     * @param extraInfo 扩展信息
     */
    virtual void onWarning(int32_t code, const char* msg, void* extraInfo){};

    /**
        * 首帧音频采集完成的回调通知
        */
    virtual void onCaptureFirstAudioFrame() {};

    /**
        * 首帧视频采集完成的回调通知
        */
    virtual void onCaptureFirstVideoFrame() {};

    /**
     * 麦克风采集音量值回调
     *
     * @param volume 音量大小
     * @note  调用 [enableVolumeEvaluation](@ref ArLivePusher#enableVolumeEvaluation:) 开启采集音量大小提示之后，会收到这个回调通知。
     */
    virtual void onMicrophoneVolumeUpdate(int32_t volume) {};

    /**
     * 推流器连接状态回调通知
     *
     * @param status    推流器连接状态 {@link ArLivePushStatus}
     * @param msg       连接状态信息
     * @param extraInfo 扩展信息
     */
    virtual void onPushStatusUpdate(ArLivePushStatus state, const char* msg, void* extraInfo){};

    /**
     * 直播推流器统计数据回调
     *
     * @param statistics 推流器统计数据 {@link ArLivePusherStatistics}
     */
    virtual void onStatisticsUpdate(ArLivePusherStatistics statistics){};

    /**
     * 截图回调
     *
     * @note  调用 [snapshot](@ref ArLivePusher#snapshot) 截图之后，会收到这个回调通知
     *
     * @param image  已截取的视频画面
     * @param length 截图数据长度，对于BGRA32而言，length = width * height * 4
     * @param width  截图画面的宽度
     * @param height 截图画面的高度
     * @param format 截图数据格式，目前只支持 ArLivePixelFormatBGRA32
     */
    virtual void onSnapshotComplete(const char* image, int length, int width, int height, ArLivePixelFormat format){};

    /**
     * 自定义视频渲染回调
     *
     * @note  调用 [enableCustomRendering](@ref ArLivePusher#enableCustomRendering) 开启本地视频自定义渲染之后，会收到这个回调通知
     *
     * @param videoFrame 视频帧数据 {@link ArLiveVideoFrame}
     */
    virtual void onRenderVideoFrame(const ArLiveVideoFrame* videoFrame){};

    /**
     * 自定义视频预处理数据回调
     *
     * @note  调用 {@link ArLivePusher#enableCustomVideoProcessing} 接口开启/关闭自定义视频处理回调
     *        Windows 暂时只支持 YUV420 格式。
     *
     * @param srcFrame 处理前的视频帧
     * @param dstFrame 处理后的视频帧
     * @return - 0：   成功
     *         - 其他： 错误
     */
    virtual int onProcessVideoFrame(ArLiveVideoFrame* srcFrame, ArLiveVideoFrame* dstFrame) {
        return 0;
    };

    /**
     * 当屏幕分享开始时，SDK 会通过此回调通知
     */
    virtual void onScreenCaptureStarted(){};

    /**
        * 当屏幕分享停止时，SDK 会通过此回调通知
        *
        * @param reason 停止原因
        *               - 0：表示用户主动停止；
        *               - 1：表示屏幕分享窗口被关闭；
        *               - 2：表示屏幕分享的显示屏状态变更（如接口被拔出、投影模式变更等）
        */
    virtual void onScreenCaptureStoped(int reason) {};

};
}  // namespace anyrtc
#endif  // ANYRTC_CPP_ARLIVEPUSHEROBSERVER_H_
/// @}
