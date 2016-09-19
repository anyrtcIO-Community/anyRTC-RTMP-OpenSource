package org.anyrtc.core;

/**
 * Created by Eric on 2016/7/28.
 */
public interface RTMPGuestHelper {
    //* For RTMPCGuesterEvent
    public void OnRtmplayerOK();
    public void OnRtmplayerStatus(int cacheTime, int curBitrate);
    public void OnRtmplayerCache(int time);
    public void OnRtmplayerClosed(int errcode);
}
