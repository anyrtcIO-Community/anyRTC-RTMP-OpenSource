package org.webrtc.effector.filter;

import android.media.effect.Effect;
import android.media.effect.EffectContext;
import android.media.effect.EffectFactory;
import android.opengl.GLES20;

import org.webrtc.GlUtil;
import org.webrtc.effector.VideoEffectorContext;
import org.webrtc.effector.VideoEffectorLogger;


public class MediaEffectFilter extends FrameImageFilter {
    public static final String TAG = MediaEffectFilter.class.getSimpleName();

    public interface Listener {
        void onInit(Effect effect);
        void onUpdate(Effect effect, VideoEffectorContext vctx);
    }

    private int textureId = -1;

    private Effect effect;
    private String effectType;
    private Listener listener;

    public MediaEffectFilter(String type, Listener listener) {
        this.effectType = type;
        this.listener = listener;
    }

    public MediaEffectFilter(String type) {
        this(type, null);
    }

    @Override
    public void init() {
        VideoEffectorLogger.d(TAG, "init: type=" + this.effectType);

        final int textures[] = new int[1];
        GLES20.glGenTextures(1, textures, 0);
        textureId = textures[0];

        EffectContext ectx = EffectContext.createWithCurrentGlContext();
        EffectFactory factory = ectx.getFactory();
        VideoEffectorLogger.d(TAG, "init: 3");
        effect = factory.createEffect(effectType);

        VideoEffectorLogger.d(TAG, "init: effect created=" + effect.toString());

        if (listener != null) {
            listener.onInit(effect);
        }
    }

    @Override
    public int filter(VideoEffectorContext context, int srcTextureId) {
        VideoEffectorLogger.d(TAG, "filter " + srcTextureId);
        if (listener != null) {
            listener.onUpdate(effect, context);
        }
        VideoEffectorContext.FrameInfo info = context.getFrameInfo();
        effect.apply(srcTextureId, info.getWidth(), info.getHeight(), textureId);
        return textureId;
    }

    @Override
    public void dispose() {
        GLES20.glDeleteTextures(1, new int[]{textureId}, 0);

        if (effect != null) {
            effect.release();
            effect = null;
        }

        listener = null;
    }
}
