/**
 * Module:    背景音乐、短音效和人声特效的管理类
 * Function: 用于对背景音乐、短音效和人声特效进行设置的管理类
 */
/// @defgroup ArAudioEffectManager_cplusplus ArAudioEffectManager
/// Tencent Cloud Audio Effect Management Module
/// @{

#ifndef __IARAUDIOEFFECTMANAGER_H__
#define __IARAUDIOEFFECTMANAGER_H__

namespace anyrtc {

class ArMusicPlayObserver;
class AudioMusicParam;

/////////////////////////////////////////////////////////////////////////////////
//
//                    音效相关的枚举值定义
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 音效相关的枚举值定义
/// @{

/**
 * 1.1 混响特效
 *
 * 混响特效可以作用于人声之上，通过声学算法对声音进行叠加处理，模拟出各种不同环境下的临场感受，目前支持如下几种混响效果：
 * 0：关闭；1：KTV；2：小房间；3：大会堂；4：低沉；5：洪亮；6：金属声；7：磁性；8：空灵；9：录音棚；10：悠扬。
 */
enum ArVoiceReverbType {
    ArLiveVoiceReverbType_0 = 0,    ///< disable
    ArLiveVoiceReverbType_1 = 1,    ///< KTV
    ArLiveVoiceReverbType_2 = 2,    ///< small room
    ArLiveVoiceReverbType_3 = 3,    ///< great hall
    ArLiveVoiceReverbType_4 = 4,    ///< deep voice
    ArLiveVoiceReverbType_5 = 5,    ///< loud voice
    ArLiveVoiceReverbType_6 = 6,    ///< metallic sound
    ArLiveVoiceReverbType_7 = 7,    ///< magnetic sound
    ArLiveVoiceReverbType_8 = 8,    ///< ethereal
    ArLiveVoiceReverbType_9 = 9,    ///< studio
    ArLiveVoiceReverbType_10 = 10,  ///< melodious
};

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//                    背景音乐的播放事件回调
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 背景音乐的事件回调接口
/// @{

// Playback progress block of background music
class ArMusicPlayObserver {
   public:
    virtual ~ArMusicPlayObserver() {
    }

    ///背景音乐开始播放
    virtual void onStart(int id, int errCode) = 0;

    ///背景音乐的播放进度
    virtual void onPlayProgress(int id, long curPtsMS, long durationMS) = 0;

    ///背景音乐已经播放完毕
    virtual void onComplete(int id, int errCode) = 0;
};

/// @}
/////////////////////////////////////////////////////////////////////////////////
//
//                    背景音乐的播放控制信息
//
/////////////////////////////////////////////////////////////////////////////////
/// @name 背景音乐的播放控制信息
/// @{

/**
 * 背景音乐的播放控制信息
 *
 * 该信息用于在接口 {@link startPlayMusic} 中指定背景音乐的相关信息，包括播放 ID、文件路径和循环次数等：
 * 1. 如果要多次播放同一首背景音乐，请不要每次播放都分配一个新的 ID，我们推荐使用相同的 ID。
 * 2. 若您希望同时播放多首不同的音乐，请为不同的音乐分配不同的 ID 进行播放。
 * 3. 如果使用同一个 ID 播放不同音乐，SDK 会先停止播放旧的音乐，再播放新的音乐。
 */
class AudioMusicParam {
   public:
    ///【字段含义】音乐 ID <br/>
    ///【特殊说明】SDK 允许播放多路音乐，因此需要使用 ID 进行标记，用于控制音乐的开始、停止、音量等。
    int id;

    ///【字段含义】音效文件的完整路径或 URL 地址。支持的音频格式包括 MP3、AAC、M4A、WAV
    char* path;

    ///【字段含义】音乐循环播放的次数 <br/>
    ///【推荐取值】取值范围为0 - 任意正整数，默认值：0。0表示播放音乐一次；1表示播放音乐两次；以此类推
    int loopCount;

    ///【字段含义】是否将音乐传到远端 <br/>
    ///【推荐取值】YES：音乐在本地播放的同时，远端用户也能听到该音乐；NO：主播只能在本地听到该音乐，远端观众听不到。默认值：NO。
    bool publish;

