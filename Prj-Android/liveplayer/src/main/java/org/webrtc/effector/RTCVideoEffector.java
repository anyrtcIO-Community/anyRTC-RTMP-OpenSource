package org.webrtc.effector;

import org.webrtc.GlUtil;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.ThreadUtils;
import org.webrtc.VideoFrame;
import org.webrtc.effector.filter.FrameImageFilter;
import org.webrtc.effector.filter.GPUImageFilter;
import org.webrtc.effector.filter.GPUImageFilterWrapper;
import org.webrtc.effector.filter.MediaEffectFilter;
import org.webrtc.effector.format.YuvByteBufferDumper;
import org.webrtc.effector.format.YuvByteBufferReader;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;


public class RTCVideoEffector {

    public static final String TAG = RTCVideoEffector.class.getSimpleName();

    public RTCVideoEffector() {}

    private VideoEffectorContext context = new VideoEffectorContext();
    private List<FrameImageFilter> filters = new ArrayList<>();
    private boolean enabled = false;

    private YuvByteBufferReader yuvBytesReader;
    private YuvByteBufferDumper yuvBytesDumper;

    private SurfaceTextureHelper helper;

    public void init(SurfaceTextureHelper helper) {

        VideoEffectorLogger.d(TAG, "init");

        this.helper = helper;
        yuvBytesReader = new YuvByteBufferReader();
        yuvBytesReader.init();

        yuvBytesDumper = new YuvByteBufferDumper();
        yuvBytesDumper.init();


        for (FrameImageFilter filter : filters) {
            filter.init();
        }

        GlUtil.checkNoGLES2Error("RTCVideoEffector.init");
    }

    public void addFilter(FrameImageFilter filter) {
        this.filters.add(filter);
    }

    public void addMediaEffectFilter(String name) {
        addMediaEffectFilter(name, null);
    }

    public void addMediaEffectFilter(String name,
                                     MediaEffectFilter.Listener listener) {
        VideoEffectorLogger.d(TAG, "addMediaEffectFilter: " + name +
                ", listener: " + listener);
        this.filters.add(new MediaEffectFilter(name, listener));
    }

    public void addGPUImageFilter(GPUImageFilter filter) {
        VideoEffectorLogger.d(TAG, "addGPUImageFilter: " + filter.toString());
        this.filters.add(new GPUImageFilterWrapper(filter));
    }

    public void addGPUImageFilter(GPUImageFilter filter,
                                  GPUImageFilterWrapper.Listener listener) {
        VideoEffectorLogger.d(TAG, "addGPUImageFilter: " + filter.toString() +
                ", listener: " + listener);
        this.filters.add(new GPUImageFilterWrapper(filter, listener));
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void enable() {
        enabled = true;
    }

    public void disable() {
        enabled = false;
    }

    public VideoFrame.I420Buffer processByteBufferFrame(VideoFrame.I420Buffer i420Buffer,
                                                 int rotation, long timestamp) {

        if (!needToProcessFrame()) {
            return i420Buffer;
        }

        // Direct buffer ではない場合スルーする
        // TODO: direct に変換してあげる手もある
        if(!i420Buffer.getDataY().isDirect()
                || !i420Buffer.getDataU().isDirect()
                || !i420Buffer.getDataV().isDirect()) {
            return i420Buffer;
        }

        int width = i420Buffer.getWidth();
        int height = i420Buffer.getHeight();
        int strideY = i420Buffer.getStrideY();
        int strideU = i420Buffer.getStrideU();
        int strideV = i420Buffer.getStrideV();

        context.updateFrameInfo(width, height, rotation, timestamp);

        int stepTextureId = yuvBytesReader.read(i420Buffer);

        // ビデオフレームの画像は回転された状態で来ることがある
        // グレースケールやセピアフィルタなど、画像全体に均質にかけるエフェクトでは問題にならないが
        // 座標を指定する必要のあるエフェクトでは、使いにくいものとなる。

        // そのため、場合によっては、フィルタをかける前後で回転の補正を行う必要がある
        // ただし、そのためのtexture間のコピーが二度発生することになる
        // 必要のないときはこの機能は使わないようにon/offできるようにしておきたい

        if (context.getFrameInfo().isRotated()) {
            // TODO
        }

        for (FrameImageFilter filter : filters) {
            if (filter.isEnabled()) {
                stepTextureId = filter.filter(context, stepTextureId);
            }
        }

        if (context.getFrameInfo().isRotated()) {
            // TODO
        }

        return yuvBytesDumper.dump(stepTextureId, width, height, strideY, strideU, strideV);
    }

    public boolean needToProcessFrame() {
        if (!enabled) {
            return false;
        }
        if (filters.size() > 0) {
            for (FrameImageFilter filter : this.filters) {
                if (filter.isEnabled()) {
                    return true;
                }
            }
            return false;
        } else {
            return false;
        }
    }

    public void dispose() {
        if (this.helper != null) {
            // This effector is not initialized
            return;
        }
        ThreadUtils.invokeAtFrontUninterruptibly(this.helper.getHandler(), () ->
                disposeInternal()
        );
    }

    private void disposeInternal() {
        for (FrameImageFilter filter : filters) {
            filter.dispose();
        }
        yuvBytesReader.dispose();
        yuvBytesDumper.dispose();
    }
}
