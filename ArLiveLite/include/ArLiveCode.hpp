// Copyright (c) 2020 anyRTC(https://www.anyrtc.io). All rights reserved.

/// @defgroup ArLiveCode_cplusplus ArLiveCode
/// anyRTC开源直播服务错误码和警告码的定义
/// @{

#ifndef ANYRTC_CPP_ARLIVECODE_H_
#define ANYRTC_CPP_ARLIVECODE_H_

namespace anyrtc {

enum ArLiveCode {

    /// 没有错误
    ArLIVE_OK = 0,

    /// 暂未归类的通用错误
    ArLIVE_ERROR_FAILED = -1,

    /// 调用 API 时，传入的参数不合法
    ArLIVE_ERROR_INVALID_PARAMETER = -2,

    /// API 调用被拒绝
    ArLIVE_ERROR_REFUSED = -3,

    /// 当前 API 不支持调用
    ArLIVE_ERROR_NOT_SUPPORTED = -4,

    /// license 不合法，调用失败
    ArLIVE_ERROR_INVALID_LICENSE = -5,

    /// 请求服务器超时
    ArLIVE_ERROR_REQUEST_TIMEOUT = -6,

    /// 服务器无法处理您的请求
    ArLIVE_ERROR_SERVER_PROCESS_FAILED = -7,

    /////////////////////////////////////////////////////////////////////////////////
    //
    //      网络相关的警告码
    //
    /////////////////////////////////////////////////////////////////////////////////

    /// 网络状况不佳：上行带宽太小，上传数据受阻
    ArLIVE_WARNING_NETWORK_BUSY = 1101,

    /// 当前视频播放出现卡顿
    ArLIVE_WARNING_VIDEO_BLOCK = 2105,

    /////////////////////////////////////////////////////////////////////////////////
    //
    //             摄像头相关的警告码
    //
    /////////////////////////////////////////////////////////////////////////////////

    /// 摄像头打开失败
    ArLIVE_WARNING_CAMERA_START_FAILED = -1301,

    /// 摄像头正在被占用中，可尝试打开其他摄像头
    ArLIVE_WARNING_CAMERA_OCCUPIED = -1316,

    /// 摄像头设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ArLIVE_WARNING_CAMERA_NO_PERMISSION = -1314,

    /////////////////////////////////////////////////////////////////////////////////
    //
    //             麦克风相关的警告码
    //
    /////////////////////////////////////////////////////////////////////////////////

    /// 麦克风打开失败
    ArLIVE_WARNING_MICROPHONE_START_FAILED = -1302,

    /// 麦克风正在被占用中，例如移动设备正在通话时，打开麦克风会失败
    ArLIVE_WARNING_MICROPHONE_OCCUPIED = -1319,

    /// 麦克风设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ArLIVE_WARNING_MICROPHONE_NO_PERMISSION = -1317,

    /////////////////////////////////////////////////////////////////////////////////
    //
    //             屏幕分享相关警告码
    //
    /////////////////////////////////////////////////////////////////////////////////

    /// 当前系统不支持屏幕分享
    ArLIVE_WARNING_SCREEN_CAPTURE_NOT_SUPPORTED = -1309,

    /// 开始录屏失败，如果在移动设备出现，可能是权限被用户拒绝了
    ArLIVE_WARNING_SCREEN_CAPTURE_START_FAILED = -1308,

    /// 录屏被系统中断
    ArLIVE_WARNING_SCREEN_CAPTURE_INTERRUPTED = -7001,

};
/// @}

}  // namespace anyrtc

#endif  // ANYRTC_CPP_ARLIVECODE_H_
/// @}
