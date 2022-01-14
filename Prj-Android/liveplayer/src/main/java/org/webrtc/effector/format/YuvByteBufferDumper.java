package org.webrtc.effector.format;

// Dump YUV formatted bytes from RGB texture

import android.opengl.GLES20;

import org.webrtc.JavaI420Buffer;
import org.webrtc.VideoFrame;

import java.nio.ByteBuffer;

public class YuvByteBufferDumper {

    public static final String TAG = YuvByteBufferDumper.class.getSimpleName();

    private int bufferId = -1;
    private LibYuvBridge libYuv = new LibYuvBridge();

    public YuvByteBufferDumper() {}

    public void init() {
        final int buffers[] = new int[1];
        GLES20.glGenFramebuffers(1, buffers, 0);
        bufferId = buffers[0];
    }

    // TODO: org.webrtc.YuvConverter.convert() がそのまま使えないか?
    public VideoFrame.I420Buffer dump(int lastTextureId, int width, int height,
                                      int strideY, int strideU, int strideV) {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, bufferId);
        GLES20.glFramebufferTexture2D(
                GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, lastTextureId, 0);

        final ByteBuffer rgbaBuffer = ByteBuffer.allocateDirect(width * height * 4);
        GLES20.glViewport(0, 0, width, height);
        GLES20.glReadPixels(0, 0, width, height,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                rgbaBuffer);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        rgbaBuffer.rewind();

        ByteBuffer dataYBuffer = ByteBuffer.allocateDirect(strideY * height);
        ByteBuffer dataUBuffer = ByteBuffer.allocateDirect(strideU * height);
        ByteBuffer dataVBuffer = ByteBuffer.allocateDirect(strideV * height);

        libYuv.rgbaToI420(
                rgbaBuffer,
                width, height,
                dataYBuffer, strideY,
                dataUBuffer, strideU,
                dataVBuffer, strideV);

        return JavaI420Buffer.wrap(
                width, height,
                dataYBuffer, strideY, dataUBuffer, strideU, dataVBuffer, strideV,
                null);
    }

    public void dispose() {
        GLES20.glDeleteFramebuffers(1, new int[]{bufferId}, 0);
    }

}
