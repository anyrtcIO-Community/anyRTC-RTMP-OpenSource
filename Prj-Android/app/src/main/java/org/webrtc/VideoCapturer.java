/*
 *  Copyright 2013 The WebRTC@AnyRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import android.content.Context;

import java.util.List;

// Base interface for all VideoCapturers to implement.
public interface VideoCapturer {
  // Interface used for providing callbacks to an observer.
  public interface CapturerObserver {
    // Notify if the camera have been started successfully or not.
    // Called on a Java thread owned by VideoCapturer.
    void onCapturerStarted(boolean success);

    // Delivers a captured frame. Called on a Java thread owned by VideoCapturer.
    void onByteBufferFrameCaptured(byte[] data, int width, int height, int rotation,
                                   long timeStamp);

    // Delivers a captured frame in a texture with id |oesTextureId|. Called on a Java thread
    // owned by VideoCapturer.
    void onTextureFrameCaptured(
            int width, int height, int oesTextureId, float[] transformMatrix, int rotation,
            long timestamp);

    // Requests an output format from the video capturer. Captured frames
    // by the camera will be scaled/or dropped by the video capturer.
    // Called on a Java thread owned by VideoCapturer.
    void onOutputFormatRequest(int width, int height, int framerate);
  }

  // An implementation of CapturerObserver that forwards all calls from
  // Java to the C layer.
  static class NativeObserver implements CapturerObserver {
    private final long nativeCapturer;

    public NativeObserver(long nativeCapturer) {
      this.nativeCapturer = nativeCapturer;
    }

    @Override
    public void onCapturerStarted(boolean success) {
      nativeCapturerStarted(nativeCapturer, success);
    }

    @Override
    public void onByteBufferFrameCaptured(byte[] data, int width, int height,
        int rotation, long timeStamp) {
      nativeOnByteBufferFrameCaptured(nativeCapturer, data, data.length, width, height, rotation,
          timeStamp);
    }

    @Override
    public void onTextureFrameCaptured(
        int width, int height, int oesTextureId, float[] transformMatrix, int rotation,
        long timestamp) {
      nativeOnTextureFrameCaptured(nativeCapturer, width, height, oesTextureId, transformMatrix,
          rotation, timestamp);
    }

    @Override
    public void onOutputFormatRequest(int width, int height, int framerate) {
      nativeOnOutputFormatRequest(nativeCapturer, width, height, framerate);
    }

    private native void nativeCapturerStarted(long nativeCapturer,
        boolean success);
    private native void nativeOnByteBufferFrameCaptured(long nativeCapturer,
        byte[] data, int length, int width, int height, int rotation, long timeStamp);
    private native void nativeOnTextureFrameCaptured(long nativeCapturer, int width, int height,
        int oesTextureId, float[] transformMatrix, int rotation, long timestamp);
    private native void nativeOnOutputFormatRequest(long nativeCapturer,
        int width, int height, int framerate);
  }

  /**
   * Returns a list with all the formats this VideoCapturer supports.
   */
  List<CameraEnumerationAndroid.CaptureFormat> getSupportedFormats();

  /**
   * Start capturing frames in a format that is as close as possible to |width| x |height| and
   * |framerate|. If the VideoCapturer wants to deliver texture frames, it should do this by
   * rendering on the SurfaceTexture in |surfaceTextureHelper|, register itself as a listener,
   * and forward the texture frames to CapturerObserver.onTextureFrameCaptured().
   */
  void startCapture(
          int width, int height, int framerate, SurfaceTextureHelper surfaceTextureHelper,
          Context applicationContext, CapturerObserver frameObserver);

  /**
   * Stop capturing. This function should block until capture is actually stopped.
   */
  void stopCapture() throws InterruptedException;

  void onOutputFormatRequest(int width, int height, int framerate);

  void changeCaptureFormat(int width, int height, int framerate);

  /**
   * Perform any final cleanup here. No more capturing will be done after this call.
   */
  void dispose();
}
