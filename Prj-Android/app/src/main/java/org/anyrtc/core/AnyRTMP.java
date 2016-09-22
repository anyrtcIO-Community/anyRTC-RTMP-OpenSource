/**
 *  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
 *
 *  Please visit https://www.anyrtc.io for detail.
 *
 * The GNU General Public License is a free, copyleft license for
 * software and other kinds of works.
 *
 * The licenses for most software and other practical works are designed
 * to take away your freedom to share and change the works.  By contrast,
 * the GNU General Public License is intended to guarantee your freedom to
 * share and change all versions of a program--to make sure it remains free
 * software for all its users.  We, the Free Software Foundation, use the
 * GNU General Public License for most of our software; it applies also to
 * any other work released this way by its authors.  You can apply it to
 * your programs, too.
 * See the GNU LICENSE file for more info.
 */
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
