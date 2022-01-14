//
// Created by liu on 2021/9/22.
//

#ifndef LIVEPLAYER_ARLIVEPLAYEVENT_H
#define LIVEPLAYER_ARLIVEPLAYEVENT_H
#include "IArLivePlayer.hpp"
#include <webrtc/modules/utility/include/helpers_android.h>
#include "webrtc/sdk/android/src/jni/jvm.h"
#include "webrtc/sdk/android/native_api/jni/scoped_java_ref.h"
#include "webrtc/sdk/android/native_api/jni/java_types.h"

namespace anyrtc{

class LivePlayEvent : public ArLivePlayerObserver{

public:
    LivePlayEvent(jobject event);
    ~LivePlayEvent(void);

public:
    void onError(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo);
    void onWarning(IArLivePlayer* player, int32_t code, const char* msg, void* extraInfo);
    void onVideoPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo);
    void onAudioPlayStatusUpdate(IArLivePlayer* player, ArLivePlayStatus status, ArLiveStatusChangeReason reason, void* extraInfo);
    void onPlayoutVolumeUpdate(IArLivePlayer* player, int32_t volume);
    void onStatisticsUpdate(IArLivePlayer* player, ArLivePlayerStatistics statistics);
    void onSnapshotComplete(IArLivePlayer* player, const char* image, int length, int width, int height, ArLivePixelFormat format);
    void onRenderVideoFrame(IArLivePlayer* player, const ArLiveVideoFrame* videoFrame);
    void onReceiveSeiMessage(IArLivePlayer* player, int payloadType, const uint8_t* data, uint32_t dataSize);
    void onVodPlaybackProcess(IArLivePlayer* player, int nAllDuration, int nCurrentPlaybackTime, int nBufferDuration);
protected:
    jobject m_jJavaObj;
    jclass m_jClass;
};
}

#endif //LIVEPLAYER_ARLIVEPLAYEVENT_H
