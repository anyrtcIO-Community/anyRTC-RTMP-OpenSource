package io.anyrtc.live.internal;

import android.content.Context;

import org.webrtc.voiceengine.WebRtcAudioManager;

import java.util.Set;

import io.anyrtc.live.ArDeviceManager;

public class ArDeviceManagerImpl extends ArDeviceManager {

    private NativeInstance nativeInstance;
    private boolean isFrontFaceCamera = true;
    private boolean switchingCamera;
    private ArAudioManager audioManager;

    public ArDeviceManagerImpl(NativeInstance nativeInstance, Context context) {
        this.nativeInstance = nativeInstance;
        audioManager = ArAudioManager.create(context);
        audioManager.start(new ArAudioManager.AudioManagerEvents() {
            @Override
            public void onAudioDeviceChanged(ArAudioManager.AudioDevice selectedAudioDevice, Set<ArAudioManager.AudioDevice> availableAudioDevices) {
            }
        });
    }



    protected void setCameraIndex(boolean isFrontFaceCamera){
        this.isFrontFaceCamera = isFrontFaceCamera;
    }

    protected void setSwitchingCamera(boolean switching, boolean isFrontFace){
        switchingCamera = switching;
        if (!switching){
            isFrontFaceCamera = isFrontFace;
        }
    }

    @Override
    public boolean isFrontCamera() {
        return isFrontFaceCamera;
    }

    @Override
    public int switchCamera() {
        if (!switchingCamera){
            switchingCamera = true;
            nativeInstance.switchCamera(!isFrontFaceCamera);
        }
        return 0;
    }

    @Override
    public float getCameraZoomMaxRatio() {
        return nativeInstance.getCameraZoomMaxRatio();
    }

    @Override
    public int setCameraZoomRatio(float var1) {
        return  nativeInstance.setCameraZoomRatio(var1);
    }

    @Override
    public boolean isAutoFocusEnabled() {
        return nativeInstance.isAutoFocusEnabled();
    }

    @Override
    public int enableCameraAutoFocus(boolean var1) {
        return nativeInstance.enableCameraAutoFocus(var1);
    }

    @Override
    public int setCameraFocusPosition(float var1, float var2) {
        return nativeInstance.setCameraFocusPosition(var1,var2);
    }

    @Override
    public boolean enableCameraTorch(boolean var1) {
        return nativeInstance.enableCameraTorch(var1);
    }

    @Override
    public int setAudioRoute(ArAudioRoute route) {
        if (audioManager!=null){
            audioManager.setDefaultAudioDevice((route == ArAudioRoute.ArAudioRouteEarpiece) ? ArAudioManager.AudioDevice.EARPIECE : ArAudioManager.AudioDevice.SPEAKER_PHONE);
        }
        return 0;
    }

    @Override
    public int setSystemVolumeType(ArSystemVolumeType volumeType) {
        switch (volumeType){
            case ArSystemVolumeTypeVOIP:
                if (audioManager!=null){
                    audioManager.setStreamMode(true);
                }
                break;
            case ArSystemVolumeTypeMedia:
                if (audioManager!=null){
                    audioManager.setStreamMode(false);
                }
                break;
            default:{
                if (audioManager!=null){
                    audioManager.setStreamMode(false);
                }
            }
        }
        return 0;
    }

    @Override
    public void setCameraCapturerParam(ArCameraCaptureParam var1) {
        nativeInstance.setCameraCapturerParam(var1.mode.ordinal(),var1.width,var1.height);
    }

    protected void setVoipMode(boolean voip){
        if (audioManager!=null){
            audioManager.setStreamMode(voip);
        }

    }

    public void release(){
        if(audioManager!=null){
            audioManager.stop();
            audioManager = null;
        }
    }


}
