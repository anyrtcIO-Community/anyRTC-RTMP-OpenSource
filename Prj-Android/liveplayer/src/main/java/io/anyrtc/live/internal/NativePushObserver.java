package io.anyrtc.live.internal;

import org.webrtc.CalledByNative;

import java.nio.ByteBuffer;

interface NativePushObserver {
    @CalledByNative
    public void onError(int code, String msg, String extraInfo);
    @CalledByNative
    public void onWarning(int code, String msg, String extraInfo);
    @CalledByNative
    public void onPushStatusUpdate(int status, String msg, String extraInfo);
    @CalledByNative
    public void onStatisticsUpdate(int appCpu, int systemCpu, int width,int height,int fps,int videoBitrate,int audioBitrate);
    @CalledByNative
    public void onCaptureFirstAudioFrame();
    @CalledByNative
    public void onCaptureFirstVideoFrame();
    @CalledByNative
    public void onGLContextCreated();
    @CalledByNative
    public void onGLContextDestroyed();
    @CalledByNative
    public void onMicrophoneVolumeUpdate(int volume);
}
