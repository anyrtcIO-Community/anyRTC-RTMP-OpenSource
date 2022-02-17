package io.anyrtc.live.internal;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Point;
import android.media.projection.MediaProjection;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import org.webrtc.Camera1Enumerator;
import org.webrtc.Camera2Capturer;
import org.webrtc.Camera2Enumerator;
import org.webrtc.CameraEnumerator;
import org.webrtc.CameraVideoCapturer;
import org.webrtc.CapturerObserver;
import org.webrtc.ContextUtils;
import org.webrtc.EglBase;
import org.webrtc.Logging;
import org.webrtc.ScreenCapturerAndroid;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.ThreadUtils;
import org.webrtc.VideoCapturer;
import org.webrtc.effector.RTCVideoEffector;
import org.webrtc.effector.filter.GPUImageBeautyFilter;

import java.util.List;

import io.anyrtc.live.AndroidUtilities;
import io.anyrtc.live.ArScreenService;
import io.anyrtc.live.Instance;


public class VideoCapturerDevice  {

    private int CAPTURE_WIDTH = Build.VERSION.SDK_INT <= 19 ? 480 : 1280;
    private int CAPTURE_HEIGHT = Build.VERSION.SDK_INT <= 19 ? 640 : 720;
    private int CAPTURE_FPS = 25;

    public static EglBase eglBase;

    public static Intent mediaProjectionPermissionResultData;

    private VideoCapturer videoCapturer;
    public static SurfaceTextureHelper videoCapturerSurfaceTextureHelper;

    private HandlerThread thread;
    private Handler handler;

    private long nativePtr;

    private CapturerObserver nativeCapturerObserver;
    private RTCVideoEffector videoEffector;
    private  GPUImageBeautyFilter filter;

    // by liuxiaozhong 相机被杀后恢复
    private static final long CAMERA_OPEN_REQUEST_INTERVAL = 2000L;

    public VideoCapturerDevice(boolean isScreenShare) {
        if (Build.VERSION.SDK_INT < 18) {
            return;
        }
        AndroidUtilities.runOnUIThread(() -> {
            if (eglBase == null) {
                eglBase = EglBase.create(null, EglBase.CONFIG_PLAIN);
            }
            thread = new HandlerThread("LiveThread");
            thread.start();
            handler = new Handler(thread.getLooper());

        });
        videoEffector = new RTCVideoEffector();
        filter = new GPUImageBeautyFilter();
        videoEffector.addGPUImageFilter(filter);
    }

    public void checkScreenCapturerSize() {
        Point size = getScreenCaptureSize();
        if (CAPTURE_WIDTH != size.x || CAPTURE_HEIGHT != size.y) {
            CAPTURE_WIDTH = size.x;
            CAPTURE_HEIGHT = size.y;
            handler.post(() -> {
                if (videoCapturer != null) {
                    videoCapturer.changeCaptureFormat(size.x, size.y, 25);
                }
            });
        }
    }

    private Point getScreenCaptureSize() {
        WindowManager wm = (WindowManager) ContextUtils.getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
        Display display = wm.getDefaultDisplay();
        Point size = new Point();
        display.getRealSize(size);

        float aspect;
        if (size.x > size.y) {
            aspect = size.y / (float) size.x;
        } else {
            aspect = size.x / (float) size.y;
        }
        int dx = -1;
        int dy = -1;
        for (int a = 1; a <= 100; a++) {
            float val = a * aspect;
            if (val == (int) val) {
                if (size.x > size.y) {
                    dx = a;
                    dy = (int) (a * aspect);
                } else {
                    dy = a;
                    dx = (int) (a * aspect);
                }
                break;
            }
        }
        if (dx != -1 && aspect != 1) {
            while (size.x > 1000 || size.y > 1000 || size.x % 4 != 0 || size.y % 4 != 0) {
                size.x -= dx;
                size.y -= dy;
                if (size.x < 800 && size.y < 800) {
                    dx = -1;
                    break;
                }
            }
        }
        if (dx == -1 || aspect == 1) {
            float scale = Math.max(size.x / 970.0f, size.y / 970.0f);
            size.x = (int) Math.ceil((size.x / scale) / 4.0f) * 4;
            size.y = (int) Math.ceil((size.y / scale) / 4.0f) * 4;
        }
        return size;
    }


