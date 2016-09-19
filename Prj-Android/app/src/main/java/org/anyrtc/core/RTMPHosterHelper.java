package org.anyrtc.core;

/**
 * Created by Eric on 2016/7/25.
 */
public interface RTMPHosterHelper {
    //* RTMP Callback
    public void OnRtmpStreamOK();
    public void OnRtmpStreamReconnecting(int times);
    public void OnRtmpStreamStatus(int delayMs, int netBand);
    public void OnRtmpStreamFailed(int code);
    public void OnRtmpStreamClosed();
}
