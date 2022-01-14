package io.anyrtc.live;

import android.graphics.Bitmap;
import android.os.Bundle;

public abstract class ArLivePusherObserver {

    public ArLivePusherObserver() {

    }

    /**
     * 直播推流器错误通知，推流器出现错误时，会回调该通知
     * @param code 错误码{@link ArLiveCode}
     * @param msg 错误信息
     * @param extraInfo 扩展信息
     */
    public void onError(int code, String msg, Bundle extraInfo) {
    }

    /**
     * 直播推流器警告通知
     * @param code 警告码{@link ArLiveCode}
     * @param msg 警告信息
     * @param extraInfo 扩展信息
     */
    public void onWarning(int code, String msg, Bundle extraInfo) {
    }

    /**
     * 首帧音频采集完成的回调通知
     */
    public void onCaptureFirstAudioFrame() {
    }

    /**
     * 首帧视频采集完成的回调通知
     */
    public void onCaptureFirstVideoFrame() {
    }

    /**
     * 麦克风采集音量值回调
     * @param volume 音量大小<br/>
     * 调用 {@link ArLivePusher#enableVolumeEvaluation(int)} 开启采集音量大小提示之后，会收到这个回调通知。
     */
    public void onMicrophoneVolumeUpdate(int volume) {
    }

    /**
     * 推流器连接状态回调通知
     * @param status 推流器连接状态 {@link ArLiveDef.ArLivePushStatus}
     * @param msg 连接状态信息
     * @param extraInfo 扩展信息
     */
    public void onPushStatusUpdate(ArLiveDef.ArLivePushStatus status, String msg, Bundle extraInfo) {
    }

    /**
     * 直播推流器统计数据回调
     * @param statistics 推流器统计数据 {@link ArLiveDef.ArLivePusherStatistics}
     */
    public void onStatisticsUpdate(ArLiveDef.ArLivePusherStatistics statistics) {
    }

    /**
     * 截图回调
     * @param image 已截取的视频画面
     */
    public void onSnapshotComplete(Bitmap image) {
    }


    /**
     * SDK 内部的 OpenGL 环境的创建通知
     */
    public void onGLContextCreated() {
    }

    /**
     *
     * @param srcFrame
     * @param dstFrame
     * @return
     */
    public int onProcessVideoFrame(ArLiveDef.ArLiveVideoFrame srcFrame, ArLiveDef.ArLiveVideoFrame dstFrame) {
        return 0;
    }

    /**
     * SDK 内部的 OpenGL 环境的销毁通知
     */
    public void onGLContextDestroyed() {
    }

    public void onSetMixTranscodingConfig(int code, String msg) {
    }
}
