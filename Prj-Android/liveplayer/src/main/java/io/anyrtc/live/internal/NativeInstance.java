package io.anyrtc.live.internal;

import android.graphics.Bitmap;

import org.webrtc.VideoSink;

import java.nio.ByteBuffer;

import io.anyrtc.live.ArDeviceManager;
import io.anyrtc.live.ArLiveDef;
import io.anyrtc.live.ArLivePusherObserver;
import io.anyrtc.live.ArLivePlayerObserver;
import io.anyrtc.live.ArScreenService;

public class NativeInstance {


    private long nativePtr;

    private static NativeInstance nativeInstance;

    public NativeInstance() {
        nativeInstance = this;
        nativePtr = makeNativeInstance(this);
    }

    public static NativeInstance getSharedInstance() {
        return nativeInstance;
    }

    protected native long makeNativeInstance(NativeInstance instance);

    protected native int nativeSetAppInBackground(boolean isBackground);

    protected native void nativeRelease();


    //player
    protected native long nativeCreatePlayKit();

    protected native void nativeSetPlayerobserver(long nativePtr, NativePlayObserver observer);

    protected native int nativeSetPlayerView(long nativeId, VideoSink playSink);

    protected native int nativeStartPlay(long nativePtr, String url);

    protected native int nativeStopPlay(long nativePtr);

    protected native int nativeIsPlaying(long nativePtr);

    protected native int nativeSetRenderRotation(long nativePtr,int rotation);

    protected native int nativePauseAudio(long nativePtr);

    protected native int nativeResumeAudio(long nativePtr);

    protected native int nativePauseVideo(long nativePtr);

    protected native int nativeResumeVideo(long nativePtr);

    protected native int nativeSetPlayoutVolume(long nativePtr, int volume);

    protected native int nativeSetCacheParams(long nativePtr, float minTime, float maxTime);

    protected native int nativeEnableVolumeEvaluation(long nativePtr, int intervalMs);

    protected native int nativeEnableCustomRendering(long nativePtr, boolean enable, int format, int type);

    protected native int nativeEnableReceiveSeiMessage(long nativePtr, boolean enable, int payloadType);

    protected native void nativeShowDebugView(long nativePtr, boolean isShow);

    protected native void nativePlayKitRelease(long nativePtr);

    //pusher
    protected native long nativeCreatePushKit();

    protected native void nativeSetPushObserver(NativePushObserver observer, long nativePtr);

    protected native int nativeSetRenderView(long nativePtr, VideoSink localSink);

    protected native int nativeSetPushRenderRotation(long nativePtr,int rotation);

    protected native int nativeStartCamera(long nativePtr, boolean isFront);

    protected native int nativeStopCamera(long nativePtr);

    protected native int nativeSetEncoderMirror(long nativePtr, boolean mirror);

    protected native int nativeStartMicrophone(long nativePtr);

    protected native int nativeStopMicrophone(long nativePtr);

    protected native int nativeSetBeautyEffect(boolean enable);

    protected native int nativeSetWhitenessLevel(float level);

    protected native int nativeSetBeautyLevel(float level);

    protected native int nativeSetToneLevel(float level);

    protected native int nativeStartVirtualCamera(long nativePtr,int type,byte[] bitmap,int width ,int height);

    protected native int nativeStopVirtualCamera(long nativePtr);

    protected native int nativePausePusherAudio(long nativePtr);

    protected native int nativeResumePusherAudio(long nativePtr);

    protected native int nativePausePusherVideo(long nativePtr);

    protected native int nativeResumePusherVideo(long nativePtr);

    protected native int nativeStartPush(long nativePtr, String url);

    protected native int nativeStopPush(long nativePtr);

    protected native int nativeIsPushing(long nativePtr);

    protected native int nativeSetVideoQuality(long nativePtr, int videoResolution, int videoResolutionMode,int fps,int bitrate,int minBitrate,int scaleMode);

    protected native int nativeSetAudioQuality(long nativePtr,int mode);


    protected native int nativePusherEnableVolumeEvaluation(long nativePtr,int intervalMs);

    //protected native int nativeEnableCustomVideoProcess(boolean var1, ArLiveDef.ArLivePixelFormat var2, ArLiveDef.ArLiveBufferType var3);

    protected native int nativeEnableCustomVideoCapture(long nativePtr,boolean enable);

    protected native int nativeSendCustomVideoFrame(long nativePtr, int pixelFormat, int bufferType, byte[] data, ByteBuffer buffer,int width,int height,int rotation,int stride);

    protected native int nativeEnableCustomAudioCapture(long nativePtr,boolean enable);

    protected native int nativeSendCustomAudioFrame(long nativePtr,int channel,int sampleRate,byte[] data);

    protected native int nativeSendSeiMessage(long nativePtr,int var1, byte[] var2);



    protected native void switchCamera(boolean front);
    protected native float getCameraZoomMaxRatio();
    protected native int setCameraZoomRatio(float var1);
    protected native boolean isAutoFocusEnabled();
    protected native int enableCameraAutoFocus(boolean var1);
    protected native int setCameraFocusPosition(float var1, float var2);
    protected native boolean enableCameraTorch(boolean enable);
    protected native void setCameraCapturerParam(int mode,int width,int height);
    public native void startScreenCapture(long nativePtr);
    protected native void stopScreenCapture(long nativePtr);

}
