/*
 *  Copyright 2015 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Build;
import android.os.SystemClock;
import android.util.Range;
import android.util.Size;

import org.webrtc.CameraEnumerationAndroid.CaptureFormat;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@TargetApi(21)
public class Camera2Enumerator {
  private final static String TAG = "Camera2Enumerator";
  private final static double NANO_SECONDS_PER_SECOND = 1.0e9;

  // Each entry contains the supported formats for a given camera index. The formats are enumerated
  // lazily in getSupportedFormats(), and cached for future reference.
  private static final Map<String, List<CaptureFormat>> cachedSupportedFormats =
      new HashMap<String, List<CaptureFormat>>();

  public static boolean isSupported() {
    return Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP;
  }

  public static List<CaptureFormat> getSupportedFormats(Context context, String cameraId) {
    return getSupportedFormats(
        (CameraManager) context.getSystemService(Context.CAMERA_SERVICE), cameraId);
  }

  public static List<CaptureFormat> getSupportedFormats(
      CameraManager cameraManager, String cameraId) {
    synchronized (cachedSupportedFormats) {
      if (cachedSupportedFormats.containsKey(cameraId)) {
        return cachedSupportedFormats.get(cameraId);
      }
      Logging.d(TAG, "Get supported formats for camera index " + cameraId + ".");
      final long startTimeMs = SystemClock.elapsedRealtime();

      final CameraCharacteristics cameraCharacteristics;
      try {
        cameraCharacteristics = cameraManager.getCameraCharacteristics(cameraId);
      } catch (Exception ex) {
        Logging.e(TAG, "getCameraCharacteristics(): " + ex);
        return new ArrayList<CaptureFormat>();
      }

      // Calculate default max fps from auto-exposure ranges in case getOutputMinFrameDuration() is
      // not supported.
      final Range<Integer>[] fpsRanges =
          cameraCharacteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
      int defaultMaxFps = 0;
      for (Range<Integer> fpsRange : fpsRanges) {
        defaultMaxFps = Math.max(defaultMaxFps, fpsRange.getUpper());
      }

      final StreamConfigurationMap streamMap =
          cameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
      final Size[] sizes = streamMap.getOutputSizes(ImageFormat.YUV_420_888);
      if (sizes == null) {
        throw new RuntimeException("ImageFormat.YUV_420_888 not supported.");
      }

      final List<CaptureFormat> formatList = new ArrayList<CaptureFormat>();
      for (Size size : sizes) {
        long minFrameDurationNs = 0;
        try {
          minFrameDurationNs = streamMap.getOutputMinFrameDuration(ImageFormat.YUV_420_888, size);
        } catch (Exception e) {
          // getOutputMinFrameDuration() is not supported on all devices. Ignore silently.
        }
        final int maxFps = (minFrameDurationNs == 0)
                               ? defaultMaxFps
                               : (int) Math.round(NANO_SECONDS_PER_SECOND / minFrameDurationNs);
        formatList.add(new CaptureFormat(size.getWidth(), size.getHeight(), 0, maxFps * 1000));
      }
      cachedSupportedFormats.put(cameraId, formatList);
      final long endTimeMs = SystemClock.elapsedRealtime();
      Logging.d(TAG, "Get supported formats for camera index " + cameraId + " done."
          + " Time spent: " + (endTimeMs - startTimeMs) + " ms.");
      return formatList;
    }
  }
}
