package io.anyrtc.live.internal;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import org.json.JSONException;
import org.json.JSONObject;
import org.webrtc.EglRenderer;
import org.webrtc.RendererCommon;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.TextureViewRenderer;
import org.webrtc.ThreadUtils;
import org.webrtc.VideoSink;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.HashMap;
import java.util.concurrent.Callable;

import io.anyrtc.live.ArLiveCode;
import io.anyrtc.live.ArLiveDef;
import io.anyrtc.live.ArLivePlayer;
import io.anyrtc.live.ArLivePlayerObserver;


public class ArLivePlayerImpl extends ArLivePlayer {

    private NativeInstance nativeInstance;
    private final byte[] mPlayerLock = new byte[0];
    private ArLivePlayerObserver arLivePlayEvent;
    private Handler applicationHandler;
    private VideoSink renderView;
    private long nativeId;
    private ArLivePlayerImpl arLivePlayer;
    private boolean isStartPlay = false;

    protected void setHandler(Handler applicationHandler) {
        this.applicationHandler = applicationHandler;
    }


    @Override
    public void setObserver(ArLivePlayerObserver var1) {
        this.arLivePlayEvent = var1;
    }

    @Override
    public int setRenderView(TextureViewRenderer view) {
        if (view == null){
            return -1;
        }
        view.init(VideoCapturerDevice.getEglBase().getEglBaseContext(),null);
        nativeInstance.nativeSetPlayerView(nativeId,view);
        renderView = view;
        return 0;
    }

    @Override
    public int setRenderView(SurfaceViewRenderer view) {
        if (view == null){
            return -1;
        }
        view.init(VideoCapturerDevice.getEglBase().getEglBaseContext(),null);
        nativeInstance.nativeSetPlayerView(nativeId,view);
        renderView = view;
        return 0;
    }

    @Override
    public int setRenderRotation(ArLiveDef.ArLiveRotation var1) {
        return nativeInstance.nativeSetRenderRotation(nativeId,var1.ordinal());
    }

    @Override
    public int setRenderFillMode(ArLiveDef.ArLiveFillMode mode) {
        if (renderView!=null){
            if (renderView instanceof TextureViewRenderer){
                ((TextureViewRenderer) renderView).setScalingType(mode == ArLiveDef.ArLiveFillMode.ArLiveFillModeFill ? RendererCommon.ScalingType.SCALE_ASPECT_FILL: RendererCommon.ScalingType.SCALE_ASPECT_FIT);
            }else if (renderView instanceof SurfaceViewRenderer){
                ((SurfaceViewRenderer) renderView).setScalingType(mode == ArLiveDef.ArLiveFillMode.ArLiveFillModeFill ? RendererCommon.ScalingType.SCALE_ASPECT_FILL: RendererCommon.ScalingType.SCALE_ASPECT_FIT);
            }
        }
        return 0;
    }

    @Override
    public int startPlay(String var1) {
        if (!TextUtils.isEmpty(var1)){
            isStartPlay = true;
            nativeInstance.nativeStartPlay(nativeId,var1);
            return ArLiveCode.ARLIVE_OK;
        }else {
            return ArLiveCode.ARLIVE_ERROR_INVALID_PARAMETER;
        }
    }

    @Override
    public int stopPlay() {
        detach();
        return 0;
    }

    @Override
    public int isPlaying() {
        return nativeInstance.nativeIsPlaying(nativeId);
    }


    @Override
    public int pauseAudio() {
        return nativeInstance.nativePauseAudio(nativeId);
    }

    @Override
    public int resumeAudio() {
        return nativeInstance.nativeResumeAudio(nativeId);
    }

    @Override
    public int pauseVideo() {
       return nativeInstance.nativePauseVideo(nativeId);
    }

    @Override
    public int resumeVideo() {
        return nativeInstance.nativeResumeVideo(nativeId);
    }

    @Override
    public int setPlayoutVolume(int var1) {
        if (var1<=100&&var1>=0){
            return nativeInstance.nativeSetPlayoutVolume(nativeId,var1);
        }
        return ArLiveCode.ARLIVE_ERROR_REFUSED;
    }

    @Override
    public int setCacheParams(float minTime, float maxTime) {
        return nativeInstance.nativeSetCacheParams(nativeId,minTime,maxTime);
    }

    @Override
    public int enableVolumeEvaluation(int var1) {
        return nativeInstance.nativeEnableVolumeEvaluation(nativeId,var1);
    }

    @Override
    public int snapshot() {
        if (!isStartPlay||arLivePlayEvent==null||renderView==null){
            return ArLiveCode.ARLIVE_ERROR_REFUSED;
        }
        if (renderView!=null){
            if (renderView instanceof TextureViewRenderer){
                ((TextureViewRenderer) renderView).addFrameListener(snapListener,1f);
            }else if (renderView instanceof SurfaceViewRenderer){
                ((SurfaceViewRenderer) renderView).addFrameListener(snapListener,1f);
            }
        }
        return ArLiveCode.ARLIVE_OK;
    }

    @Override
    public int enableCustomRendering(boolean var1, ArLiveDef.ArLivePixelFormat format, ArLiveDef.ArLiveBufferType bufferType) {
        return nativeInstance.nativeEnableCustomRendering(nativeId,var1,format.ordinal(),bufferType.ordinal());
    }

