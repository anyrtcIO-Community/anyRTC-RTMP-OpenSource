package org.anyrtc.core;

import android.content.Context;

import org.webrtc.EglBase;

/**
 * Created by Eric on 2016/9/15.
 */
public class AnyRTMP {
    /**
     * 加载api所需要的动态库
     */
    static {
        System.loadLibrary("faac");
        System.loadLibrary("faad2");
        System.loadLibrary("openh264");
        System.loadLibrary("anyrtmp-jni");
    }

    private static class SingletonHolder {
        private static final AnyRTMP INSTANCE = new AnyRTMP();
    }

    public static final AnyRTMP Inst() {
        return SingletonHolder.INSTANCE;
    }

    private final LooperExecutor executor;
    private final EglBase eglBase;

    private AnyRTMP() {
        executor = new LooperExecutor();
        eglBase = EglBase.create();
        executor.requestStart();
    }

    public void Init(final Context ctx) {
        executor.execute(new Runnable() {
            @Override
            public void run() {
                nativeInitCtx(ctx, eglBase.getEglBaseContext());
            }
        });
    }

    public LooperExecutor Executor() {
        return executor;
    }

    public EglBase Egl() {
        return eglBase;
    }

    public void dispose() {
        executor.requestStop();
    }

    /**
     * Jni interface
     */
    private static native void nativeInitCtx(Context ctx, EglBase.Context context);
}
