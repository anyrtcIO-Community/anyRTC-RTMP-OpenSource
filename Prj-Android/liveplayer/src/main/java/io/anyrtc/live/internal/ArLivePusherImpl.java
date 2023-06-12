package io.anyrtc.live.internal;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;


import org.webrtc.EglRenderer;
import org.webrtc.RendererCommon;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.TextureViewRenderer;
import org.webrtc.VideoSink;

import io.anyrtc.live.ArBeautyManager;
import io.anyrtc.live.ArDeviceManager;
import io.anyrtc.live.ArLiveCode;
import io.anyrtc.live.ArLiveDef;
import io.anyrtc.live.ArLivePusher;
import io.anyrtc.live.ArLivePusherObserver;

public class ArLivePusherImpl extends ArLivePusher {

    private NativeInstance nativeInstance;
    private Handler applicationHandler;
    private VideoSink renderView;
    private Context context;
    private ArLivePusherObserver pusherObserver;
    private ArLiveDef.ArLiveMirrorType curMirrorType = ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeAuto;
    private ArDeviceManagerImpl deviceManager;
    private ArBeautyManager beautyManager;
    private boolean isStartCamera = false;
    private boolean isScreen = false;
    private long nativeId;



    protected void setHandler(Handler applicationHandler, Context context) {
        this.applicationHandler = applicationHandler;
        this.context=context;
    }

    @Override
    public void setObserver(ArLivePusherObserver var1) {
        this.pusherObserver = var1;
    }

    private class innerArLivePusherObserver implements NativePushObserver{

