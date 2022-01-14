package org.webrtc.effector;

import android.graphics.PointF;

import java.util.HashMap;
import java.util.Map;

public class VideoEffectorContext {

    public static class FrameInfo {

        private int originalWidth = 0;
        private int originalHeight = 0;
        private int width = 0;
        private int height = 0;
        private int rotation = 0;
        private long timestamp = 0;
        private boolean rotated = false;

        private boolean transformChanged = true;

        public FrameInfo() {}

        public void update(int width,
                           int height,
                           int rotation,
                           long timestamp) {

            if (this.width != width
                    || this.height != height
                    || this.rotation != rotation) {
                this.transformChanged = true;
            } else {
                this.transformChanged = false;
            }

            this.originalWidth = width;
            this.originalHeight = height;
            this.rotation  = rotation;
            this.timestamp = timestamp;

            if (rotation == 90 || rotation == 270) {
                this.width = this.originalHeight;
                this.height = this.originalWidth;
            } else {
                this.width = this.originalWidth;
                this.height = this.originalHeight;
            }

            this.rotated = (rotation != 0);
        }

        public boolean needToBeReAnalyzed() {
            return this.transformChanged;
        }

        public boolean isRotated() {
            return rotated;
        }

        public int getOrignalWidth() {
            return this.originalWidth;
        }

        public int getOriginalHeight() {
            return this.originalHeight;
        }

        public int getWidth() {
            // TODO fix to width
            return this.originalWidth;
        }

        public int getHeight() {
            // TODO fix to height
            return this.originalHeight;
        }

        public int getRotation() {
            return this.rotation;
        }

        public long getTimestamp() {
            return this.timestamp;
        }
    }

    private FrameInfo frameInfo = new FrameInfo();

    public VideoEffectorContext() {}

    public FrameInfo getFrameInfo() {
        return this.frameInfo;
    }

    public boolean needToReAnalyzeFrame() {
        if (this.frameInfo == null) {
            return false;
        } else {
            return this.frameInfo.needToBeReAnalyzed();
        }
    }

    void updateFrameInfo(int width,
                         int height,
                         int rotation,
                         long timestamp) {
        this.frameInfo.update(width, height, rotation, timestamp);
    }

}

