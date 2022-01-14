package io.anyrtc.live;

import android.content.Context;
import android.graphics.Bitmap;

import org.webrtc.SurfaceViewRenderer;
import org.webrtc.TextureViewRenderer;

import io.anyrtc.live.internal.ArLiveEngineImpl;
import io.anyrtc.live.internal.NativeLoader;

public abstract class ArLiveEngine  {


    /**
     * 创建一个 ArLiveEngine 引擎<br/>
     * 只可创建1个 ArLiveEngine d对象
     * @param context 安卓上下文
     * @return 返回 {@link ArLiveEngine}
     */
    public static synchronized ArLiveEngine create(Context context){
        instance = null;
        if (context!=null && NativeLoader.initNativeLibs()){
            if (instance == null){
                instance = new ArLiveEngineImpl(context);
            }
            return instance;
        }else {
            return null;
        }
    }

    /**
     * 释放ArLiveEngine 引擎
     */
    public static void release(){
        if (instance!=null){
            instance.doDestory();
            System.gc();
        }
    }

    /**
     * 创建推流器
     * @return 返回一个推流器
     */
    public abstract ArLivePusher createArLivePusher();

    /**
     * 创建播放（拉流）器（可以创建多个）
     * @return 返回创建等播放器
     */
    public abstract ArLivePlayer createArLivePlayer();


    public static ArLiveEngineImpl getInstance() {
        return instance;
    }

    private static ArLiveEngineImpl instance = null;





}
