/// @defgroup ArLivePusher_cplusplus ArLivePusher
/// anyRTC开源直播推流器。<br/>
/// 主要负责将本地的音频和视频画面进行编码，并推送到指定的推流地址，支持任意的推流服务端。
///
/// 推流器包含如下能力：
/// - 自定义的视频采集，让您可以根据项目需要定制自己的音视频数据源；
/// - Qos 流量控制技术，具备上行网络自适应能力，可以根据主播端网络的具体情况实时调节音视频数据量；
/// - 基础美颜
///
/// @{

#ifndef ANYRTC_CPP_I_ARLIVEPUSHER_H_
#define ANYRTC_CPP_I_ARLIVEPUSHER_H_

#include "ArLiveDef.hpp"
#include "ArLivePusherObserver.hpp"
#include "IArAudioEffectManager.h"
#include "IArDeviceManager.h"
#ifdef __ANDROID__
#include "api/video_codecs/video_encoder_factory.h"
#endif
namespace anyrtc {
class IArLivePusher;
}

namespace anyrtc {

class IArLivePusher {
   public:
    /**
     * 设置推流器回调。
     *
     * 通过设置回调，可以监听 ArLivePusher 推流器的一些回调事件，
     * 包括推流器状态、音量回调、统计数据、警告和错误信息等。
     *
     * @param observer 推流器的回调目标对象，更多信息请查看 {@link ArLivePusherObserver}
     */
    virtual void setObserver(ArLivePusherObserver* observer) = 0;

    /**
     * 设置厂家类型Oem。
     *
     * 由于各个厂家的标准不统一，比如webrtc的whip方案，
     * 需要针对不同厂家用针对性的方案去实现
     *
     * @param observer oem的厂家枚举，更多信息请查看 {@link ArLiveOem}
     */
    virtual void setLiveOem(ArLiveOem oem) = 0;

    /**
     * 设置本地摄像头预览 View。
     *
     * 本地摄像头采集到的画面，经过美颜、脸形调整、滤镜等多种效果叠加之后，最终会显示到传入的 View 上。
     *
     * @param view 本地摄像头预览 View
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK：成功
     */
    virtual int32_t setRenderView(void* view) = 0;

    /**
     * 设置本地摄像头预览镜像。
     *
     * 本地摄像头分为前置摄像头和后置摄像头，系统默认情况下，是前置摄像头镜像，后置摄像头不镜像，这里可以修改前置后置摄像头的默认镜像类型。
     *
     * @param mirrorType 摄像头镜像类型 {@link ArLiveMirrorType}
     *         - ArLiveMirrorTypeAuto  【默认值】: 默认镜像类型. 在这种情况下，前置摄像头的画面是镜像的，后置摄像头的画面不是镜像的
     *         - ArLiveMirrorTypeEnable:  前置摄像头 和 后置摄像头，都切换为镜像模式
     *         - ArLiveMirrorTypeDisable: 前置摄像头 和 后置摄像头，都切换为非镜像模式
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setRenderMirror(ArLiveMirrorType mirrorType) = 0;

    /**
     * 设置视频编码镜像。
     *
     * @note  编码镜像只影响观众端看到的视频效果。
     * @param mirror 是否镜像
     *         - false【默认值】: 播放端看到的是非镜像画面
     *         - true: 播放端看到的是镜像画面
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setEncoderMirror(bool mirror) = 0;

    /**
     * 设置本地摄像头预览画面的旋转角度。
     *
     * @note  只旋转本地预览画面，不影响推流出去的画面。
     * @param rotation 预览画面的旋转角度 {@link ArLiveRotation}
     *         - ArLiveRotation0【默认值】: 0度, 不旋转
     *         - ArLiveRotation90:  顺时针旋转90度
     *         - ArLiveRotation180: 顺时针旋转180度
     *         - ArLiveRotation270: 顺时针旋转270度
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setRenderRotation(ArLiveRotation rotation) = 0;
    
#ifdef __APPLE__
    virtual int32_t setBeautyEffect(bool enable) = 0;
#endif

#if TARGET_PLATFORM_PHONE

	/**
		* 打开本地摄像头。
		*
		* @param frontCamera 指定摄像头方向是否为前置
		*         - true 【默认值】: 切换到前置摄像头
		*         - false: 切换到后置摄像头
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t startCamera(bool frontCamera) = 0;
#ifdef __ANDROID__
    virtual int startScreenCapture() = 0;
    virtual int stopScreenCapture() = 0;
    virtual void setExVideoEncoderFactory(webrtc::VideoEncoderFactory *video_encoder_factory) = 0;
#endif
#elif TARGET_PLATFORM_DESKTOP

	/**
		* 打开本地摄像头。
		*
		* @note startVirtualCamera，startCamera，startScreenCapture，同一 Pusher 实例下，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流
		*
		* @param cameraId 摄像头标识
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t startCamera(const char* cameraId) = 0;
#endif

	/**
		* 关闭本地摄像头。
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t stopCamera() = 0;

	/**
		* 打开麦克风。
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t startMicrophone() = 0;

	/**
		* 关闭麦克风。
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t stopMicrophone() = 0;

	/**
		* 开启图片推流。
		*
		* @param image 图片
		*
		* @note startVirtualCamera，startCamera，startScreenCapture，同一 Pusher 实例下，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t startVirtualCamera(ArLiveImage* image) = 0;

	/**
		* 关闭图片推流。
		*
		* @return 返回值 {@link ArLiveCode}
		*         - ArLIVE_OK: 成功
		*/
	virtual int32_t stopVirtualCamera() = 0;

