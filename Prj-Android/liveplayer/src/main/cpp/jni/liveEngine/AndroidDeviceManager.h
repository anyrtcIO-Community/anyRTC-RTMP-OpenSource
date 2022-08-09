//
// Created by liu on 2021/11/4.
//

#ifndef LIVEPLAYER_ANDROIDDEVICEMANAGER_H
#define LIVEPLAYER_ANDROIDDEVICEMANAGER_H

#include "jni.h"
#include <modules/utility/include/jvm_android.h>
#include <IArDeviceManager.h>
#include "webrtc/sdk/android/src/jni/jvm.h"
#include "sdk/android/native_api/video/video_source.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "PlatformContext.h"
#include "VideoCameraCapturer.h"
#include "sdk/android/native_api/jni/class_loader.h"

class AndroidDeviceManager{

public:
    AndroidDeviceManager(void);

    virtual ~AndroidDeviceManager(void);

    static AndroidDeviceManager &Inst();

    void setPlatformContext(std::shared_ptr<arlive::PlatformContext> _platformContext);

    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> createVideoSource();

    void setVideoSource(rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _videoSource,
                        std::string deviceId, bool isScreenCapture);


    void *
    createVideoCapture(size_t width, size_t height, size_t fps, size_t capture_device_index);

    std::unique_ptr<webrtc::VideoEncoderFactory> makeVideoEncoderFactory();
    std::unique_ptr<webrtc::VideoDecoderFactory> makeVideoDecoderFactory();

    jint switchCamera(bool isFront);
    jboolean isFrontCamera();
    jfloat getCameraZoomMaxRatio();
    jint setCameraZoomRatio(float zoomRatio);
    void setCameraCapturerParam(int mode,int width,int height);
    jboolean isAutoFocusEnabled();
    jint enableCameraAutoFocus(bool enabled);
    jint setCameraFocusPosition(float x, float y);
    jboolean enableCameraTorch(bool enabled);
    void recoverCamera();

    bool startCapture();

    bool stopCapture();

    void destoryCapture();

    webrtc::ScopedJavaLocalRef<jobject> GetJavaVideoCapturerObserver(JNIEnv *env);

    int setBeautyEffect(bool enable);

    int setWhitenessLevel(float level);

    int setBeautyLevel(float level);

    int setToneLevel(float level);

public:
    rtc::scoped_refptr<webrtc::JavaVideoTrackSourceInterface> videoSource;
    std::unique_ptr<arlive::VideoCameraCapturer> _capturer;
    int captureWidth;
    int captureheight;
    int captureFps;
    bool isScreenCapture;
    std::shared_ptr<arlive::PlatformContext> _platformContext;
    arlive::VideoState _state = arlive::VideoState::Inactive;
};


#endif //LIVEPLAYER_ANDROIDDEVICEMANAGER_H
