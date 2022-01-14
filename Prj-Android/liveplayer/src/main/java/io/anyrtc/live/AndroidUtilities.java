package io.anyrtc.live;

import io.anyrtc.live.internal.ArLiveEngineImpl;

public class AndroidUtilities{

    public static void runOnUIThread(Runnable runnable) {
        runOnUIThread(runnable, 0);
    }

    public static void runOnUIThread(Runnable runnable, long delay) {
        if (ArLiveEngineImpl.applicationHandler == null) {
            return;
        }
        if (delay == 0) {
            ArLiveEngineImpl.applicationHandler.post(runnable);
        } else {
            ArLiveEngineImpl.applicationHandler.postDelayed(runnable, delay);
        }
    }

    public static void cancelRunOnUIThread(Runnable runnable) {
        if (ArLiveEngineImpl.applicationHandler == null) {
            return;
        }
        ArLiveEngineImpl.applicationHandler.removeCallbacks(runnable);
    }
}