    /**
     * 暂停推流器的音频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t pauseAudio() = 0;

    /**
     * 恢复推流器的音频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t resumeAudio() = 0;

    /**
     * 暂停推流器的视频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t pauseVideo() = 0;

    /**
     * 恢复推流器的视频流。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t resumeVideo() = 0;

    /**
     * 开始音视频数据推流。
     *
     * @param url 推流的目标地址，支持任意推流服务端
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 操作成功，开始连接推流目标地址
     *         - ArLIVE_ERROR_INVALID_PARAMETER: 操作失败，url 不合法
     *         - ArLIVE_ERROR_INVALID_LICENSE: 操作失败，license 不合法，鉴权失败
     *         - ArLIVE_ERROR_REFUSED: 操作失败，RTC 不支持同一设备上同时推拉同一个 StreamId
     */
    virtual int32_t startPush(const char* url) = 0;

    /**
     * 停止推送音视频数据。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t stopPush() = 0;

    /**
     * 当前推流器是否正在推流中。
     *
     * @return 是否正在推流
     *         - 1: 正在推流中
     *         - 0: 已经停止推流
     */
    virtual int32_t isPushing() = 0;

    /**
     * 设置推流音频质量。
     *
     * @param quality 音频质量 {@link ArLiveAudioQuality}
     *         - ArLiveAudioQualityDefault 【默认值】: 通用
     *         - ArLiveAudioQualitySpeech: 语音
     *         - ArLiveAudioQualityMusic:  音乐
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_REFUSED: 推流过程中，不允许调整音质
     */
    virtual int32_t setAudioQuality(ArLiveAudioQuality quality) = 0;

    /**
     * 设置推流视频编码参数
     *
     * @param param  视频编码参数 {@link ArLiveVideoEncoderParam}
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t setVideoQuality(const ArLiveVideoEncoderParam& param) = 0;

    /**
     * 获取音效管理对象 {@link ArAudioEffectManager}。
     *
     * 通过音效管理，您可以使用以下功能：
     * - 调整麦克风收集的人声音量。
     * - 设置混响和变声效果。
     * - 开启耳返，设置耳返音量。
     * - 添加背景音乐，调整背景音乐的播放效果。
     */
    virtual ArAudioEffectManager* getAudioEffectManager() = 0;

