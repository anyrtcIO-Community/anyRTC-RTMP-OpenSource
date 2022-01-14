package io.anyrtc.live.internal;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.util.Log;

import java.io.File;

public class NativeLoader {

    private static volatile boolean nativeLoaded = false;

    public static synchronized boolean initNativeLibs() {
        if (nativeLoaded) {
            return true;
        }
        try {
            try {
                System.loadLibrary("anyLive");
                nativeLoaded = true;
                return true;
            } catch (Error e) {
                Log.e("anyLive",e.getMessage());
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
        return false;
    }

    public static void unInit(){
        nativeLoaded = false;
    }
}
