/*
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#include "video_renderer.h"
// TODO(pbos): Android renderer

#include "webrtc/typedefs.h"
#include "webrtc/media/base/videosinkinterface.h"
#include "webrtc/media/engine/webrtcvideoframe.h"

namespace webrtc {

class NullRenderer : public VideoRenderer {
	void OnFrame(const cricket::VideoFrame& video_frame) override {}
};
    
class AndroidRender : public VideoRenderer {
public:
    AndroidRender(rtc::VideoSinkInterface<cricket::VideoFrame> *render){
        video_renderer_ = render;
    };
    virtual ~AndroidRender(void){};

    void OnFrame(const cricket::VideoFrame& nativeVideoFrame) override {
        if (video_renderer_ != NULL) {
			video_renderer_->OnFrame(nativeVideoFrame);
		}
    }
    
public:
    bool Init(size_t width, size_t height){return true;};
    void Resize(size_t width, size_t height){};
    void Destroy(){};
    
private:
    rtc::VideoSinkInterface<cricket::VideoFrame>	*video_renderer_;
};
    
VideoRenderer* VideoRenderer::CreatePlatformRenderer(const void* hwnd,
                                     size_t width,
                                     size_t height)
{
    AndroidRender* render = new AndroidRender((rtc::VideoSinkInterface<cricket::VideoFrame>*)hwnd);
    render->Init(width, height);
    return render;
}

VideoRenderer* VideoRenderer::Create(const void* hwnd,
                                     size_t width,
                                     size_t height) {
  VideoRenderer* renderer = CreatePlatformRenderer(hwnd, width, height);
  if (renderer != NULL) {
    // TODO(mflodman) Add a warning log.
    return renderer;
  }

  return new NullRenderer();
}
}  // namespace webrtc
