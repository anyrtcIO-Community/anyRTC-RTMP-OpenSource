/// @defgroup ArLivePremier_cplusplus ArLivePremier
///
/// @{

#ifndef ANYRTC_CPP_ARLIVE_PREMIER_H_
#define ANYRTC_CPP_ARLIVE_PREMIER_H_

#include "ArLiveDef.hpp"

namespace anyrtc {
#ifdef _WIN32
class ArLivePremierObserver;
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                      ArLive 高级接口
//
/////////////////////////////////////////////////////////////////////////////////

class V2_API ArLivePremier {
   public:
    /**
     * 1. 获取 SDK 版本号
     */
    static const char* getSDKVersionStr();

/**
 * 2. 设置 ArLivePremier 回调接口
 */
#ifdef _WIN32
    static void setObserver(ArLivePremierObserver* observer);
#endif

/**
 * 3. 开启/关闭音频采集回调
 *
 * @param enable true：开启音频采集回调，用于第三方美声；false：关闭音频采集回调。【默认值】：false
 * @param format 回调出来的 AudioFrame 的格式
 *
 * @note 需要在 {@link ArLivePusher#startPush} 之前调用，才会生效。
 */
#ifdef _WIN32
    static int32_t enableAudioCaptureObserver(bool enable, const ArLiveAudioFrameObserverFormat& format);
#endif

    /**
     * 4. 设置 SDK 接入环境
     *
     * @note 如您的应用无特殊需求，请不要调用此接口进行设置。
     * @param env 目前支持 “default” 和 “GDPR” 两个参数
     *        - default：默认环境，SDK 会在全球寻找最佳接入点进行接入。
     *        - GDPR：所有音视频数据和质量统计数据都不会经过中国大陆地区的服务器。
     */
    static int32_t setEnvironment(const char* env);

/**
 * 6. 设置 SDK sock5 代理配置
 *
 * @param host sock5 代理服务器的地址
 * @param port sock5 代理服务器的端口
 * @param username sock5 代理服务器的验证的用户名
 * @param password sock5 代理服务器的验证的密码
 */
#ifdef _WIN32
    static int32_t setSocks5Proxy(const char* host, unsigned short port, const char* username, const char* password);
#endif
};

/////////////////////////////////////////////////////////////////////////////////
//
//                      ArLive 高级回调接口
//
/////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
class ArLivePremierObserver {
   public:
    virtual ~ArLivePremierObserver() {
    }

    /**
     * 本地麦克风采集到的音频数据回调
     *
     * @param frame 音频数据
     *
     * @note - 请不要在此回调函数中做任何耗时操作，建议直接拷贝到另一线程进行处理，否则会导致各种声音问题
     * @note - 此接口回调出的音频数据支持修改
     * @note - 此接口回调出的音频时间帧长固定为0.02s
     *         由时间帧长转化为字节帧长的公式为【采样率 × 时间帧长 × 声道数 × 采样点位宽】。
     *         以SDK默认的音频录制格式48000采样率、单声道、16采样点位宽为例，字节帧长为【48000 × 0.02s × 1 × 16bit = 15360bit = 1920字节】
     * @note - 此接口回调出的音频数据**不包含**背景音、音效、混响等前处理效果，延迟极低。
     */
    virtual void onCaptureAudioFrame(ArLiveAudioFrame* frame){};
};
#endif

}  // namespace anyrtc
#endif  // #ifndef ANYRTC_CPP_ARLIVE_PREMIER_H_
/// @}
