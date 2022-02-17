/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.SystemClock;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.TimeUnit;
import org.webrtc.CameraEnumerationAndroid.CaptureFormat;

@SuppressWarnings("deprecation")
class Camera1Session implements CameraSession {
  private static final String TAG = "Camera1Session";
  private static final int NUMBER_OF_CAPTURE_BUFFERS = 3;

  private static final Histogram camera1StartTimeMsHistogram =
      Histogram.createCounts("WebRTC.Android.Camera1.StartTimeMs", 1, 10000, 50);
  private static final Histogram camera1StopTimeMsHistogram =
      Histogram.createCounts("WebRTC.Android.Camera1.StopTimeMs", 1, 10000, 50);
  private static final Histogram camera1ResolutionHistogram = Histogram.createEnumeration(
      "WebRTC.Android.Camera1.Resolution", CameraEnumerationAndroid.COMMON_RESOLUTIONS.size());

  private static enum SessionState { RUNNING, STOPPED }

  private final Handler cameraThreadHandler;
  private final Events events;
  private final boolean captureToTexture;
  private final Context applicationContext;
  private final SurfaceTextureHelper surfaceTextureHelper;
  private final int cameraId;
  private final android.hardware.Camera camera;
  private final android.hardware.Camera.CameraInfo info;
  private final CaptureFormat captureFormat;
  // Used only for stats. Only used on the camera thread.
  private final long constructionTimeNs; // Construction time of this class.

  private SessionState state;
  private boolean firstFrameReported;
  private boolean mIsAutoFaceFocusEnabled = false;
  private boolean isFaceDetectionStarted = false;
  private boolean faceDetectEnabled = false;
  


  // TODO(titovartem) make correct fix during webrtc:9175
  @SuppressWarnings("ByteBufferBackingArray")
  public static void create(final CreateSessionCallback callback, final Events events,
      final boolean captureToTexture, final Context applicationContext,
      final SurfaceTextureHelper surfaceTextureHelper, final int cameraId, final int width,
      final int height, final int framerate) {
    final long constructionTimeNs = System.nanoTime();
    Logging.d(TAG, "Open camera " + cameraId);
    events.onCameraOpening();

    final android.hardware.Camera camera;
    try {
      camera = android.hardware.Camera.open(cameraId);
    } catch (RuntimeException e) {
      callback.onFailure(FailureType.ERROR, e.getMessage());
      return;
    }

    if (camera == null) {
      callback.onFailure(FailureType.ERROR,
          "android.hardware.Camera.open returned null for camera id = " + cameraId);
      return;
    }

    try {
      camera.setPreviewTexture(surfaceTextureHelper.getSurfaceTexture());
    } catch (IOException | RuntimeException e) {
      camera.release();
      callback.onFailure(FailureType.ERROR, e.getMessage());
      return;
    }

    final android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
    android.hardware.Camera.getCameraInfo(cameraId, info);

    final CaptureFormat captureFormat;
    try {
      final android.hardware.Camera.Parameters parameters = camera.getParameters();
      captureFormat = findClosestCaptureFormat(parameters, width, height, framerate);
      final Size pictureSize = findClosestPictureSize(parameters, width, height);
      updateCameraParameters(camera, parameters, captureFormat, pictureSize, captureToTexture);
    } catch (RuntimeException e) {
      camera.release();
      callback.onFailure(FailureType.ERROR, e.getMessage());
      return;
    }

    if (!captureToTexture) {
      final int frameSize = captureFormat.frameSize();
      for (int i = 0; i < NUMBER_OF_CAPTURE_BUFFERS; ++i) {
        final ByteBuffer buffer = ByteBuffer.allocateDirect(frameSize);
        camera.addCallbackBuffer(buffer.array());
      }
    }

    // Calculate orientation manually and send it as CVO insted.
    camera.setDisplayOrientation(0 /* degrees */);

    callback.onDone(new Camera1Session(events, captureToTexture, applicationContext,
        surfaceTextureHelper, cameraId, camera, info, captureFormat, constructionTimeNs));
  }

