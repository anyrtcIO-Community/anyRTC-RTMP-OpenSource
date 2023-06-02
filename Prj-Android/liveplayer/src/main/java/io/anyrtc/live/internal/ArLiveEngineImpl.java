package io.anyrtc.live.internal;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.Application;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;


import org.webrtc.ContextUtils;
import org.webrtc.Logging;
import org.webrtc.ThreadUtils;
import org.webrtc.voiceengine.WebRtcAudioManager;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.anyrtc.live.ArLiveEngine;
import io.anyrtc.live.ArLivePlayer;
import io.anyrtc.live.ArLivePusher;

public class ArLiveEngineImpl extends ArLiveEngine {

    private Context context;
    public static volatile Handler applicationHandler;
    //private ArLiveEngineObserver arLiveEngineEvent;
    private NativeInstance nativeInstance;
    protected List<ArLivePlayerImpl> playerList = new ArrayList<>();
    protected List<ArLivePusherImpl> pusherList = new ArrayList<>();
    private ProcessLifecycleOwner  mProcessLifecycleOwner = null;

    private ArLivePusherImpl pusher;
    protected ArDeviceManagerImpl deviceManager;
    public ArLiveEngineImpl(Context context) {
        this.context = context;
        ContextUtils.initialize(context);
        applicationHandler = new Handler(context.getMainLooper());
        WebRtcAudioManager.setBlacklistDeviceForOpenSLESUsage(true);
        nativeInstance = new NativeInstance();
        deviceManager = new ArDeviceManagerImpl(nativeInstance,context);
        try{
            mProcessLifecycleOwner = new ProcessLifecycleOwner(isAppInForeground(),this);
            Application app = (Application)context.getApplicationContext();
            app.registerActivityLifecycleCallbacks(this.mProcessLifecycleOwner);
        }catch (Exception e){
            Logging.e("ArLivePusherImpl", "Unable to registerActivityLifecycleCallbacks, ", e);
        }

    }

    public ArLivePusherImpl getPusher() {
        return pusher;
    }

    @Override
    public ArLivePusher createArLivePusher() {
        if (pusher == null) {
            pusher = new ArLivePusherImpl();
            pusher.setHandler(applicationHandler,context);
            boolean isSuccess = pusher.attach(nativeInstance);
            if (isSuccess){
                return pusher;
            }else {
                throw new RuntimeException("not allowed create the ArLivePushKit!");
            }
        }else {
            return pusher;
        }

    }


    @Override
    public ArLivePlayer createArLivePlayer() {
        ArLivePlayerImpl arLivePlayKit = new ArLivePlayerImpl();
        arLivePlayKit.setHandler(applicationHandler);
        boolean isSuccess = arLivePlayKit.attach(nativeInstance);
        if (isSuccess){
            playerList.add(arLivePlayKit);
            return arLivePlayKit;
        }else {
            throw new RuntimeException("not allowed create the ArLivePushKit!");
        }
    }



    public void doDestory() {
        for (ArLivePlayerImpl arLivePlayer:playerList){
            if (arLivePlayer.isNativeOk()){
                arLivePlayer.stopPlay();
            }
        }
        if (pusher!=null){
            if (pusher.isNativeOk()){
                pusher.stopPush();
            }
        }
        deviceManager.release();
        playerList.clear();
        try {
            if (this.mProcessLifecycleOwner != null) {
                Application app = (Application)context.getApplicationContext();
                app.unregisterActivityLifecycleCallbacks(this.mProcessLifecycleOwner);
                this.mProcessLifecycleOwner = null;
            }
        } catch (Exception var3) {
            Logging.e("ArLiveEngineImpl", "unregister ProcessLifecycleOwner failed ", var3);
        }
        nativeInstance.nativeRelease();
        NativeLoader.unInit();
    }


    private  boolean isAppInForeground() {
        final ActivityManager.RunningAppProcessInfo appProcessInfo = new ActivityManager.RunningAppProcessInfo();
        final CountDownLatch countLatch = new CountDownLatch(1);
        Runnable processInfoTask = new Runnable() {
            public void run() {
                try {
                    ActivityManager.getMyMemoryState(appProcessInfo);
                } catch (Exception var2) {
                    Log.e("ArLiveEngineImpl", "get App InForeground state failed.", var2);
                }

                countLatch.countDown();
            }
        };
        (new Thread(processInfoTask)).start();
        if (!ThreadUtils.awaitUninterruptibly(countLatch, 100L)) {
            Log.e("ArLiveEngineImpl", "get App InForeground state timeout.");
            return true;
        } else {
            return appProcessInfo.importance == 100 || appProcessInfo.importance == 200;
        }
    }


    void onForegroundChanged(boolean foreground) {
        Log.d("ArLiveEngineImpl", "onForegroundChanged() " + foreground);
        nativeInstance.nativeSetAppInBackground(!foreground);
    }








}

