package io.anyrtc.live;

import org.webrtc.VideoFrame;
import org.webrtc.VideoSink;

public class ProxyVideoSink implements VideoSink {
    private VideoSink target;
    private VideoSink background;

    private long nativeInstance;

    @Override
    synchronized public void onFrame(VideoFrame frame) {
        if (target != null) {
            target.onFrame(frame);
        }
        if (background != null) {
            background.onFrame(frame);
        }
    }

    synchronized public void setTarget(VideoSink newTarget) {
        if (target != newTarget) {
            if (target != null) {
                target.setParentSink(null);
            }
            target = newTarget;
            if (target != null) {
                target.setParentSink(this);
            }
        }
    }

    synchronized public void setBackground(VideoSink newBackground) {
        if (background != null) {
            background.setParentSink(null);
        }
        background = newBackground;
        if (background != null) {
            background.setParentSink(this);
        }
    }

    synchronized public void removeTarget(VideoSink target) {
        if (this.target == target) {
            this.target = null;
        }
    }

    synchronized public void removeBackground(VideoSink background) {
        if (this.background == background) {
            this.background = null;
        }
    }

    synchronized public void swap() {
        if (target != null && background != null) {
            target = background;
            background = null;
        }
    }
}