    @Override
    public int enableReceiveSeiMessage(boolean var1, int payloadType) {
        return nativeInstance.nativeEnableReceiveSeiMessage(nativeId,var1,payloadType);
    }

//    @Override
//    public void showDebugView(boolean isShow) {
//        nativeInstance.nativeShowDebugView(nativeId,isShow);
//    }

//    @Override
//    public void release() {
//        detach();
//    }


    boolean attach(NativeInstance nativeInstance){
        synchronized (mPlayerLock){
            this.nativeInstance = nativeInstance;
            nativeId = nativeInstance.nativeCreatePlayKit();
            if (nativeId!=0){
                this.nativeInstance.nativeSetPlayerobserver(nativeId,new InnerNativePlayObserver());
                arLivePlayer = this;
                ArLiveEngineImpl.getInstance().deviceManager.setVoipMode(false);
                return true;
            }
            return false;
        }
    }

     void detach(){
         synchronized (mPlayerLock){
             nativeId = 0;
             nativeInstance.nativeStopPlay(nativeId);
             nativeInstance.nativePlayKitRelease(nativeId);
             isStartPlay = false;
             if (renderView!=null){
                 if (renderView instanceof TextureViewRenderer){
                     ((TextureViewRenderer) renderView).release();
                 }else if (renderView instanceof SurfaceViewRenderer){
                     ((SurfaceViewRenderer) renderView).release();
                 }
             }
         }
     }

     boolean isNativeOk(){
        return nativeId!=0;
     }



    private class InnerNativePlayObserver implements NativePlayObserver {
        @Override
        public void onAudioPlayStatusUpdate(int status, int reason, String extraInfo) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        Bundle bundle = new Bundle();
                        arLivePlayEvent.onAudioPlayStatusUpdate(arLivePlayer, ArLiveDef.ArLivePlayStatus.values()[status], ArLiveDef.ArLiveStatusChangeReason.values()[reason],bundle);
                    }
                }
            });
        }

        @Override
        public void onVideoPlayStatusUpdate(int status, int reason, String extraInfo) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        Bundle bundle = new Bundle();
                        arLivePlayEvent.onVideoPlayStatusUpdate(arLivePlayer, ArLiveDef.ArLivePlayStatus.values()[status], ArLiveDef.ArLiveStatusChangeReason.values()[reason],bundle);
                    }
                }
            });
        }

        @Override
        public void onPlayoutVolumeUpdate(int volume) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        arLivePlayEvent.onPlayoutVolumeUpdate(arLivePlayer,volume);
                    }
                }
            });
        }

        @Override
        public void onStatisticsUpdate(int appCpu, int systemCpu, int width, int height, int fps, int videoBitrate, int audioBitrate) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        arLivePlayEvent.onStatisticsUpdate(arLivePlayer,new ArLiveDef.ArLivePlayerStatistics(appCpu,systemCpu,width,height,fps,videoBitrate,audioBitrate));
                    }
                }
            });
        }

        @Override
        public void onRenderVideoFrame(int pixelFormat, int bufferType,  byte[] data, ByteBuffer buffer, int width, int height, int rotation) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                   if (arLivePlayEvent!=null){
                       arLivePlayEvent.onRenderVideoFrame(arLivePlayer,new ArLiveDef.ArLiveVideoFrame(ArLiveDef.ArLivePixelFormat.values()[pixelFormat], ArLiveDef.ArLiveBufferType.values()[bufferType],null,data,buffer,width,height,rotation,0));
                   }
                }
            });
        }

        @Override
        public void onError(int code, String msg, String extraInfo) {
            applicationHandler.post(() -> {
                if (arLivePlayEvent!=null){
                    arLivePlayEvent.onError(arLivePlayer,code,msg,null);
                }
            });
        }

        @Override
        public void onWarning(int code, String msg, String extraInfo) {
            applicationHandler.post(() -> {
                if (arLivePlayEvent!=null){
                    arLivePlayEvent.onWarning(arLivePlayer,code,msg,null);
                }
            });
        }

        @Override
        public void onVodPlaybackProcess(int allDuration, int currentPlaybackTime, int bufferDuration) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        arLivePlayEvent.onVodPlaybackProcess(arLivePlayer,allDuration,currentPlaybackTime,bufferDuration);
                    }
                }
            });
        }


        @Override
        public void onReceiveSeiMessage(int payloadType, byte[] data) {
            applicationHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (arLivePlayEvent!=null){
                        arLivePlayEvent.onReceiveSeiMessage(arLivePlayer,payloadType,data);
                    }
                }
            });
        }
    };

    private EglRenderer.FrameListener snapListener = new EglRenderer.FrameListener() {
        @Override
        public void onFrame(Bitmap frame) {
            if (renderView!=null && arLivePlayEvent!=null){
                if (renderView instanceof TextureViewRenderer){
                    applicationHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            arLivePlayEvent.onSnapshotComplete(arLivePlayer,frame);
                            ((TextureViewRenderer) renderView).removeFrameListener(snapListener);
                        }
                    });

                }else if (renderView instanceof SurfaceViewRenderer){
                    arLivePlayEvent.onSnapshotComplete(arLivePlayer,frame);
                    applicationHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            arLivePlayEvent.onSnapshotComplete(arLivePlayer,frame);
                            ((SurfaceViewRenderer) renderView).removeFrameListener(snapListener);
                        }
                    });

                }

            }
        }
    };

}
