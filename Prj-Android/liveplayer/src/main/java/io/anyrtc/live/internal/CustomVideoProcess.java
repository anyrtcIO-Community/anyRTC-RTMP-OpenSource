package io.anyrtc.live.internal;

import org.webrtc.VideoFrame;

import io.anyrtc.live.ArLiveDef;
import io.anyrtc.live.ArLivePusherObserver;

public class CustomVideoProcess {

    private volatile static CustomVideoProcess singleton;
    protected boolean enable = false;
    protected ArLiveDef.ArLivePixelFormat format = ArLiveDef.ArLivePixelFormat.ArLivePixelFormatUnknown;
    protected ArLiveDef.ArLiveBufferType bufferType = ArLiveDef.ArLiveBufferType.ArLiveBufferTypeUnknown;
    protected ArLivePusherObserver observer = null;

    private CustomVideoProcess (){}

    public static CustomVideoProcess getSingleton() {
        if (singleton == null) {
            synchronized (CustomVideoProcess.class) {
                if (singleton == null) {
                    singleton = new CustomVideoProcess();
                }
            }
        }
        return singleton;
    }
    public void enableCustomVideoProcess(boolean var1, ArLiveDef.ArLivePixelFormat var2, ArLiveDef.ArLiveBufferType var3, ArLivePusherObserver observer) {
        this.enable = var1;
        this.bufferType = var3;
        this.format = var2;
        this.observer = observer;

    }

    protected void setFrame(VideoFrame videoFrame){

    }


}