        @Override
        public void onError(int code, String msg, String extraInfo) {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onError(code, msg, null);
                }
            });
        }

        @Override
        public void onWarning(int code, String msg, String extraInfo) {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onWarning(code, msg, null);
                }
            });
        }

        @Override
        public void onPushStatusUpdate(int status, String msg, String extraInfo) {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onPushStatusUpdate(ArLiveDef.ArLivePushStatus.values()[status], msg, null);
                }
            });
        }

        @Override
        public void onStatisticsUpdate(int appCpu, int systemCpu, int width, int height, int fps, int videoBitrate, int audioBitrate) {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    ArLiveDef.ArLivePusherStatistics statistics = new ArLiveDef.ArLivePusherStatistics();
                    statistics.appCpu = appCpu;
                    statistics.systemCpu = systemCpu;
                    statistics.width = width;
                    statistics.height = height;
                    statistics.fps = fps;
                    statistics.audioBitrate = audioBitrate;
                    statistics.videoBitrate = videoBitrate;
                    pusherObserver.onStatisticsUpdate(statistics);
                }
            });
        }

        @Override
        public void onCaptureFirstAudioFrame() {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onCaptureFirstAudioFrame();
                }
            });
        }

        @Override
        public void onCaptureFirstVideoFrame() {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onCaptureFirstVideoFrame();
                }
            });
        }

        @Override
        public void onGLContextCreated() {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onGLContextCreated();
                }
            });
        }

        @Override
        public void onGLContextDestroyed() {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onGLContextDestroyed();
                }
            });
        }

        @Override
        public void onMicrophoneVolumeUpdate(int volume) {
            applicationHandler.post(() -> {
                if (pusherObserver!=null){
                    pusherObserver.onMicrophoneVolumeUpdate(volume);
                }
            });
        }


    }

    @Override
    public int setRenderView(TextureViewRenderer view) {
        if (view == null){
            return -1;
        }
        view.init(VideoCapturerDevice.getEglBase().getEglBaseContext(),null);
        nativeInstance.nativeSetRenderView(nativeId,view);
        view.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL);
        renderView = view;
        return 0;
    }

    @Override
    public int setRenderView(SurfaceViewRenderer view) {
        if (view == null){
            return -1;
        }
        view.init(VideoCapturerDevice.getEglBase().getEglBaseContext(),null);
        nativeInstance.nativeSetRenderView(nativeId,view);
        view.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL);
        renderView = view;
        return 0;
    }

    @Override
    public int setRenderMirror(ArLiveDef.ArLiveMirrorType var1) {
        if (renderView!=null){
            this.curMirrorType = var1;
            if (renderView instanceof TextureViewRenderer){
                switch (var1){
                    case ArLiveMirrorTypeAuto:
                        ((TextureViewRenderer) renderView).setMirror(deviceManager.isFrontCamera());
                        break;
                    case ArLiveMirrorTypeEnable:
                        ((TextureViewRenderer) renderView).setMirror(true);
                        break;
                    case ArLiveMirrorTypeDisable:
                        ((TextureViewRenderer) renderView).setMirror(false);
                        break;
                }
            }else if (renderView instanceof SurfaceViewRenderer){
                switch (var1){
                    case ArLiveMirrorTypeAuto:
                        ((SurfaceViewRenderer) renderView).setMirror(deviceManager.isFrontCamera());
                        break;
                    case ArLiveMirrorTypeEnable:
                        ((SurfaceViewRenderer) renderView).setMirror(true);
                        break;
                    case ArLiveMirrorTypeDisable:
                        ((SurfaceViewRenderer) renderView).setMirror(false);
                        break;
                }
            }
        }
        return 0;
    }

    @Override
    public int setEncoderMirror(boolean var1) {
        return nativeInstance.nativeSetEncoderMirror(nativeId,var1);
    }

    @Override
    public int setRenderRotation(ArLiveDef.ArLiveRotation var1) {
        if (renderView!=null){
            if (renderView instanceof TextureViewRenderer){
                switch (var1){
                    case ArLiveRotation0:
                        ((TextureViewRenderer) renderView).setRenderRotation(0);
                        break;
                    case ArLiveRotation90:
                        ((TextureViewRenderer) renderView).setRenderRotation(90);
                        break;
                    case ArLiveRotation180:
                        ((TextureViewRenderer) renderView).setRenderRotation(180);
                        break;
                    case ArLiveRotation270:
                        ((TextureViewRenderer) renderView).setRenderRotation(270);
                        break;
                }
            }else if (renderView instanceof SurfaceViewRenderer){
                switch (var1){
                    case ArLiveRotation0:
                        ((SurfaceViewRenderer) renderView).setRenderRotation(0);
                        break;
                    case ArLiveRotation90:
                        ((SurfaceViewRenderer) renderView).setRenderRotation(90);
                        break;
                    case ArLiveRotation180:
                        ((SurfaceViewRenderer) renderView).setRenderRotation(180);
                        break;
                    case ArLiveRotation270:
                        ((SurfaceViewRenderer) renderView).setRenderRotation(270);
                        break;
                }
            }
        }
        return 0;
    }

    @Override
    public int startCamera(boolean isFrontFaceCamera) {
        deviceManager.setCameraIndex(isFrontFaceCamera);
        nativeInstance.nativeStartCamera(nativeId,isFrontFaceCamera);
        isStartCamera = true;
        return 0;
    }

    @Override
    public int stopCamera() {
        nativeInstance.nativeStopCamera(nativeId);
        isStartCamera = false;
        return 0;
    }

    @Override
    public int startMicrophone() {
        return nativeInstance.nativeStartMicrophone(nativeId);
    }

    @Override
    public int stopMicrophone() {
        return nativeInstance.nativeStopMicrophone(nativeId);
    }



    boolean isNativeOk(){
        return nativeId!=0;
    }


    protected void setSwitchCamera(boolean switching, boolean isFrontFace){
        deviceManager.setSwitchingCamera(switching,isFrontFace);
        if (!switching) {
            if (isFrontFace&&curMirrorType== ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeAuto) {
                if (renderView != null) {
                    if (renderView instanceof TextureViewRenderer) {
                        ((TextureViewRenderer) renderView).setMirror(curMirrorType!= ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeDisable);
                    }
                    if (renderView instanceof SurfaceViewRenderer) {
                        ((SurfaceViewRenderer) renderView).setMirror(curMirrorType!= ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeDisable);
                    }
                }
            } else {
                if (curMirrorType== ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeAuto) {
                    if (renderView != null) {
                        if (renderView instanceof TextureViewRenderer) {
                            ((TextureViewRenderer) renderView).setMirror(false);
                        }
                        if (renderView instanceof SurfaceViewRenderer) {
                            ((SurfaceViewRenderer) renderView).setMirror(false);
                        }
                    }
                }
            }
        }
    }



    @Override
    public int startScreenCapture() {
        if (Build.VERSION.SDK_INT>Build.VERSION_CODES.LOLLIPOP) {
            Intent i = new Intent(context, ArScreenCapture.ArScreenCaptureAssistantActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            i.putExtra("pusherId",nativeId);
            context.startActivity(i);
            return 0;
        }else {
            return -1;
        }
    }

    @Override
    public int stopScreenCapture() {
        if (isScreen) {
            nativeInstance.stopScreenCapture(nativeId);
        }
        return 0;
    }

    @Override
    public int startVirtualCamera(Bitmap var1) {
        if (var1==null){
            return -1;
        }
        String name = var1.getConfig().name();
        return nativeInstance.nativeStartVirtualCamera(nativeId,name.equals("ARGB_8888")?2:1,bitmapToRgba(var1),var1.getWidth(),var1.getHeight());
    }

    public  byte[] bitmapToRgba(Bitmap bitmap) {
        if (bitmap.getConfig() != Bitmap.Config.ARGB_8888)
            throw new IllegalArgumentException("Bitmap must be in ARGB_8888 format");
        int[] pixels = new int[bitmap.getWidth() * bitmap.getHeight()];
        byte[] bytes = new byte[pixels.length * 4];
        bitmap.getPixels(pixels, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(), bitmap.getHeight());
        int i = 0;
        for (int pixel : pixels) {
            // Get components assuming is ARGB
            int A = (pixel >> 24) & 0xff;
            int R = (pixel >> 16) & 0xff;
            int G = (pixel >> 8) & 0xff;
            int B = pixel & 0xff;
            bytes[i++] = (byte) R;
            bytes[i++] = (byte) G;
            bytes[i++] = (byte) B;
            bytes[i++] = (byte) A;
        }
        return bytes;
    }

    @Override
    public int stopVirtualCamera() {
        return nativeInstance.nativeStopVirtualCamera(nativeId);
    }


    @Override
    public int pauseAudio() {
        return nativeInstance.nativePausePusherAudio(nativeId);
    }

    @Override
    public int resumeAudio() {
        return nativeInstance.nativeResumePusherAudio(nativeId);
    }

    @Override
    public int pauseVideo() {
        return nativeInstance.nativePausePusherVideo(nativeId);
    }

    @Override
    public int resumeVideo() {
        return nativeInstance.nativeResumePusherVideo(nativeId);
    }

    @Override
    public int startPush(String pushUrl) {
        if (TextUtils.isEmpty(pushUrl)||!pushUrl.startsWith("rtmp://")){
            return ArLiveCode.ARLIVE_ERROR_INVALID_PARAMETER;
        }
        return nativeInstance.nativeStartPush(nativeId,pushUrl);
    }

    @Override
    public int stopPush() {
        detach();
        return 0;
    }


    @Override
    public int isPushing() {
        return nativeInstance.nativeIsPushing(nativeId);
    }

    @Override
    public int setAudioQuality(ArLiveDef.ArLiveAudioQuality var1) {
        return nativeInstance.nativeSetAudioQuality(nativeId,var1.ordinal());
    }

    @Override
    public int setVideoQuality(ArLiveDef.ArLiveVideoEncoderParam var1) {
        return nativeInstance.nativeSetVideoQuality(nativeId,var1.videoResolution.ordinal(),var1.videoResolutionMode.ordinal(),var1.videoFps,var1.videoBitrate,var1.minVideoBitrate,var1.videoScaleMode.ordinal());
    }

    @Override
    public ArBeautyManager getBeautyManager() {
        if (beautyManager == null){
            beautyManager = new ArBeautyManagerImpl(nativeInstance);
        }
        return beautyManager;
    }

    @Override
    public ArDeviceManagerImpl getDeviceManager() {
        return deviceManager;
    }

    @Override
    public int snapshot() {
        if (isStartCamera&&pusherObserver!=null&&renderView!=null){
                if (renderView instanceof TextureViewRenderer){
                    ((TextureViewRenderer) renderView).addFrameListener(snapListener,1f);
                }else if (renderView instanceof SurfaceViewRenderer){
                    ((SurfaceViewRenderer) renderView).addFrameListener(snapListener,1f);
                }
            return ArLiveCode.ARLIVE_OK;
        }
        return ArLiveCode.ARLIVE_ERROR_REFUSED;
    }

    @Override
    public int setWatermark(Bitmap var1, float var2, float var3, float var4) {
        return 0;
    }

    @Override
    public int enableVolumeEvaluation(int var1) {
        return nativeInstance.nativeEnableVolumeEvaluation(nativeId,var1);
    }

    @Override
    public int enableCustomVideoProcess(boolean var1, ArLiveDef.ArLivePixelFormat var2, ArLiveDef.ArLiveBufferType var3) {
        return 0;
    }

    @Override
    public int enableCustomVideoCapture(boolean var1) {
        return nativeInstance.nativeEnableCustomVideoCapture(nativeId,var1);
    }

    @Override
    public int sendCustomVideoFrame(ArLiveDef.ArLiveVideoFrame var1) {
        return nativeInstance.nativeSendCustomVideoFrame(nativeId,var1.pixelFormat.ordinal(),var1.bufferType.ordinal(),var1.data,var1.buffer,var1.width,var1.height,var1.rotation, var1.stride);
    }

    @Override
    public int enableCustomAudioCapture(boolean var1) {
        return nativeInstance.nativeEnableCustomAudioCapture(nativeId,var1);
    }

    @Override
    public int sendCustomAudioFrame(ArLiveDef.ArLiveAudioFrame var1) {
        return nativeInstance.nativeSendCustomAudioFrame(nativeId,var1.channel,var1.sampleRate,var1.data);
    }

    @Override
    public int sendSeiMessage(int var1, byte[] var2) {
        return nativeInstance.nativeSendSeiMessage(nativeId,var1,var2);
    }


     boolean attach(NativeInstance nativeInstance){
        this.nativeInstance = nativeInstance;
         deviceManager = ArLiveEngineImpl.getInstance().deviceManager;
         deviceManager.setVoipMode(true);
         nativeId = nativeInstance.nativeCreatePushKit();
         nativeInstance.nativeSetPushObserver(new innerArLivePusherObserver(),nativeId);
        return nativeId!=0;
    }

    void detach(){
        nativeInstance.nativeStopPush(nativeId);
        nativeId = 0;
        if (renderView!=null){
            if (renderView instanceof TextureViewRenderer){
                ((TextureViewRenderer) renderView).release();
            }
            if (renderView instanceof SurfaceViewRenderer){
                ((SurfaceViewRenderer) renderView).release();
            }
        }
    }

    protected ArLivePusherImpl getPusher(){
        return this;
    }


    private EglRenderer.FrameListener snapListener = new EglRenderer.FrameListener() {
        @Override
        public void onFrame(Bitmap frame) {
            if (renderView!=null && pusherObserver!=null){
                if (renderView instanceof TextureViewRenderer){
                    pusherObserver.onSnapshotComplete(frame);
                    applicationHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            ((TextureViewRenderer) renderView).removeFrameListener(snapListener);
                        }
                    });

                }else if (renderView instanceof SurfaceViewRenderer){
                    pusherObserver.onSnapshotComplete(frame);
                    applicationHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            ((SurfaceViewRenderer) renderView).removeFrameListener(snapListener);
                        }
                    });

                }

            }
        }
    };
}
