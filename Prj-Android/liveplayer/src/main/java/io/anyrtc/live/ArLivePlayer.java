package io.anyrtc.live;

import android.view.SurfaceView;
import android.view.TextureView;

import org.webrtc.SurfaceViewRenderer;
import org.webrtc.TextureViewRenderer;

public abstract class ArLivePlayer {

    /**
     * 设置播放器回调。
     *
     * 通过设置回调，可以监听 ArLivePlayer 播放器的一些回调事件， 包括播放器状态、播放音量回调、音视频首帧回调、统计数据、警告和错误信息等。
     * @param observer 播放器的回调目标对象,更多信息请查看{@link ArLivePlayerObserver}
     */
    public abstract void setObserver(ArLivePlayerObserver observer);

    /**
     * 设置播放器的视频渲染 View。 该控件负责显示视频内容。
     * @param view 播放器渲染 View
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderView(TextureViewRenderer view);

    /**
     * 设置播放器的视频渲染 View。 该控件负责显示视频内容。
     * @param view 播放器渲染 View
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setRenderView(SurfaceViewRenderer view);

    /**
     * 设置播放器画面的旋转角度。
     * @param rotation 旋转角度 {@link ArLiveDef.ArLiveRotation}
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
     * 设置画面的填充模式。
     * @param var1
     * @return
     */
    public abstract int setRenderFillMode(ArLiveDef.ArLiveFillMode var1);


    /**
     * 开始播放音视频流。
     * @param url 音视频流的播放地址，支持 RTMP, HTTP-FLV, WEBRTC等。
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK: 操作成功，开始连接并播放</li>
     *     <li>ARLIVE_ERROR_INVALID_PARAMETER:操作失败，url 不合法</li>
     *     <li>ARLIVE_ERROR_REFUSED: RTC 不支持同一设备上同时推拉同一个 StreamId</li>
     * </ul>
     */
    public abstract int startPlay(String url);

    /**
     * 停止播放音视频流。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int stopPlay();

    /**
     * 播放器是否正在播放中。
     * @return 是否正在播放
     * <ul>
     *     <li>1: 正在播放中</li>
     *     <li>0: 已经停止播放</li>
     * </ul>
     */
    public abstract int isPlaying();

    /**
     * 暂停播放器的音频流
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int pauseAudio();

    /**
     * 恢复播放器的音频流。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int resumeAudio();

    /**
     * 暂停播放器的视频流。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int pauseVideo();

    /**
     * 恢复播放器的视频流。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int resumeVideo();


    /**
     * 设置播放器音量。
     * @param volume 音量大小，取值范围0 - 100。【默认值】: 100
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int setPlayoutVolume(int volume);

    /**
     * 设置播放器缓存自动调整的最小和最大时间 ( 单位：秒 )。
     * @param minTime 缓存自动调整的最小时间，取值需要大于0。【默认值】：1
     * @param maxTime 缓存自动调整的最大时间，取值需要大于0。【默认值】：5
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     *     <li>ARLIVE_OK: 成功</li>
     *     <li>ARLIVE_ERROR_INVALID_PARAMETER: 操作失败，minTime 和 maxTime 需要大于0</li>
     *     <li>ARLIVE_ERROR_REFUSED: 播放器处于播放状态，不支持修改缓存策略</li>
     * </ul>
     */
    public abstract int setCacheParams(float minTime, float maxTime);

    /**
     * 启用播放音量大小提示。</br>
     * 开启后可以在 {@link ArLivePlayerObserver#onPlayoutVolumeUpdate(ArLivePlayer, int)}回调中获取到 SDK 对音量大小值的评估。
     * @param intervalMs 决定了 onPlayoutVolumeUpdate 回调的触发间隔，单位为ms，最小间隔为100ms，如果小于等于0则会关闭回调，建议设置为300ms；【默认值】：0，不开启
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int enableVolumeEvaluation(int intervalMs);

    /**
     * 截取播放过程中的视频画面。
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     * <li>ARLIVE_OK: 成功</li>
     * <li>ARLIVE_ERROR_REFUSED:播放器处于停止状态，不允许调用截图操作</li>
     * </ul>
     */
    public abstract int snapshot();

    /**
     * 设置视频自定义渲染回调。</br>
     * 通过该方法，可以获取解码后的每一帧视频画面，进行自定义渲染处理，添加自定义显示效果。
     * @param enable 是否开启自定义渲染。【默认值】：false
     * @param pixelFormat 自定义渲染回调的视频像素格式 {@link io.anyrtc.live.ArLiveDef.ArLivePixelFormat}
     * @param bufferType 自定义渲染回调的视频像素格式 {@link io.anyrtc.live.ArLiveDef.ArLiveBufferType}
     * @return 返回值 {@link ArLiveCode}
     * <ul>
     * <li>ARLIVE_OK: 成功</li>
     * <li>ARLIVE_ERROR_NOT_SUPPORTED:像素格式或者数据格式不支持</li>
     * </ul>
     */
    public abstract int enableCustomRendering(boolean enable, ArLiveDef.ArLivePixelFormat pixelFormat, ArLiveDef.ArLiveBufferType bufferType);

    /**
     * 开启接收 SEI 消息
     * @param enable true: 开启接收 SEI 消息; false: 关闭接收 SEI 消息。【默认值】: false
     * @param payloadType 指定接收 SEI 消息的 payloadType，支持 5、242，请与发送端的 payloadType 保持一致。
     * @return 返回值 {@link ArLiveCode}
     * <ul><li>ARLIVE_OK:成功</li></ul>
     */
    public abstract int enableReceiveSeiMessage(boolean enable, int payloadType);

//    public abstract void showDebugView(boolean var1);






















}
