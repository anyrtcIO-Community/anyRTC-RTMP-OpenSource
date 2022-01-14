//
// Created by liu on 2021/9/22.
//

#ifndef LIVEPLAYER_ARLIVEPUSHEVENT_H
#define LIVEPLAYER_ARLIVEPUSHEVENT_H
#include "webrtc/sdk/android/src/jni/jvm.h"
#include "webrtc/sdk/android/native_api/jni/scoped_java_ref.h"
#include "webrtc/sdk/android/native_api/jni/java_types.h"
#include "IArLivePusher.hpp"
#include <webrtc/modules/utility/include/helpers_android.h>
namespace anyrtc {
    class LivePushEvent : public ArLivePusherObserver {
    public:
        LivePushEvent(jobject event);

        ~LivePushEvent(void);

    public:
        void onError(int32_t code, const char* msg, void* extraInfo);

        void onWarning(int32_t code, const char* msg, void* extraInfo);

        void onPushStatusUpdate(ArLivePushStatus state, const char* msg, void* extraInfo);
        void onMicrophoneVolumeUpdate(int32_t volume);

        void onStatisticsUpdate(ArLivePusherStatistics statistics);
        void onSnapshotComplete(const char* image, int length, int width, int height, ArLivePixelFormat format);
    protected:
        jobject m_jJavaObj;
        jclass m_jClass;
    };
}
#endif //LIVEPLAYER_ARLIVEPUSHEVENT_H
