package io.anyrtc.live.internal;

import android.content.Context;
import android.os.Handler;


import org.webrtc.ContextUtils;
import org.webrtc.voiceengine.WebRtcAudioManager;

import java.util.ArrayList;
import java.util.List;

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

    private ArLivePusherImpl pusher;
    protected ArDeviceManagerImpl deviceManager;
    public ArLiveEngineImpl(Context context) {
        this.context = context;
        ContextUtils.initialize(context);
        applicationHandler = new Handler(context.getMainLooper());
        WebRtcAudioManager.setBlacklistDeviceForOpenSLESUsage(true);
        nativeInstance = new NativeInstance();
        deviceManager = new ArDeviceManagerImpl(nativeInstance,context);

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
        nativeInstance.nativeRelease();
        NativeLoader.unInit();
    }












}