    /**
     * 获取设备管理对象 {@link ArDeviceManager}。
     *
     * 通过设备管理，您可以使用以下功能：
     * - 切换前后摄像头。
     * - 设置自动聚焦。
     * - 设置摄像头缩放倍数。
     * - 打开或关闭闪光灯。
     * - 切换耳机或者扬声器。
     * - 修改音量类型(媒体音量或者通话音量)。
     */
    virtual ArDeviceManager* getDeviceManager() = 0;

    /**
     * 截取推流过程中的本地画面。
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_REFUSED: 已经停止推流，不允许调用截图操作
     */
    virtual int32_t snapshot() = 0;

    /**
     * 设置推流器水印。默认情况下，水印不开启。
     *
     * 水印的位置是通过 x, y, scale 来指定的。
     * - x：水印的坐标，取值范围为0 - 1的浮点数。
     * - y：水印的坐标，取值范围为0 - 1的浮点数。
     * - scale：水印的大小比例，取值范围为0 - 1的浮点数。
     *
     * @param watermarkPath 水印图片文件路径，为 nullptr 则等同于关闭水印
     * @param x             水印显示的左上角 x 轴偏移
     * @param y             水印显示的左上角 y 轴偏移
     * @param scale         水印显示的宽度占画面宽度比例（水印按该参数等比例缩放显示）
     *
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *
     * @note watermarkPath
     * 1、在 iOS/Mac 坏境下，如果图片存放在 .xcassets 中，请直接传入文件名：
     * self.pusher->setWatermark(“imageName”, 0.1, 0.1, 0.2);
     * 2、在 Android 坏境，如果图片存放在 assets 目录下，请直接传入文件名或者路径名：
     * self.pusher->setWatermark(“imageName.png”, 0.1, 0.1, 0.2);
     * 其它没有列举到的情况，按照各平台的方式获取文件路径并传入即可。
     */
    virtual int32_t setWatermark(const char* watermarkPath, float x, float y, float scale) = 0;