  private static void updateCameraParameters(android.hardware.Camera camera,
      android.hardware.Camera.Parameters parameters, CaptureFormat captureFormat, Size pictureSize,
      boolean captureToTexture) {
    final List<String> focusModes = parameters.getSupportedFocusModes();

    parameters.setPreviewFpsRange(captureFormat.framerate.min, captureFormat.framerate.max);
    parameters.setPreviewSize(captureFormat.width, captureFormat.height);
    parameters.setPictureSize(pictureSize.width, pictureSize.height);
    if (!captureToTexture) {
      parameters.setPreviewFormat(captureFormat.imageFormat);
    }

    if (parameters.isVideoStabilizationSupported()) {
      parameters.setVideoStabilization(true);
    }
    if (focusModes.contains(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
      parameters.setFocusMode(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
    }
    camera.setParameters(parameters);
  }

  private static CaptureFormat findClosestCaptureFormat(
      android.hardware.Camera.Parameters parameters, int width, int height, int framerate) {
    // Find closest supported format for |width| x |height| @ |framerate|.
    final List<CaptureFormat.FramerateRange> supportedFramerates =
        Camera1Enumerator.convertFramerates(parameters.getSupportedPreviewFpsRange());
    Logging.d(TAG, "Available fps ranges: " + supportedFramerates);

    final CaptureFormat.FramerateRange fpsRange =
        CameraEnumerationAndroid.getClosestSupportedFramerateRange(supportedFramerates, framerate);

    final Size previewSize = CameraEnumerationAndroid.getClosestSupportedSize(
        Camera1Enumerator.convertSizes(parameters.getSupportedPreviewSizes()), width, height);
    CameraEnumerationAndroid.reportCameraResolution(camera1ResolutionHistogram, previewSize);

    return new CaptureFormat(previewSize.width, previewSize.height, fpsRange);
  }

  private static Size findClosestPictureSize(
      android.hardware.Camera.Parameters parameters, int width, int height) {
    return CameraEnumerationAndroid.getClosestSupportedSize(
        Camera1Enumerator.convertSizes(parameters.getSupportedPictureSizes()), width, height);
  }

  private Camera1Session(Events events, boolean captureToTexture, Context applicationContext,
      SurfaceTextureHelper surfaceTextureHelper, int cameraId, android.hardware.Camera camera,
      android.hardware.Camera.CameraInfo info, CaptureFormat captureFormat,
      long constructionTimeNs) {
    Logging.d(TAG, "Create new camera1 session on camera " + cameraId);

    this.cameraThreadHandler = new Handler();
    this.events = events;
    this.captureToTexture = captureToTexture;
    this.applicationContext = applicationContext;
    this.surfaceTextureHelper = surfaceTextureHelper;
    this.cameraId = cameraId;
    this.camera = camera;
    this.info = info;
    this.captureFormat = captureFormat;
    this.constructionTimeNs = constructionTimeNs;

    surfaceTextureHelper.setTextureSize(captureFormat.width, captureFormat.height);

    startCapturing();
  }

  @Override
  public void stop() {
    Logging.d(TAG, "Stop camera1 session on camera " + cameraId);
    checkIsOnCameraThread();
    if (state != SessionState.STOPPED) {
      final long stopStartTime = System.nanoTime();
      stopInternal();
      final int stopTimeMs = (int) TimeUnit.NANOSECONDS.toMillis(System.nanoTime() - stopStartTime);
      camera1StopTimeMsHistogram.addSample(stopTimeMs);
    }
  }

  private void startCapturing() {
    Logging.d(TAG, "Start capturing");
    checkIsOnCameraThread();

    state = SessionState.RUNNING;
    camera.setErrorCallback(new android.hardware.Camera.ErrorCallback() {
      @Override
      public void onError(int error, android.hardware.Camera camera) {
        String errorMessage;
        if (error == android.hardware.Camera.CAMERA_ERROR_SERVER_DIED) {
          errorMessage = "Camera server died!";
        } else {
          errorMessage = "Camera error: " + error;
        }
        Logging.e(TAG, errorMessage);
        stopInternal();
        if (error == android.hardware.Camera.CAMERA_ERROR_EVICTED) {
          events.onCameraDisconnected(Camera1Session.this);
        } else {
          events.onCameraError(Camera1Session.this, errorMessage);
        }
        if (error == 2 || error == 100 || error == 1) {
          recoverCamera();
        }

      }
    });

    if (captureToTexture) {
      listenForTextureFrames();
    } else {
      listenForBytebufferFrames();
    }
    try {
      camera.startPreview();
    } catch (RuntimeException e) {
      stopInternal();
      events.onCameraError(this, e.getMessage());
    }
  }

  private void stopInternal() {
    Logging.d(TAG, "Stop internal");
    checkIsOnCameraThread();
    if (state == SessionState.STOPPED) {
      Logging.d(TAG, "Camera is already stopped");
      return;
    }

    state = SessionState.STOPPED;
    surfaceTextureHelper.stopListening();
    // Note: stopPreview or other driver code might deadlock. Deadlock in
    // android.hardware.Camera._stopPreview(Native Method) has been observed on
    // Nexus 5 (hammerhead), OS version LMY48I.
    camera.stopPreview();
    camera.release();
    events.onCameraClosed(this);
    Logging.d(TAG, "Stop done");
  }

  @Override
  public boolean enableCameraTorch(boolean enable) {
    if (cameraThreadHandler!=null){
      boolean torch = ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Callable<Boolean>() {
        @Override
        public Boolean call() throws Exception {
          boolean result = setTorchMode(enable);
          return result;
        }
      });
      return torch;
    }else {
      return false;
    }
  }

  @Override
  public float getCameraZoomMaxRatio() {
    if (cameraThreadHandler!=null){
      float zoom = ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Callable<Float>() {
        @Override
        public Float call() throws Exception {
          float result = getCameraZoomMaxRatio_I();
          return result;
        }
      });
      return zoom;
    }else {
      return 0f;
    }
  }

