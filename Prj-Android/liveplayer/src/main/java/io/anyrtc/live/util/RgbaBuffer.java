package io.anyrtc.live.util;

import org.webrtc.VideoFrame;

import java.nio.ByteBuffer;

//可用于自定义渲染
public class RgbaBuffer implements VideoFrame.Buffer {
    private final ByteBuffer mBuffer;
    private int mWidth;
    private int mHeight;
    private final Runnable releaseCallback;
    private final Object refCountLock = new Object();
    private int refCount;

    public RgbaBuffer(ByteBuffer buffer, int width, int height, Runnable releaseCallback) {
        this.mBuffer = buffer;
        this.mWidth = width;
        this.mHeight = height;
        this.releaseCallback = releaseCallback;
    }

    public ByteBuffer getBuffer() {
        return this.mBuffer;
    }

    public int getWidth() {
        return this.mWidth;
    }

    public int getHeight() {
        return this.mHeight;
    }

    public VideoFrame.I420Buffer toI420() {
        return null;
    }

    public void retain() {
        synchronized(this.refCountLock) {
            ++this.refCount;
        }
    }

    public void release() {
        synchronized(this.refCountLock) {
            if (--this.refCount == 0 && this.releaseCallback != null) {
                this.releaseCallback.run();
            }

        }
    }

    public VideoFrame.Buffer cropAndScale(int cropX, int cropY, int cropWidth, int cropHeight, int scaleWidth, int scaleHeight) {
        return null;
    }
}

