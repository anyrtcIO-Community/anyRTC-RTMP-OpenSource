package io.anyrtc.live.internal;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import org.webrtc.Logging;

import java.lang.ref.WeakReference;

class ProcessLifecycleOwner implements Application.ActivityLifecycleCallbacks {
    private static final String TAG = "ProcessLifecycleOwner";
    private static final long TIMEOUT_MS = 1000L;
    private  final WeakReference<ArLiveEngineImpl> arLiveEngineWeakReference;
    private final Handler handler;
    private boolean isForeground;
    private final Runnable mDelayedPauseRunnable = new Runnable() {
        public void run() {
            ProcessLifecycleOwner.this.setForeground(false);
        }
    };
    private final Runnable mDelayedResumeRunnable = new Runnable() {
        public void run() {
            ProcessLifecycleOwner.this.setForeground(true);
        }
    };

    ProcessLifecycleOwner(boolean foreground,ArLiveEngineImpl arLiveEngine) {
        this.isForeground = foreground;
        this.handler = new Handler(Looper.getMainLooper());
        this.arLiveEngineWeakReference = new WeakReference<>(arLiveEngine);
        Logging.d("ProcessLifecycleOwner", "ProcessLifecycleOwner, isForeground : " + this.isForeground);
    }

    public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
    }

    public void onActivityStarted(Activity activity) {
    }

    public void onActivityResumed(Activity activity) {
        Logging.d("ProcessLifecycleOwner", "onActivityResumed()");
        this.handler.removeCallbacks(this.mDelayedPauseRunnable);
        this.handler.postDelayed(this.mDelayedResumeRunnable, 1000L);
    }

    public void onActivityPaused(Activity activity) {
        Logging.d("ProcessLifecycleOwner", "onActivityPaused()");
        this.handler.removeCallbacks(this.mDelayedResumeRunnable);
        this.handler.postDelayed(this.mDelayedPauseRunnable, 1000L);
    }

    public void onActivityStopped(Activity activity) {
    }

    public void onActivitySaveInstanceState(Activity activity, Bundle outState) {
    }

    public void onActivityDestroyed(Activity activity) {
    }

    private void setForeground(boolean para) {
        if (this.isForeground != para) {
            this.isForeground = para;
            ArLiveEngineImpl cu = (ArLiveEngineImpl)this.arLiveEngineWeakReference.get();
            if (cu != null) {
                cu.onForegroundChanged(this.isForeground);
            }
        }
    }
}

