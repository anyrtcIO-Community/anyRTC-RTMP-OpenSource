package io.anyrtc.live.util;

public class AsyncHttpEventsImpl implements AsyncHttpURLConnection.AsyncHttpEvents {

    private long nativePrt;
    public AsyncHttpEventsImpl(long nativePtr) {
        this.nativePrt= nativePtr;
    }

    @Override
    public void onHttpError(String errorMessage) {
        setHttpError(nativePrt,errorMessage);
    }

    @Override
    public void onHttpComplete(String response) {
        setHttpComplete(nativePrt,response);
    }

    private native void setHttpError(long nativePrt,String error);
    private native void setHttpComplete(long nativePrt,String response);
}
