package io.anyrtc.live;

public abstract class ArDeviceManager {

     public abstract boolean isFrontCamera();

     public abstract int switchCamera();

     public abstract float getCameraZoomMaxRatio();

     public abstract int setCameraZoomRatio(float var1);

     public abstract boolean isAutoFocusEnabled();

     public abstract int enableCameraAutoFocus(boolean var1);

     public abstract int setCameraFocusPosition(float var1, float var2);

     public abstract boolean enableCameraTorch(boolean var1);

     public abstract int setAudioRoute(ArAudioRoute var1);

     public abstract int setSystemVolumeType(ArSystemVolumeType var1);

    public abstract void setCameraCapturerParam(ArCameraCaptureParam var1);

    public static class ArCameraCaptureParam {
        public ArCameraCaptureMode mode;
        public int width,height;

        public ArCameraCaptureParam() {
            mode=ArCameraCaptureMode.ArCameraResolutionStrategyAuto;
            width = 720;
            height = 1280;
        }
    }

    public static enum ArCameraCaptureMode {
        ArCameraResolutionStrategyAuto,
        ArCameraResolutionStrategyPerformance,
        ArCameraResolutionStrategyHighQuality,
        ArCameraCaptureManual;

        private ArCameraCaptureMode() {
        }
    }

    public static enum ArAudioRoute {
        ArAudioRouteSpeakerphone,
        ArAudioRouteEarpiece;

        private ArAudioRoute() {
        }
    }

    public static enum ArSystemVolumeType {
        ArSystemVolumeTypeAuto,
        ArSystemVolumeTypeMedia,
        ArSystemVolumeTypeVOIP;

        private ArSystemVolumeType() {
        }
    }
}