  private float getCameraZoomMaxRatio_I(){
    if (camera!=null){
      android.hardware.Camera.Parameters parameters = camera.getParameters();
      int maxZoom = 0;
      if (isZoomSupported(parameters)){
        maxZoom = parameters.getMaxZoom();
      }
      List<Integer> ratios = parameters.getZoomRatios();
      if (ratios!=null&&ratios.size()>maxZoom){
        return (ratios.get(maxZoom)/100f);
      }
    }
    return 0f;
  }

  private boolean isZoomSupported(Camera.Parameters parameters) {
    if (parameters != null) {
      boolean var2 = parameters.isZoomSupported();
      if (var2) {
        return true;
      } else {
        Logging.w(TAG, "camera zoom is not supported ");
        return false;
      }
    } else {
      return false;
    }
  }
  private boolean setTorchMode(boolean enable){
    if (camera !=null){
      android.hardware.Camera.Parameters parameters = camera.getParameters();
      if (parameters!=null){
        List list = parameters.getSupportedFlashModes();
        if (list!=null&&list.contains("torch")){
          if (enable){
            parameters.setFlashMode("torch");
          }else {
            parameters.setFlashMode("off");
          }
          camera.setParameters(parameters);
          return true;
        }
      }
      return false;
    }
    return false;
  }