    ///【字段含义】播放的是否为短音乐文件 <br/>
    ///【推荐取值】YES：需要重复播放的短音乐文件；NO：正常的音乐文件。默认值：NO
    bool isShortFile;

    ///【字段含义】音乐开始播放时间点，单位：毫秒。
    long startTimeMS;

    ///【字段含义】音乐结束播放时间点，单位毫秒，0表示播放至文件结尾。
    long endTimeMS;

    AudioMusicParam(int id_, char* path_) {
        path = path_;
        id = id_;
        loopCount = 0;
        publish = false;
        isShortFile = false;
        startTimeMS = 0;
        endTimeMS = 0;
    }
};
/// @}

// Definition of audio effect management module
class ArAudioEffectManager {
   protected:
    ArAudioEffectManager() {
    }
    virtual ~ArAudioEffectManager() {
    }

   public:
    /////////////////////////////////////////////////////////////////////////////////
    //
    //                    人声相关的特效接口
    //
    /////////////////////////////////////////////////////////////////////////////////
    /// @name 人声相关的特效接口
    /// @{

    /**
     * 1.3 设置人声的混响效果
     *
     * 通过该接口您可以设置人声的混响效果，具体特效请参考枚举定义{@link ArVoiceReverbType}。
     *
     * @note 设置的效果在退出房间后会自动失效，如果下次进房还需要对应特效，需要调用此接口再次进行设置。
     */
    virtual void setVoiceReverbType(ArVoiceReverbType type) = 0;

    /**
     * 1.5 设置语音音量
     *
     * 该接口可以设置语音音量的大小，一般配合音乐音量的设置接口 {@link setAllMusicVolume} 协同使用，用于调谐语音和音乐在混音前各自的音量占比。
     *
     * @param volume 音量大小，取值范围为0 - 100，默认值：100。
     * @note 如果将 volume 设置成 100 之后感觉音量还是太小，可以将 volume 最大设置成 150，但超过 100 的 volume 会有爆音的风险，请谨慎操作。
     */
    virtual void setVoiceCaptureVolume(int volume) = 0;

    /**
     * 1.6 设置语音音调
     *
     * 该接口可以设置语音音调，用于实现变调不变速的目的。
     *
     * @param pitch 音调，取值范围为-1.0f~1.0f，默认值：0.0f。
     */
    virtual void setVoicePitch(double pitch) = 0;

    /// @}
    /////////////////////////////////////////////////////////////////////////////////
    //
    //                    背景音乐的相关接口
    //
    /////////////////////////////////////////////////////////////////////////////////
    /// @name 背景音乐的相关接口
    /// @{

    /**
     * 2.0 设置背景音乐的事件回调接口
     *
     * 请在播放背景音乐之前使用该接口设置播放事件回调，以便感知背景音乐的播放进度。
     *
     * @param musicId   音乐 ID
     * @param observer  具体参考 ArMusicPlayObserver 中定义接口
     */
    virtual void setMusicObserver(int musicId, ArMusicPlayObserver* observer) = 0;

    /**
     * 2.1 开始播放背景音乐
     *
     * 每个音乐都需要您指定具体的 ID，您可以通过该 ID 对音乐的开始、停止、音量等进行设置。
     *
     * @note
     * 1. 如果要多次播放同一首背景音乐，请不要每次播放都分配一个新的 ID，我们推荐使用相同的 ID。
     * 2. 若您希望同时播放多首不同的音乐，请为不同的音乐分配不同的 ID 进行播放。
     * 3. 如果使用同一个 ID 播放不同音乐，SDK 会先停止播放旧的音乐，再播放新的音乐。
     *
     * @param musicParam 音乐参数
     * @param startBlock 播放开始回调
     * @param progressBlock 播放进度回调
     * @param completeBlock 播放结束回调
     */
    virtual void startPlayMusic(AudioMusicParam musicParam) = 0;

    /**
     * 2.2 停止播放背景音乐
     *
     * @param id  音乐 ID
     */
    virtual void stopPlayMusic(int id) = 0;

