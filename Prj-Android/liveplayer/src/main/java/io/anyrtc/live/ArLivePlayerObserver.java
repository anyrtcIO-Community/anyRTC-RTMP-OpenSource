package io.anyrtc.live;

import android.graphics.Bitmap;
import android.os.Bundle;


public abstract class ArLivePlayerObserver {


    public ArLivePlayerObserver() {
    }

    /**
     * 直播播放器错误通知，播放器出现错误时，会回调该通知
     * @param player 回调该通知的播放器对象
     * @param code 错误码{@link ArLiveCode}
     * @param msg 错误信息
     * @param extraInfo 扩展信息
     */
    public void onError(ArLivePlayer player, int code, String msg, Bundle extraInfo) {
    }

    /**
     * 直播播放器警告通知
     * @param player 回调该通知的播放器对象
     * @param code 警告码{@link ArLiveCode}
     * @param msg 警告信息
     * @param extraInfo 扩展信息
     */
    public void onWarning(ArLivePlayer player, int code, String msg, Bundle extraInfo) {
    }

    /**
     * 直播播放器视频状态变化通知
     * @param player 回调该通知的播放器对象
     * @param status 状态码{@link ArLiveDef.ArLivePlayStatus}
     * @param reason 状态对应的原因{@link ArLiveDef.ArLiveStatusChangeReason}
     * @param extraInfo 扩展信息
     */
    public void onVideoPlayStatusUpdate(ArLivePlayer player, ArLiveDef.ArLivePlayStatus status, ArLiveDef.ArLiveStatusChangeReason reason, Bundle extraInfo) {
    }

    /**
     * 直播播放器音频状态变化通知
     * @param player 回调该通知的播放器对象
     * @param status 状态码{@link ArLiveDef.ArLivePlayStatus}
     * @param reason 状态对应的原因{@link ArLiveDef.ArLiveStatusChangeReason}
     * @param extraInfo 扩展信息
     */
    public void onAudioPlayStatusUpdate(ArLivePlayer player, ArLiveDef.ArLivePlayStatus status, ArLiveDef.ArLiveStatusChangeReason reason, Bundle extraInfo) {
    }

    /**
     * 播放器音量大小回调
     * @param player 回调该通知的播放器对象
     * @param volume 音量大小
     */
    public void onPlayoutVolumeUpdate(ArLivePlayer player, int volume) {
    }

    /**
     * 直播播放器统计数据回调
     * @param player 回调该通知的播放器对象
     * @param statistics 播放器统计数据
     */
    public void onStatisticsUpdate(ArLivePlayer player, ArLiveDef.ArLivePlayerStatistics statistics) {
    }

    /**
     * 截图回调
     * @param player 回调该通知的播放器对象
     * @param image 已截取的视频画面
     */
    public void onSnapshotComplete(ArLivePlayer player, Bitmap image) {
    }

    /**
     * 自定义视频渲染回调
     * @param player 回调该通知的播放器对象
     * @param videoFrame 视频帧数据 {@link ArLiveDef.ArLiveVideoFrame}
     */
    public void onRenderVideoFrame(ArLivePlayer player, ArLiveDef.ArLiveVideoFrame videoFrame) {
    }

    /**
     * 收到 SEI 消息的回调，发送端通过 ArLivePusher 中的 sendSeiMessage 来发送 SEI 消息。
     * @param player 回调该通知的播放器对象。
     * @param payloadType 回调数据的SEI payloadType
     * @param data 数据
     */
    public void onReceiveSeiMessage(ArLivePlayer player, int payloadType, byte[] data) {
    }


    public void onVodPlaybackProcess(ArLivePlayer player,int allDuration, int currentPlaybackTime, int bufferDuration) {

    }

}
