package io.anyrtc.live.internal;

import android.os.Handler;

import org.webrtc.CapturerObserver;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.ThreadUtils;
import org.webrtc.VideoFrame;
import org.webrtc.effector.RTCVideoEffector;

public class CapturerObserverProxy implements CapturerObserver {
    public static final String TAG = CapturerObserverProxy.class.getSimpleName();

    private CapturerObserver originalObserver;
    private RTCVideoEffector videoEffector;

    public CapturerObserverProxy(final SurfaceTextureHelper surfaceTextureHelper,
                                 CapturerObserver observer,
                                 RTCVideoEffector effector) {

        this.originalObserver = observer;
        this.videoEffector = effector;

        final Handler handler = surfaceTextureHelper.getHandler();
        ThreadUtils.invokeAtFrontUninterruptibly(handler, () ->
                videoEffector.init(surfaceTextureHelper)
        );
    }

    @Override
    public void onCapturerStarted(boolean success) {
        this.originalObserver.onCapturerStarted(success);
    }

    @Override
    public void onCapturerStopped() {
        this.originalObserver.onCapturerStopped();
    }

    @Override
    public void onFrameCaptured(VideoFrame frame) {
        if (this.videoEffector.needToProcessFrame()) {
            VideoFrame.I420Buffer originalI420Buffer = frame.getBuffer().toI420();
            VideoFrame.I420Buffer effectedI420Buffer =
                    this.videoEffector.processByteBufferFrame(
                            originalI420Buffer, frame.getRotation(), frame.getTimestampNs());

            VideoFrame effectedVideoFrame = new VideoFrame(
                    effectedI420Buffer, frame.getRotation(), frame.getTimestampNs());
            originalI420Buffer.release();
            this.originalObserver.onFrameCaptured(effectedVideoFrame);
        } else {
            this.originalObserver.onFrameCaptured(frame);
        }
    }
}
