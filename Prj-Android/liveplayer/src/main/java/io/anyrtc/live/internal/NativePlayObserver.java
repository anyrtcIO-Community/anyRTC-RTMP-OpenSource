package io.anyrtc.live.internal;

import android.graphics.Bitmap;

import org.webrtc.CalledByNative;

import java.nio.ByteBuffer;

import io.anyrtc.live.ArLiveDef;
import io.anyrtc.live.ArLivePlayer;

interface NativePlayObserver {
    @CalledByNative
    public void onError(int code, String msg, String extraInfo);
    @CalledByNative
    public void onWarning(int code, String msg, String extraInfo);
    @CalledByNative
    public void onVodPlaybackProcess(int allDuration, int currentPlaybackTime, int bufferDuration) ;
    @CalledByNative
    public void onVideoPlayStatusUpdate(int status, int reason, String extraInfo);
    @CalledByNative
    public void onAudioPlayStatusUpdate(int status, int reason, String extraInfo);
    @CalledByNative
    public void onPlayoutVolumeUpdate(int volume);
    @CalledByNative
    public void onStatisticsUpdate(int appCpu,int systemCpu, int width, int height, int fps, int videoBitrate, int audioBitrate);
    @CalledByNative
    public void onRenderVideoFrame(int pixelFormat,int bufferType,byte[] data,ByteBuffer buffer,int width, int height,int rotation);
    @CalledByNative
    public void onReceiveSeiMessage(int payloadType, byte[] data);
}
