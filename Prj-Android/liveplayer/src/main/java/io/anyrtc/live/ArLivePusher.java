package io.anyrtc.live;

import android.graphics.Bitmap;

import org.webrtc.SurfaceViewRenderer;
import org.webrtc.TextureViewRenderer;

import io.anyrtc.live.internal.ArDeviceManagerImpl;

public abstract class ArLivePusher{

    /**
     * 设置推流器回调。<br/>
     * 通过设置回调，可以监听 ArLivePusher 推流器的一些回调事件， 包括推流器状态、音量回调、统计数据、警告和错误信息等。
     * @param observer
     */
    public abstract void setObserver(ArLivePusherObserver observer);

    /**
     * 设置本地摄像头预览 View。<br/>
     * 本地摄像头采集到的画面，经过美颜、脸形调整、滤镜等多种效果叠加之后，最终会显示到传入的 View 上。
     * @param view 本地摄像头预览 View
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderView(TextureViewRenderer view);

    /**
     * 设置本地摄像头预览 View。<br/>
     * 本地摄像头采集到的画面，经过美颜、脸形调整、滤镜等多种效果叠加之后，最终会显示到传入的 View 上。
     * @param view 本地摄像头预览 View
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderView(SurfaceViewRenderer view);

    /**
     * 设置本地摄像头预览镜像。<br/>
     * 本地摄像头分为前置摄像头和后置摄像头，系统默认情况下，是前置摄像头镜像，后置摄像头不镜像，这里可以修改前置后置摄像头的默认镜像类型。
     * @param mirrorType 摄像头镜像类型{@link ArLiveDef.ArLiveMirrorType}
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderMirror(ArLiveDef.ArLiveMirrorType mirrorType);

    /**
     * 设置视频编码镜像。(编码镜像只影响观众端看到的视频效果)
     * @param mirror 是否镜像
     * <ul>
     * <li>false【默认值】: 播放端看到的是非镜像画面</li>
     * <li>true: 播放端看到的是镜像画面</li>
     * </ul>
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setEncoderMirror(boolean mirror);


    /**
     * 设置本地摄像头预览画面的旋转角度。(只旋转本地预览画面，不影响推流出去的画面)
     * @param rotation 预览画面的旋转角度 {@link ArLiveDef.ArLiveRotation}
     * <ul>
     * <li>ArLiveRotation0【默认值】: 0度, 不旋转</li>
     * <li>ArLiveRotation90: 顺时针旋转90度 </li>
     * <li>ArLiveRotation180: 顺时针旋转180度</li>
     * <li>ArLiveRotation270: 顺时针旋转270度 </li>
     * </ul>
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderRotation(ArLiveDef.ArLiveRotation rotation);

    /**
     * 打开本地摄像头。
     * @param frontCamera 指定摄像头方向是否为前置
     *  <ul>
     * <li>true 【默认值】: 切换到前置摄像头</li>
     * <li>false: 切换到后置摄像头 </li>
     * </ul>
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int startCamera(boolean frontCamera);

    /**
     * 关闭本地摄像头。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int stopCamera();

    /**
     * 打开麦克风。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int startMicrophone();

    /**
     * 关闭麦克风。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int stopMicrophone();


    /**
     * 开启屏幕采集（安卓5.0以上可用）
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int startScreenCapture();

    /**
     * 停止屏幕采集（安卓5.0以上可用）
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int stopScreenCapture();

    /**
     * 开启图片推流
     * startVirtualCamera、startCamera 和 startScreenCapture，仅有一个能上行，三者为覆盖关系。例如先调用 startCamera，后调用 startVirtualCamera。此时表现为暂停摄像头推流，开启图片推流。
     * @param var1 图片
     * @return
     */
    public abstract int startVirtualCamera(Bitmap var1);

    /**
     * 关闭图片推流。
     * @return
     */
    public abstract int stopVirtualCamera();

    /**
     * 暂停推流器的音频流
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int pauseAudio();

    /**
     * 恢复推流器的音频流
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int resumeAudio();

    /**
     * 暂停推流器的视频流
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int pauseVideo();

    /**
     * 恢复推流器的视频流
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int resumeVideo();


    /**
     * 开始音视频数据推流。
     * @param url 推流的目标地址，支持任意推流服务端
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK: 操作成功，开始连接推流目标地址</li>
     *     <li>ARLIVE_ERROR_INVALID_PARAMETER: 操作失败，url 不合法</li>
     *     <li>ARLIVE_ERROR_INVALID_LICENSE: 操作失败，license 不合法，鉴权失败</li>
     *     <li>ARLIVE_ERROR_REFUSED: 操作失败，RTC 不支持同一设备上同时推拉同一个 StreamId</li>
     * </ul>
     */
    public abstract int startPush(String url);

    /**
     * 停止推送音视频数据。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int stopPush();

    /**
     * 当前推流器是否正在推流中。
     * @return 是否正在推流
     * <ul>
     *     <li>1: 正在推流中</li>
     *     <li>0: 已经停止推流</li>
     * </ul>
     */
    public abstract int isPushing();


    /**
     * 设置推流音频质量。
     * @param quality 音频质量 {@link ArLiveDef.ArLiveAudioQuality}
     * <ul>
     *     <li>ArLiveAudioQualityDefault 【默认值】: 通用</li>
     *     <li>ArLiveAudioQualitySpeech: 语音</li>
     *     <li>ArLiveAudioQualityMusic: 音乐</li>
     * </ul>
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     *     <li>ARLIVE_ERROR_REFUSED:推流过程中，不允许调整音质</li>
     * </ul>
     */
    public abstract int setAudioQuality(ArLiveDef.ArLiveAudioQuality quality);