    private void init(long ptr, String deviceName, int width, int height, int fps) {
        if (Build.VERSION.SDK_INT < 18) {
            return;
        }
        CAPTURE_HEIGHT = height;
        CAPTURE_WIDTH = width;
        CAPTURE_FPS = fps;
        AndroidUtilities.runOnUIThread(() -> {
            if (eglBase == null) {
                return;
            }
            nativePtr = ptr;
            if ("screen".equals(deviceName)) {
                if (Build.VERSION.SDK_INT < 21) {
                    return;
                }
                if (videoCapturer == null) {
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                        videoCapturer = new ScreenCapturerAndroid(mediaProjectionPermissionResultData, new MediaProjection.Callback() {
                            @Override
                            public void onStop() {
                                AndroidUtilities.runOnUIThread(() -> {

                                });
                            }
                        });
                    }
                    Point size = getScreenCaptureSize();
                    CAPTURE_WIDTH = size.x;
                    CAPTURE_HEIGHT = size.y;
                    videoCapturerSurfaceTextureHelper = SurfaceTextureHelper.create("ScreenCapturerThread", eglBase.getEglBaseContext());
                    handler.post(() -> {
                        if (videoCapturerSurfaceTextureHelper == null || nativePtr == 0) {
                            return;
                        }
                        nativeCapturerObserver = nativeGetJavaVideoCapturerObserver(nativePtr);
                        videoCapturer.initialize(videoCapturerSurfaceTextureHelper, ContextUtils.getApplicationContext(), nativeCapturerObserver);
                        videoCapturer.startCapture(size.x, size.y, CAPTURE_FPS);
                    });
                }

            } else {
                CameraEnumerator enumerator = new Camera1Enumerator(false);
                //CameraEnumerator enumerator = Camera2Enumerator.isSupported(ContextUtils.getApplicationContext()) ? new Camera2Enumerator(ContextUtils.getApplicationContext()) : new Camera1Enumerator();
                int index = -1;
                String[] names = enumerator.getDeviceNames();
                for (int a = 0; a < names.length; a++) {
                    boolean isFrontFace = enumerator.isFrontFacing(names[a]);
                    if (isFrontFace == "front".equals(deviceName)) {
                        index = a;
                        break;
                    }
                }
                if (index == -1) {
                    return;
                }
                String cameraName = names[index];
                if (videoCapturer == null) {

                    videoCapturer = enumerator.createCapturer(cameraName, new CameraVideoCapturer.CameraEventsHandler() {
                        @Override
                        public void onCameraError(String errorDescription) {
                        }

                        @Override
                        public void onCameraDisconnected() {

                        }

                        @Override
                        public void onCameraFreezed(String errorDescription) {
                        }

                        @Override
                        public void onCameraOpening(String cameraName) {
                        }

                        @Override
                        public void onFirstFrameAvailable() {
                            AndroidUtilities.runOnUIThread(() -> {
                            });
                        }

                        @Override
                        public void onCameraClosed() {
                        }
                    });

                    videoCapturerSurfaceTextureHelper = SurfaceTextureHelper.create("VideoCapturerThread", eglBase.getEglBaseContext());
                    handler.post(() -> {
                        if (videoCapturerSurfaceTextureHelper == null) {
                            return;
                        }
                        nativeCapturerObserver = nativeGetJavaVideoCapturerObserver(nativePtr);
                        CapturerObserverProxy observerProxy = new CapturerObserverProxy(videoCapturerSurfaceTextureHelper, nativeCapturerObserver, videoEffector);
                        videoCapturer.initialize(videoCapturerSurfaceTextureHelper, ContextUtils.getApplicationContext(), observerProxy);
                    });
                } else {
                    handler.post(() -> ((CameraVideoCapturer) videoCapturer).switchCamera(new CameraVideoCapturer.CameraSwitchHandler() {


                        @Override
                        public void onCameraSwitchDone(boolean isFrontCamera) {
                            AndroidUtilities.runOnUIThread(() -> {
                                if (ArLiveEngineImpl.getInstance().getPusher() != null) {
                                   ArLiveEngineImpl.getInstance().getPusher().setSwitchCamera(false, isFrontCamera);
                                }
                            });
                        }

                        @Override
                        public void onCameraSwitchError(String errorDescription) {

                        }
                    }, cameraName));
                }
            }


        });
    }


    private void onStateChanged(long ptr, int state) {
        if (Build.VERSION.SDK_INT < 18) {
            return;
        }
        AndroidUtilities.runOnUIThread(() -> {
            if (nativePtr != ptr) {
                return;
            }
            handler.post(() -> {
                if (videoCapturer == null) {
                    return;
                }
                if (state == Instance.VIDEO_STATE_ACTIVE) {
                    videoCapturer.startCapture(CAPTURE_WIDTH, CAPTURE_HEIGHT, CAPTURE_FPS);
                } else {
                    try {
                        videoCapturer.stopCapture();
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
        });
    }

    private void needBeautyEffect(boolean need){
        if (need){
            videoEffector.enable();
        }else {
            videoEffector.disable();
        }
    }

    private void setWhitenessLevel(float level){
        if (filter!=null){
            filter.setBrightLevel(level);
        }
    }

    private void setBeautyLevel(float level){
        if (filter!=null){
            filter.setBeautyLevel(level);
        }
    }

    private void setToneLevel(float level){
        if (filter!=null){
            filter.setToneLevel(level);
        }
    }

    private void recoverCamera(){
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if(isForeground()){
                    onStateChanged(nativePtr,Instance.VIDEO_STATE_ACTIVE);
                }else {
                    handler.postDelayed(this,CAMERA_OPEN_REQUEST_INTERVAL);
                }
            }
        },CAMERA_OPEN_REQUEST_INTERVAL);
    }


    private EglBase.Context getSharedEGLContext() {
        if (eglBase == null) {
            eglBase = EglBase.create(null, EglBase.CONFIG_PLAIN);
        }
        return eglBase != null ? eglBase.getEglBaseContext() : null;
    }

    public static EglBase getEglBase() {
        if (eglBase == null) {
            eglBase = EglBase.create(null, EglBase.CONFIG_PLAIN);
        }
        return eglBase;
    }

    private void onDestroy() {
        if (nativePtr != 0) {
            nativePtr = 0;
            if (Build.VERSION.SDK_INT < 18) {
                return;
            }
            AndroidUtilities.runOnUIThread(() -> {
                handler.post(() -> {
                    if (videoCapturer != null) {
                        try {
                            videoCapturer.stopCapture();
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                        if (videoCapturer instanceof ScreenCapturerAndroid) {
                            ArScreenService screenService = ArScreenService.getSharedInstance();
                            if (screenService != null) {
                                screenService.stopScreen();
                            }
                        }
                        videoCapturer.dispose();
                        videoCapturer = null;
                    }
                    if (videoCapturerSurfaceTextureHelper != null) {
                        videoCapturerSurfaceTextureHelper.dispose();
                        videoCapturerSurfaceTextureHelper = null;
                    }
                });
                try {
                    thread.quitSafely();
                } catch (Exception e) {
                }
            });
        }

    }

    private static native CapturerObserver nativeGetJavaVideoCapturerObserver(long ptr);


    private boolean isFrontCamera() {
        return false;
    }

    private int switchCamera() {
        return 0;
    }

    private float getCameraZoomMaxRatio() {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).getCameraZoomMaxRatio();
        }
        return 0f;
    }

    private int setCameraZoomRatio(float var1) {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).setZoomRatio(var1);
        }
        return 0;
    }

    private boolean isAutoFocusEnabled() {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).isAutoFaceFocusEnabled();
        }
        return false;
    }

    public int enableCameraAutoFocus(boolean var1) {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).enableCameraAutoFocus(var1);
        }
        return -1;
    }

    private int setCameraFocusPosition(float var1, float var2) {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).setFocusPosition(var1,var2);
        }
        return -1;
    }

    private boolean enableCameraTorch(boolean var1) {
        if (videoCapturer instanceof CameraVideoCapturer){
            return ((CameraVideoCapturer) videoCapturer).enableCameraTorch(var1);
        }
        return false;
    }


    private void setCameraCapturerParam(int width,int height,int fps) {
        if (videoCapturer instanceof CameraVideoCapturer){
            ((CameraVideoCapturer) videoCapturer).changeCaptureFormat(width,height,fps);
        }
    }
    private boolean isForeground() {
            ActivityManager activityManager = (ActivityManager) ContextUtils.getApplicationContext().getSystemService("activity");
            List<ActivityManager.RunningAppProcessInfo> processes = activityManager.getRunningAppProcesses();
            if (processes == null) {
                Logging.e("CAMERA1", "List of RunningAppProcessInfo is null");
                return false;
            }

            for (int i = 0; i < processes.size(); ++i) {
                ActivityManager.RunningAppProcessInfo processInfo = (ActivityManager.RunningAppProcessInfo) processes.get(i);
                if (processInfo == null) {
                    Logging.e("CAMERA1", "ActivityManager.RunningAppProcessInfo is null");
                } else if (processInfo.processName.equals(ContextUtils.getApplicationContext().getPackageName()) && processInfo.importance == 100) {
                    return true;
                }
            }
        return false;
    }

}
