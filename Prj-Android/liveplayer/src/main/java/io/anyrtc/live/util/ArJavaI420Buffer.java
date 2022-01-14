package io.anyrtc.live.util;


import org.webrtc.JavaI420Buffer;
import org.webrtc.VideoFrame;

import java.nio.ByteBuffer;

/**
 * 可用于自定义渲染
 */
public class ArJavaI420Buffer implements VideoFrame.I420Buffer {
    private final int width;
    private final int height;
    private final ByteBuffer dataY;
    private final ByteBuffer dataU;
    private final ByteBuffer dataV;
    private final int strideY;
    private final int strideU;
    private final int strideV;
    private final Runnable releaseCallback;
    private final Object refCountLock = new Object();
    private int refCount;

    private ArJavaI420Buffer(int var1, int var2, ByteBuffer var3, int var4, ByteBuffer var5, int var6, ByteBuffer var7, int var8, Runnable var9) {
        this.width = var1;
        this.height = var2;
        this.dataY = var3;
        this.dataU = var5;
        this.dataV = var7;
        this.strideY = var4;
        this.strideU = var6;
        this.strideV = var8;
        this.releaseCallback = var9;
        this.refCount = 1;
    }

    public static ArJavaI420Buffer wrap(int var0, int var1, ByteBuffer var2, int var3, ByteBuffer var4, int var5, ByteBuffer var6, int var7, Runnable var8) {
        if (var2 != null && var4 != null && var6 != null) {
            if (var2.isDirect() && var4.isDirect() && var6.isDirect()) {
                var2 = var2.slice();
                var4 = var4.slice();
                var6 = var6.slice();
                int var9 = (var1 + 1) / 2;
                int var10 = var3 * var1;
                int var11 = var5 * var9;
                int var12 = var7 * var9;
                if (var2.capacity() < var10) {
                    throw new IllegalArgumentException("Y-buffer must be at least " + var10 + " bytes.");
                } else if (var4.capacity() < var11) {
                    throw new IllegalArgumentException("U-buffer must be at least " + var11 + " bytes.");
                } else if (var6.capacity() < var12) {
                    throw new IllegalArgumentException("V-buffer must be at least " + var12 + " bytes.");
                } else {
                    return new ArJavaI420Buffer(var0, var1, var2, var3, var4, var5, var6, var7, var8);
                }
            } else {
                throw new IllegalArgumentException("Data buffers must be direct byte buffers.");
            }
        } else {
            throw new IllegalArgumentException("Data buffers cannot be null.");
        }
    }

    public static ArJavaI420Buffer createYUV(byte[] var0, int var1, int var2) {
        if (var0 != null && var0.length != 0) {
            ArJavaI420Buffer var3 = allocate(var1, var2);
            ByteBuffer var4 = var3.getDataY();
            ByteBuffer var5 = var3.getDataU();
            ByteBuffer var6 = var3.getDataV();
            int var7 = (var2 + 1) / 2;
            int var8 = var2 * var3.getStrideY();
            int var9 = var7 * var3.getStrideU();
            int var10 = var7 * var3.getStrideV();
            var4.put(var0, 0, var8);
            var5.put(var0, var8, var9);
            var6.put(var0, var8 + var9, var10);
            return var3;
        } else {
            return null;
        }
    }

    public static ArJavaI420Buffer allocate(int var0, int var1) {
        int var2 = (var1 + 1) / 2;
        int var3 = (var0 + 1) / 2;
        byte var4 = 0;
        int var5 = var4 + var0 * var1;
        int var6 = var5 + var3 * var2;
        ByteBuffer var7 = ByteBuffer.allocateDirect(var0 * var1 + 2 * var3 * var2);
        var7.position(var4);
        var7.limit(var5);
        ByteBuffer var8 = var7.slice();
        var7.position(var5);
        var7.limit(var6);
        ByteBuffer var9 = var7.slice();
        var7.position(var6);
        var7.limit(var6 + var3 * var2);
        ByteBuffer var10 = var7.slice();
        return new ArJavaI420Buffer(var0, var1, var8, var0, var9, var3, var10, var3, (Runnable)null);
    }

    public int getWidth() {
        return this.width;
    }

    public int getHeight() {
        return this.height;
    }

    public ByteBuffer getDataY() {
        return this.dataY.slice();
    }

    public ByteBuffer getDataU() {
        return this.dataU.slice();
    }

    public ByteBuffer getDataV() {
        return this.dataV.slice();
    }

    public int getStrideY() {
        return this.strideY;
    }

    public int getStrideU() {
        return this.strideU;
    }

    public int getStrideV() {
        return this.strideV;
    }

    public VideoFrame.I420Buffer toI420() {
        this.retain();
        return this;
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

    public VideoFrame.Buffer cropAndScale(int var1, int var2, int var3, int var4, int var5, int var6) {
        return JavaI420Buffer.cropAndScaleI420(this, var1, var2, var3, var4, var5, var6);
    }
}