  @Override
  public boolean isAutoFocusEnabled() {
    if (cameraThreadHandler!=null){
      boolean support = ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Callable<Boolean>() {
        @Override
        public Boolean call() throws Exception {
          boolean result = isAutoFaceFocusSupported();
          return result;
        }
      });
      return support;
    }else {
      return false;
    }
  }

  private boolean isAutoFaceFocusSupported() {
    if (this.camera != null) {
      Camera.Parameters var1 = this.camera.getParameters();
      if (var1 != null && var1.getMaxNumDetectedFaces() > 0 && var1.getMaxNumFocusAreas() > 0 && isSupported("auto", var1.getSupportedFocusModes())) {
        return true;
      }
    }
    return false;
  }

  @Override
  public int enableCameraAutoFocus(boolean enable) {
    Logging.d("CAMERA1", "enableCameraAutoFocus: " + enable);
    boolean var2 = this.mIsAutoFaceFocusEnabled != enable;
    this.mIsAutoFaceFocusEnabled = enable;
    if (this.isAutoFaceFocusSupported() && var2) {
      if (this.mIsAutoFaceFocusEnabled && !this.isFaceDetectionStarted) {
        this.startFaceDetection();
      } else if (!this.mIsAutoFaceFocusEnabled && this.isFaceDetectionStarted && !this.faceDetectEnabled) {
        this.stopFaceDetection();
      }
    }
    return 0;
  }

  @Override
  public int setZoomRatio(float zoom) {
    if (cameraThreadHandler!=null){
      int result = ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Callable<Integer>() {
        @Override
        public Integer call() throws Exception {
          return setZoom_l(zoom);
        }
      });
      return result;
    }else {
      return -1;
    }
  }


  @Override
  public int setFocusPosition(float x, float y) {
    if (cameraThreadHandler!=null){
      int result = ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Callable<Integer>() {
        @Override
        public Integer call() throws Exception {
          return setFocusPosition_I(x,y);
        }
      });
      return result;
    }else {
      return -1;
    }
  }

  private int setFocusPosition_I(float x,float y){
    if (mIsAutoFaceFocusEnabled){
      return -1;
    }
    Logging.d("CAMERA1", "setFocus called camera api1");
    if (this.camera == null) {
      return -1;
    } else if (!(x < 0.0f) && !(x > 1.0f) && !(y < 0.0f) && !(y > 1.0f)) {
      Rect var4 = calculateTapArea(x, y, 1.0f);
      Rect var5 = calculateTapArea(x, y, 1.5f);

      try {
        this.camera.cancelAutoFocus();
      } catch (RuntimeException var12) {
        Logging.w("CAMERA1", "Failed to cancle AutoFocus" + var12);
      }

      Camera.Parameters var6 = camera.getParameters();
      if (var6 == null) {
        return -1;
      } else {
        ArrayList var7;
        if (var6.getMaxNumFocusAreas() > 0) {
          var7 = new ArrayList();
          var7.add(new Camera.Area(var4, 800));
          var6.setFocusAreas(var7);
        } else {
          Logging.d("CAMERA1", "focus areas not supported");
        }

        if (var6.getMaxNumMeteringAreas() > 0) {
          var7 = new ArrayList();
          var7.add(new Camera.Area(var5, 800));
          var6.setMeteringAreas(var7);
        } else {
          Logging.d("CAMERA1", "metering areas not supported");
        }

        final String var13 = var6.getFocusMode();
        if (isSupported("macro", var6.getSupportedFocusModes())) {
          var6.setFocusMode("macro");
            this.camera.setParameters(var6);
        } else {
          Logging.d("focus", "FOCUS_MODE_MACRO is not supported");
        }

        try {
          this.camera.autoFocus(new Camera.AutoFocusCallback() {
            public void onAutoFocus(boolean success, Camera camera) {
              if (camera != null) {
                Camera.Parameters var3 = camera.getParameters();
                var3.setFocusMode(var13);
                  camera.setParameters(var3);
              }
            }
          });
        } catch (Exception var10) {
          Logging.w("CAMERA1", "mCamera.autoFocus Exception: " + var10);
          return -1;
        }
        return 0;
      }
    } else {
      Logging.e("CAMERA1", "set focus unreasonable inputs");
      return -1;
    }
  }

  private static Rect calculateTapArea(float x, float y, float coefficient) {
    float var3 = 300.0F;
    int var4 = Float.valueOf(var3 * coefficient).intValue();
    int var5 = (int)(x * 2000.0F - 1000.0F);
    int var6 = (int)(y * 2000.0F - 1000.0F);
    int var7 = var4 / 2;
    RectF var8 = new RectF((float)clamp(var5 - var7, -1000, 1000), (float)clamp(var6 - var7, -1000, 1000), (float)clamp(var5 + var7, -1000, 1000), (float)clamp(var6 + var7, -1000, 1000));
    return new Rect(Math.round(var8.left), Math.round(var8.top), Math.round(var8.right), Math.round(var8.bottom));
  }
  private static int clamp(int var0, int var1, int var2) {
    return Math.max(var1, Math.min(var2, var0));
  }
  private List<Integer> getZoomRatios() {
    if (this.camera != null) {
      Camera.Parameters var1 = this.camera.getParameters();
      if (this.isZoomSupported(var1)) {
        return var1.getZoomRatios();
      }
    }

    return null;
  }
  private int setZoom_l(float zoom) {
    if (zoom < 0.0f) {
      return -1;
    } else {
      int var2 = (int)(zoom * 100.0f + 0.5f);
      List var3 = this.getZoomRatios();
      if (var3 == null) {
        return -1;
      } else {
        int var4 = 0;
        int var6;
        for(int var5 = 0; var5 < var3.size(); ++var5) {
          var6 = (Integer)var3.get(var5);
          if (var2 <= var6) {
            var4 = var5;
            break;
          }
        }

        if (this.camera != null) {
          Camera.Parameters var9 = this.camera.getParameters();
          if (this.isZoomSupported(var9)) {
            var6 = var9.getMaxZoom();
            if (var4 > var6) {
              Logging.w("CAMERA1", "zoom value is larger than maxZoom value");
              return -1;
            }
            var9.setZoom(var4);
            try {
              this.camera.setParameters(var9);
            } catch (Exception var8) {
              Logging.w("CAMERA1", "setParameters failed, zoomLevel: " + var4 + ", " + var8);
            }
          }
        }

        return 0;
      }
    }
  }

  private void startFaceDetection(){
    if (cameraThreadHandler!=null){
      ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Runnable() {
        @Override
        public void run() {
          startFaceDetection_l();
        }
      });
    }
  }

  private void stopFaceDetection(){
    if (cameraThreadHandler!=null){
      ThreadUtils.invokeAtFrontUninterruptibly(cameraThreadHandler, new Runnable() {
        @Override
        public void run() {
          stopFaceDetection_l();
        }
      });
    }
  }

  private void startFaceDetection_l() {
    if (this.camera != null) {
      try {
        Logging.d("CAMERA1", "enable face detection");
        this.camera.startFaceDetection();
        this.isFaceDetectionStarted = true;
      } catch (Exception var2) {
        Logging.e("CAMERA1", "start face detection failed:" + var2);
        this.camera.stopFaceDetection();
        this.isFaceDetectionStarted = false;
      }

    }
  }

  private void stopFaceDetection_l() {
    if (this.camera != null) {
      Logging.d("CAMERA1", "disable face detection");
      this.camera.stopFaceDetection();
      this.isFaceDetectionStarted = false;
    }
  }


  private  boolean isSupported(String value, List<String> supported) {
    return supported == null ? false : supported.indexOf(value) >= 0;
  }

  private void listenForTextureFrames() {
    surfaceTextureHelper.startListening((VideoFrame frame) -> {
      checkIsOnCameraThread();

      if (state != SessionState.RUNNING) {
        Logging.d(TAG, "Texture frame captured but camera is no longer running.");
        return;
      }

      if (!firstFrameReported) {
        final int startTimeMs =
            (int) TimeUnit.NANOSECONDS.toMillis(System.nanoTime() - constructionTimeNs);
        camera1StartTimeMsHistogram.addSample(startTimeMs);
        firstFrameReported = true;
      }

      // Undo the mirror that the OS "helps" us with.
      // http://developer.android.com/reference/android/hardware/Camera.html#setDisplayOrientation(int)
      final VideoFrame modifiedFrame = new VideoFrame(
          CameraSession.createTextureBufferWithModifiedTransformMatrix(
              (TextureBufferImpl) frame.getBuffer(),
              /* mirror= */ info.facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT,
              /* rotation= */ 0),
          /* rotation= */ getFrameOrientation(), frame.getTimestampNs());
      events.onFrameCaptured(Camera1Session.this, modifiedFrame);
      modifiedFrame.release();
    });
  }

  private void listenForBytebufferFrames() {
    camera.setPreviewCallbackWithBuffer(new android.hardware.Camera.PreviewCallback() {
      @Override
      public void onPreviewFrame(final byte[] data, android.hardware.Camera callbackCamera) {
        checkIsOnCameraThread();

        if (callbackCamera != camera) {
          Logging.e(TAG, "Callback from a different camera. This should never happen.");
          return;
        }

        if (state != SessionState.RUNNING) {
          Logging.d(TAG, "Bytebuffer frame captured but camera is no longer running.");
          return;
        }

        final long captureTimeNs = TimeUnit.MILLISECONDS.toNanos(SystemClock.elapsedRealtime());

        if (!firstFrameReported) {
          final int startTimeMs =
              (int) TimeUnit.NANOSECONDS.toMillis(System.nanoTime() - constructionTimeNs);
          camera1StartTimeMsHistogram.addSample(startTimeMs);
          firstFrameReported = true;
        }

        VideoFrame.Buffer frameBuffer = new NV21Buffer(
            data, captureFormat.width, captureFormat.height, () -> cameraThreadHandler.post(() -> {
              if (state == SessionState.RUNNING) {
                camera.addCallbackBuffer(data);
              }
            }));
        final VideoFrame frame = new VideoFrame(frameBuffer, getFrameOrientation(), captureTimeNs);
        events.onFrameCaptured(Camera1Session.this, frame);
        frame.release();
      }
    });
  }

  private int getFrameOrientation() {
    int rotation = CameraSession.getDeviceOrientation(applicationContext);
    if (info.facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK) {
      rotation = 360 - rotation;
    }
    return (info.orientation + rotation) % 360;
  }

  private void checkIsOnCameraThread() {
    if (Thread.currentThread() != cameraThreadHandler.getLooper().getThread()) {
      throw new IllegalStateException("Wrong thread");
    }
  }

  private native void recoverCamera();
}