    /**
     * 设置推流视频编码参数
     * @param param 视频编码参数 {@link ArLiveDef.ArLiveVideoEncoderParam}
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setVideoQuality(ArLiveDef.ArLiveVideoEncoderParam param);

    /**
     * 获取美颜管理对象 {@link ArBeautyManager}
     * @return 返回美颜管理对象
     */
    public abstract ArBeautyManager getBeautyManager();


    /**
     * 获取设备管理器
     * @return 返回设备管理器 {@link ArDeviceManagerImpl}
     */
    public abstract ArDeviceManagerImpl getDeviceManager();
    /**
     * 截取推流过程中的本地画面。
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     *     <li>ARLIVE_ERROR_REFUSED: 已经停止推流，不允许调用截图操作</li>
     * </ul>
     */
    public abstract int snapshot();


    /**
     * 设置推流器水印。默认情况下，水印不开启。
     * @param image 水印图片。如果该值为 null，则等效于禁用水印
     * @param x 水印的横坐标，取值范围为0 - 1的浮点数。
     * @param y 水印的纵坐标，取值范围为0 - 1的浮点数。
     * @param scale 水印图片的缩放比例，取值范围为0 - 1的浮点数。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setWatermark(Bitmap image, float x, float y, float scale);

    /**
     * 启用采集音量大小提示。<br/>
     * 开启后可以在 {@link ArLivePusherObserver#onMicrophoneVolumeUpdate(int)} 回调中获取到 SDK 对音量大小值的评估。
     * @param intervalMs 决定了 onMicrophoneVolumeUpdate 回调的触发间隔，单位为ms，最小间隔为100ms，如果小于等于0则会关闭回调，建议设置为300ms；【默认值】：0，不开启
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int enableVolumeEvaluation(int intervalMs);


    /**
     * 开启/关闭自定义视频处理。<br/>
     * RTMP 支持格式： format = ArLivePixelFormatTexture2D && buffetType = ArLiveBufferTypeTexture <br/>
     * </>RTC 支持格式： format = ArLivePixelFormatTexture2D && bufferType = ArLiveBufferTypeTexture format = ArLivePixelFormatI420 && bufferType = ArLiveBufferTypeByteBuffer format = ArLivePixelFormatI420 && bufferType = ArLiveBufferTypeByteArray
     * @param enable true: 开启; false: 关闭。【默认值】: false
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     *     <li>ARLIVE_ERROR_NOT_SUPPORTED: 不支持的格式</li>
     * </ul>
     */
    public abstract int enableCustomVideoProcess(boolean enable, ArLiveDef.ArLivePixelFormat pixelFormat, ArLiveDef.ArLiveBufferType bufferType);

    /**
     * 开启/关闭自定义视频采集。<br/>
     * 在自定义视频采集模式下，SDK 不再从摄像头采集图像，只保留编码和发送能力。
     * @param enable true: 开启自定义采集; false: 关闭自定义采集。【默认值】: false
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int enableCustomVideoCapture(boolean enable);

    /**
     * 在自定义视频采集模式下，将采集的视频数据发送到SDK。<br/>
     * 在自定义视频采集模式下，SDK不再采集摄像头数据，仅保留编码和发送功能。<br/>
     * 需要在 {@link ArLivePusher#startPush(String)} 之前调用 {@link ArLivePusher#enableCustomVideoCapture(boolean)}开启自定义采集。
     * @param videoFrame 向 SDK 发送的 视频帧数据 {@link ArLiveDef.ArLiveVideoFrame}
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     *     <li>ARLIVE_ERROR_INVALID_PARAMETER: 发送失败，视频帧数据不合法</li>
     *     <li>ARLIVE_ERROR_REFUSED: 发送失败，您必须先调用 enableCustomVideoCapture 开启自定义视频采集。</li>
     * </ul>
     */
    public abstract int sendCustomVideoFrame(ArLiveDef.ArLiveVideoFrame videoFrame);

    /**
     * 开启/关闭自定义音频采集。<br/>
     * 在自定义音频采集模式下，SDK 不再从麦克风采集声音，只保留编码和发送能力。<br/>
     * 开启/关闭自定义音频采集</br>
     * 需要在 {@link ArLivePusher#startPush(String)} 前调用才会生效。
     * @param enable true: 开启自定义采集; false: 关闭自定义采集。【默认值】: false
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int enableCustomAudioCapture(boolean enable);


    /**
     * 在自定义音频采集模式下，将采集的音频数据发送到SDK，SDK不再采集麦克风数据，仅保留编码和发送功能。<br/>
     * 在自定义音频采集模式下，将采集的音频数据发送到SDK<br/>
     * 需要在 startPush 之前调用 {@link ArLivePusher#enableCustomAudioCapture(boolean)} 开启自定义采集。
     * @param audioFrame 向 SDK 发送的 音频帧数据 {@link ArLiveDef.ArLiveAudioFrame}
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     *     <li>ARLIVE_ERROR_REFUSED: 发送失败，您必须先调用 enableCustomAudioCapture 开启自定义音频采集</li>
     * </ul>
     */
    public abstract int sendCustomAudioFrame(ArLiveDef.ArLiveAudioFrame audioFrame);

    /**
     * 发送 SEI 消息<br/>
     * 播放端 {@link ArLivePlayer} 通过 {@link ArLivePlayerObserver} 中的 onReceiveSeiMessage 回调来接收该消息。
     * @param payloadType 数据类型，支持 5、242。推荐填：242
     * @param data 	待发送的数据
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK:成功</li>
     * </ul>
     */
    public abstract int sendSeiMessage(int payloadType, byte[] data);

//    public abstract void showDebugView(boolean var1);

//    public abstract int setProperty(String var1, Object var2);

//    public abstract int setMixTranscodingConfig(ArLiveDef.ArLiveTranscodingConfig var1);

//    public abstract void release();
























}