    /**
     * 2.3 暂停播放背景音乐
     *
     * @param id  音乐 ID
     */
    virtual void pausePlayMusic(int id) = 0;

    /**
     * 2.4 恢复播放背景音乐
     *
     * @param id  音乐 ID
     */
    virtual void resumePlayMusic(int id) = 0;

    /**
     * 2.5 设置所有背景音乐的本地音量和远端音量的大小
     *
     * 该接口可以设置所有背景音乐的本地音量和远端音量。
     * - 本地音量：即主播本地可以听到的背景音乐的音量大小。
     * - 远端音量：即观众端可以听到的背景音乐的音量大小。
     *
     * @param volume 音量大小，取值范围为0 - 100，默认值：100。
     * @note 如果将 volume 设置成 100 之后感觉音量还是太小，可以将 volume 最大设置成 150，但超过 100 的 volume 会有爆音的风险，请谨慎操作。
     */
    virtual void setAllMusicVolume(int volume) = 0;

    /**
     * 2.6 设置某一首背景音乐的远端音量的大小
     *
     * 该接口可以细粒度地控制每一首背景音乐的远端音量，也就是观众端可听到的背景音乐的音量大小。
     *
     * @param id     音乐 ID
     * @param volume 音量大小，取值范围为0 - 100；默认值：100
     * @note 如果将 volume 设置成 100 之后感觉音量还是太小，可以将 volume 最大设置成 150，但超过 100 的 volume 会有爆音的风险，请谨慎操作。
     */
    virtual void setMusicPublishVolume(int id, int volume) = 0;

    /**
     * 2.7 设置某一首背景音乐的本地音量的大小
     *
     * 该接口可以细粒度地控制每一首背景音乐的本地音量，也就是主播本地可以听到的背景音乐的音量大小。
     *
     * @param id     音乐 ID
     * @param volume 音量大小，取值范围为0 - 100，默认值：100。
     * @note 如果将 volume 设置成 100 之后感觉音量还是太小，可以将 volume 最大设置成 150，但超过 100 的 volume 会有爆音的风险，请谨慎操作。
     */
    virtual void setMusicPlayoutVolume(int id, int volume) = 0;

    /**
     * 2.8 调整背景音乐的音调高低
     *
     * @param id    音乐 ID
     * @param pitch 音调，默认值是0.0f，范围是：[-1 ~ 1] 之间的浮点数；
     */
    virtual void setMusicPitch(int id, float pitch) = 0;

    /**
     * 2.9 调整背景音乐的变速效果
     *
     * @param id    音乐 ID
     * @param speedRate 速度，默认值是1.0f，范围是：[0.5 ~ 2] 之间的浮点数；
     */
    virtual void setMusicSpeedRate(int id, float speedRate) = 0;

    /**
     * 2.10 获取背景音乐的播放进度（单位：毫秒）
     *
     * @param id    音乐 ID
     * @return 成功返回当前播放时间，单位：毫秒，失败返回-1
     */
    virtual long getMusicCurrentPosInMS(int id) = 0;

    /**
     * 2.11 获取背景音乐的总时长（单位：毫秒）
     *
     * @param path 音乐文件路径。
     * @return 成功返回时长，失败返回-1
     */
    virtual long getMusicDurationInMS(char* path) = 0;

    /**
     * 2.12 设置背景音乐的播放进度（单位：毫秒）
     *
     * @note 请尽量避免过度频繁地调用该接口，因为该接口可能会再次读写音乐文件，耗时稍高。
     *       因此，当用户拖拽音乐的播放进度条时，请在用户完成拖拽操作后再调用本接口。
     *       因为 UI 上的进度条控件往往会以很高的频率反馈用户的拖拽进度，如不做频率限制，会导致较差的用户体验。
     *
     * @param id  音乐 ID
     * @param pts 单位: 毫秒
     */
    virtual void seekMusicToPosInTime(int id, int pts) = 0;

    /// @}
};
}  // End of namespace anyrtc

#ifdef _WIN32
using namespace anyrtc;
#endif

#endif /* __IARAUDIOEFFECTMANAGER_H__ */

/// @}