    /**
     * 启用采集音量大小提示。
     *
     * 开启后可以在 {@link ArLivePusherObserver#onMicrophoneVolumeUpdate(int)} 回调中获取到 SDK 对音量大小值的评估。
     * @param intervalMs 决定了 onMicrophoneVolumeUpdate 回调的触发间隔，单位为ms，最小间隔为100ms，如果小于等于0则会关闭回调，建议设置为300ms；【默认值】：0，不开启
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t enableVolumeEvaluation(int32_t intervalMs) = 0;

    /**
     * 开启/关闭自定义视频采集。
     *
     * 在自定义视频采集模式下，SDK 不再从摄像头采集图像，只保留编码和发送能力。
     * @note  需要在 {@link ArLivePusher#startPush(String)} 之前调用，才会生效。
     * @param enable true: 开启自定义采集; false: 关闭自定义采集。【默认值】: false
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t enableCustomVideoCapture(bool enable) = 0;

    /**
     * 在自定义视频采集模式下，将采集的视频数据发送到SDK。
     *
     * 在自定义视频采集模式下，SDK不再采集摄像头数据，仅保留编码和发送功能。
     *
     * @note  需要在 {@link ArLivePusher#startPush(String)} 之前调用 {@link ArLivePusher#enableCustomVideoCapture(boolean)} 开启自定义采集。
     * @param videoFrame 向 SDK 发送的 视频帧数据 {@link ArLiveVideoFrame}
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     *         - ArLIVE_ERROR_INVALID_PARAMETER: 发送失败，视频帧数据不合法
     *         - ArLIVE_ERROR_REFUSED: 发送失败，您必须先调用 enableCustomVideoCapture 开启自定义视频采集。
     */
    virtual int32_t sendCustomVideoFrame(ArLiveVideoFrame* videoFrame) = 0;

/**
 * 设置本地视频自定义渲染回调。
 *
 * 通过该方法，可以获取解码后的每一帧视频画面，进行自定义渲染处理，添加自定义显示效果。
 *
 * @param enable      是否开启自定义渲染。【默认值】：false
 * @param pixelFormat 自定义渲染回调的视频像素格式 {@link ArLivePixelFormat}。
 * @param bufferType  自定义渲染回调的视频数据格式 {@link ArLiveBufferType}。
 * @return 返回值 {@link ArLiveCode}
 *         - ArLIVE_OK: 成功
 */
#ifdef _WIN32
    virtual int32_t enableCustomRendering(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType) = 0;
#endif

/**
 * 开启/关闭自定义音频采集
 *
 *  @brief 开启/关闭自定义音频采集。<br/>
 *         在自定义音频采集模式下，SDK 不再从麦克风采集声音，只保留编码和发送能力。
 *  @note   需要在 [startPush]({@link ArLivePusher#startPush(String)}) 前调用才会生效。
 *  @param enable true: 开启自定义采集; false: 关闭自定义采集。【默认值】: false
 *  @return 返回值 {@link ArLiveCode}
 *          - ArLIVE_OK: 成功
 */
    virtual int enableCustomAudioCapture(bool enable) = 0;

/**
 * 在自定义音频采集模式下，将采集的音频数据发送到SDK
 *
 *  @brief 在自定义音频采集模式下，将采集的音频数据发送到SDK，SDK不再采集麦克风数据，仅保留编码和发送功能。
 *  @note   需要在 [startPush]({@link ArLivePusher#startPush(String)}) 之前调用  {@link ArLivePusher#enableCustomAudioCapture(boolean)} 开启自定义采集。
 *  @param audioFrame 向 SDK 发送的 音频帧数据 {@link ArLiveAudioFrame}
 *  @return 返回值 {@link ArLiveCode}
 *            - ArLIVE_OK: 成功
 *            - ArLIVE_ERROR_REFUSED: 发送失败，您必须先调用 enableCustomAudioCapture 开启自定义音频采集
 */
    virtual int sendCustomAudioFrame(ArLiveAudioFrame* audioFrame) = 0;

    /**
     * 发送 SEI 消息
     *
     * 播放端 {@link ArLivePlayer} 通过 {@link ArLivePlayerObserver} 中的 `onReceiveSeiMessage` 回调来接收该消息。
     *
     * @param payloadType 数据类型，支持 5、242。推荐填：242
     * @param data        待发送的数据
     * @param dataSize    数据大小
     * @return 返回值 {@link ArLiveCode}
     *         - ArLIVE_OK: 成功
     */
    virtual int32_t sendSeiMessage(int payloadType, const uint8_t* data, uint32_t dataSize) = 0;

#if TARGET_PLATFORM_DESKTOP

    /**
     * 打开系统声音采集
     *
     * 开启后可以采集整个操作系统的播放声音（path 为空）或某一个播放器（path 不为空）的声音，
     * 并将其混入到当前麦克风采集的声音中一起发送到云端。
     *
     * @param path
     *        - path 为空，代表采集整个操作系统的声音。( Windows 平台)
     *        - path 填写 exe 程序（如 QQ音乐）所在的路径，将会启动此程序并只采集此程序的声音。( Windows 平台，采集程序声音仅支持32位 SDK )
     *        - path 默认为空，其他值未定义。（ Mac 平台）
     *
     * @note 此接口目前仅适用于 Windows 、 Mac 平台
     */
    virtual int32_t startSystemAudioLoopback(const char* path = nullptr) = 0;

    /**
     * 关闭系统声音采集
     *
     * @note 此接口目前仅适用于 Windows 、 Mac 平台
     */
    virtual int32_t stopSystemAudioLoopback() = 0;

