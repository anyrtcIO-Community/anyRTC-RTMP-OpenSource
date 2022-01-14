package io.anyrtc.live.internal;

import io.anyrtc.live.ArBeautyManager;

public class ArBeautyManagerImpl extends ArBeautyManager {

    private NativeInstance nativeInstance;
    private boolean beautyEffect = false;

    public ArBeautyManagerImpl(NativeInstance instance) {
        this.nativeInstance = instance;
        nativeInstance.nativeSetBeautyEffect(true);
        beautyEffect = true;
    }


    @Override
    public void setBeautyEffect(boolean enable) {
        this.beautyEffect = enable;
        nativeInstance.nativeSetBeautyEffect(enable);
    }

    @Override
    public void setWhitenessLevel(float leave) {
        if (beautyEffect){
            nativeInstance.nativeSetWhitenessLevel(leave);
        }
    }

    @Override
    public void setBeautyLevel(float leave) {
        if (beautyEffect){
            nativeInstance.nativeSetBeautyLevel(leave);
        }
    }

    @Override
    public void setToneLevel(float leave) {
        if (beautyEffect){
            nativeInstance.nativeSetToneLevel(leave);
        }
    }
}
