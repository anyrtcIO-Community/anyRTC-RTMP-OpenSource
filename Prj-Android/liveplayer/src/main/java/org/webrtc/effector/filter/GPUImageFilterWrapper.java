package org.webrtc.effector.filter;

import android.opengl.GLES20;

import org.webrtc.GlUtil;
import org.webrtc.effector.VideoEffectorContext;

import java.nio.FloatBuffer;


public class GPUImageFilterWrapper extends FrameImageFilter {

    public interface Listener {
        void onInit(GPUImageFilter filter);
        void onUpdate(GPUImageFilter filter, VideoEffectorContext vctx);
    }

    private static final FloatBuffer FULL_RECTANGLE_BUF = GlUtil.createFloatBuffer(new float[] {
            -1.0f, -1.0f, // Bottom left.
            1.0f, -1.0f, // Bottom right.
            -1.0f, 1.0f, // Top left.
            1.0f, 1.0f, // Top right.
    });

    private static final FloatBuffer FULL_RECTANGLE_TEX_BUF = GlUtil.createFloatBuffer(new float[] {
            0.0f, 0.0f, // Bottom left.
            1.0f, 0.0f, // Bottom right.
            0.0f, 1.0f, // Top left.
            1.0f, 1.0f // Top right.
    });

    private GPUImageFilter originalFilter;
    private Listener listener;

    public GPUImageFilterWrapper(GPUImageFilter filter) {
        this(filter, null);
    }
    public GPUImageFilterWrapper(GPUImageFilter filter, Listener listener) {
        this.originalFilter = filter;
        this.listener = listener;
    }

    private int textureId = -1;
    private int bufferId = -1;

    private int width = 0;
    private int height = 0;

    @Override
    public void init() {
        this.originalFilter.ifNeedInit();

        textureId = GlUtil.generateTexture(GLES20.GL_TEXTURE_2D);

        final int buffers[] = new int[1];
        GLES20.glGenFramebuffers(1, buffers, 0);
        bufferId = buffers[0];

        if (this.listener != null) {
            this.listener.onInit(originalFilter);
        }
    }

    private void resizeTextureIfNeeded(int width, int height) {
        if (width == 0 || height == 0) {
            throw new IllegalArgumentException("invalid size of texture");
        }
        if (this.width == width && this.height == height) {
            // not changed,  do nothing
            return;
        }
        this.width  = width;
        this.height = height;
        resetTexture(width, height);
        attachTextureToFramebuffer();
        GlUtil.checkNoGLES2Error("GPUImageFilterWrapper.resizeTextureIfNeeded");
    }

    private void attachTextureToFramebuffer() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, bufferId);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D,
                textureId, 0);
        final int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
        if (status != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
            throw new IllegalStateException("Framebuffer not complete, status: " + status);
        }
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
    }

    private void resetTexture(int width, int height) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA,
                width, height, 0,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
    }

    @Override
    public int filter(VideoEffectorContext ctx, int srcTextureId) {

        if (this.listener != null) {
            this.listener.onUpdate(originalFilter, ctx);
        }

        VideoEffectorContext.FrameInfo info = ctx.getFrameInfo();
        this.originalFilter.onOutputSizeChanged(info.getWidth(), info.getHeight());
        resizeTextureIfNeeded(info.getWidth(), info.getHeight());

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, bufferId);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);

        this.originalFilter.onDraw(srcTextureId, FULL_RECTANGLE_BUF, FULL_RECTANGLE_TEX_BUF);

        GLES20.glFlush();
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        return textureId;
    }

    @Override
    public void dispose() {
        GLES20.glDeleteTextures(1, new int[]{textureId}, 0);
        GLES20.glDeleteFramebuffers(1, new int[]{bufferId}, 0);
    }
}