    /**
     * 开启/关闭视频自定义预处理
     *
     * @param enable      是否开启自定义视频预处理。【默认值】：false
     * @param pixelFormat 自定义视频预处理回调的视频像素格式 {@link ArLivePixelFormat}
     * @param bufferType  自定义视频预处理的视频数据格式 {@link ArLiveBufferType}
     */
    virtual int32_t enableCustomVideoProcess(bool enable, ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType) = 0;

/**
 * 启动屏幕分享
 *
 * @note startVirtualCamera，startCamera，startScreenCapture，同一 Pusher 实例下，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流
 */
#ifdef _WIN32
    virtual void startScreenCapture() = 0;
#endif

/**
 * 停止屏幕采集
 */
#ifdef _WIN32
    virtual void stopScreenCapture() = 0;
#endif

/**
 * 枚举可分享的屏幕窗口
 *
 * 如果您要给您的 App 增加屏幕分享功能，一般需要先显示一个窗口选择界面，这样用户可以选择希望分享的窗口。
 * 通过如下函数，您可以获得可分享窗口的 ID、类型、窗口名称以及缩略图。
 * 拿到这些信息后，您就可以实现一个窗口选择界面，当然，您也可以使用我们在 Demo 源码中已经实现好的一个界面。
 *
 * @note
 * - 返回的列表中包括屏幕和应用窗口，屏幕会在列表的前面几个元素中。
 * - delete IArLiveScreenCaptureSourceList* 指针会导致编译错误，SDK 维护 IArLiveScreenCaptureSourceList 对象的生命周期。
 * - 获取完屏幕窗口列表后请手动调用 IArLiveScreenCaptureSourceList 的 release 方法释放资源，否则可能会引起内存泄漏。
 * - Windows 平台 v8.3 版本后获取窗口列表默认携带最小化窗口，且最小化窗口的缩略图数据默认填充窗口图标数据。
 *
 * @param thumbSize 指定要获取的窗口缩略图大小，缩略图可用于绘制在窗口选择界面上
 * @param iconSize  指定要获取的窗口图标大小
 *
 * @return 窗口列表包括屏幕
 */
#ifdef _WIN32
    virtual IArLiveScreenCaptureSourceList* getScreenCaptureSources(const SIZE& thumbSize, const SIZE& iconSize) = 0;
#endif

/**
 * 设置屏幕分享参数，该方法在屏幕分享过程中也可以调用
 *
 * 如果您期望在屏幕分享的过程中，切换想要分享的窗口，可以再次调用这个函数而不需要重新开启屏幕分享。
 *
 * 支持如下四种情况：
 * - 共享整个屏幕：sourceInfoList 中 type 为 Screen 的 source，captureRect 设为 { 0, 0, 0, 0 }
 * - 共享指定区域：sourceInfoList 中 type 为 Screen 的 source，captureRect 设为非 nullptr，例如 { 100, 100, 300, 300 }
 * - 共享整个窗口：sourceInfoList 中 type 为 Window 的 source，captureRect 设为 { 0, 0, 0, 0 }
 * - 共享窗口区域：sourceInfoList 中 type 为 Window 的 source，captureRect 设为非 nullptr，例如 { 100, 100, 300, 300 }
 *
 * @param source      指定分享源
 * @param captureRect 指定捕获的区域
 * @param property    指定屏幕分享目标的属性，包括捕获鼠标，高亮捕获窗口等，详情参考 ArLiveScreenCaptureProperty 定义
 *
 * @note 设置高亮边框颜色、宽度参数在 Mac 平台不生效
 */
#ifdef _WIN32
    virtual void setScreenCaptureSource(const ArLiveScreenCaptureSourceInfo& source, const RECT& captureRect, const ArLiveScreenCaptureProperty& property) = 0;
#endif

#endif

    /**
     * 显示仪表盘。
     *
     * @param isShow 是否显示。【默认值】：false
     */
    virtual void showDebugView(bool isShow) = 0;

   protected:
    virtual ~IArLivePusher(){};
};

}  // namespace anyrtc

#endif  // ANYRTC_CPP_I_ARLIVEPUSHER_H_
/// @}
