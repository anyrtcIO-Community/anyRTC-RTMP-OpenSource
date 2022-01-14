#import "base/RTCVideoRenderer.h"
#import "base/RTCVideoFrame.h"
#include "sdk/objc/native/src/objc_frame_buffer.h"
#include "../VideoRender/video_renderer.h"
#include "../VideoRender/RTCVideoRender.h"

namespace webrtc {
    
class IOSRender : public RTCVideoRender, public VideoRenderer {
public: // Copy form VideoRendererAdapter.mm
    IOSRender(id<RTCVideoRenderer> render){
        video_renderer_ = render;
        size_ = CGSizeZero;
    };
    
    virtual ~IOSRender(void){
        
    };
    
    virtual void* GetHWND(){
        return NULL;
    };
    
    virtual void OnChangeVideoSize(int width, int height){
    };
    
    RTCVideoRender* Render() {
        return this;
    }

    virtual void DoScale(int width, int height){};
    virtual void Mirror(){};
    virtual int DoCapture(const char* fileName){
        return -1;
    };
    
    void OnFrame(const webrtc::VideoFrame& nativeVideoFrame) override {
        int64_t time  = 0;
        if (nativeVideoFrame.timestamp_us() !=0) {
            time = nativeVideoFrame.timestamp_us();
        } else {
            time = nativeVideoFrame.timestamp();
        }
        RTCVideoFrame* videoFrame = [[RTCVideoFrame alloc]
        initWithBuffer:ToObjCVideoFrameBuffer(nativeVideoFrame.video_frame_buffer())
        rotation:(RTCVideoRotation)nativeVideoFrame.rotation()
        timeStampNs:time];

        CGSize current_size = (videoFrame.rotation % 180 == 0)
        ? CGSizeMake(videoFrame.width, videoFrame.height)
        : CGSizeMake(videoFrame.height, videoFrame.width);
        if (!CGSizeEqualToSize(size_, current_size)) {
            size_ = current_size;
            [video_renderer_ setSize:size_];
        }
        [video_renderer_ renderFrame:videoFrame];
    }
    
public:
    bool Init(size_t width, size_t height){return true;};
    void Resize(size_t width, size_t height){};
    void Destroy(){};
    
private:
    id<RTCVideoRenderer> video_renderer_;
    CGSize size_;
    
#ifdef FF_CAPTURE
    rtc::CriticalSection    cs_video_cap_buf_;
    rtc::scoped_refptr<webrtc::I420Buffer> video_cap_buffer_;
#endif
};

VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd, size_t width, size_t height) {
    IOSRender* render = new IOSRender((__bridge id<RTCVideoRenderer>)hwnd);
    render->Init(width, height);
    return render;
}

}//namespace webrtc
