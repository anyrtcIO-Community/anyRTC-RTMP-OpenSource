#ifndef __AR_AVCODE_H__
#define __AR_AVCODE_H__

/////////////////////////////////////////////////////////////////////////////////
//
//                     错误码
//
/////////////////////////////////////////////////////////////////////////////////

typedef enum ArAVError
{
    /////////////////////////////////////////////////////////////////////////////////
    //
    //       基础错误码
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_NULL                                        = 0,        ///< 无错误

    /////////////////////////////////////////////////////////////////////////////////
    //
    //       进房（enterRoom）相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##onEnterRoom() 和 TRTCCloudDelegate##OnError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_ROOM_ENTER_FAIL                             = -3301,    ///< 进入房间失败
    ERR_ENTER_ROOM_PARAM_NULL                       = -3316,    ///< 进房参数为空，请检查 enterRoom:appScene: 接口调用是否传入有效的 param
    ERR_SDK_APPID_INVALID                           = -3317,    ///< 进房参数 sdkAppId 错误
    ERR_ROOM_ID_INVALID                             = -3318,    ///< 进房参数 roomId 错误
    ERR_USER_ID_INVALID                             = -3319,    ///< 进房参数 userID 不正确
    ERR_USER_SIG_INVALID                            = -3320,    ///< 进房参数 userSig 不正确
    ERR_ROOM_REQUEST_ENTER_ROOM_TIMEOUT             = -3308,    ///< 请求进房超时，请检查网络
    ERR_SERVER_INFO_SERVICE_SUSPENDED               = -100013,  ///< 服务不可用。请检查：套餐包剩余分钟数是否大于0，腾讯云账号是否欠费


    /////////////////////////////////////////////////////////////////////////////////
    //
    //       退房（exitRoom）相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##OnError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_ROOM_REQUEST_QUIT_ROOM_TIMEOUT              = -3325,    ///< 请求退房超时


    /////////////////////////////////////////////////////////////////////////////////
    //
    //       设备（摄像头、麦克风、扬声器）相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##OnError() 通知
    //             区段：-6000 ~ -6999
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_CAMERA_START_FAIL                           = -1301,    ///< 打开摄像头失败，例如在 Windows 或 Mac 设备，摄像头的配置程序（驱动程序）异常，禁用后重新启用设备，或者重启机器，或者更新配置程序
    ERR_CAMERA_NOT_AUTHORIZED                       = -1314,    ///< 摄像头设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ERR_CAMERA_SET_PARAM_FAIL                       = -1315,    ///< 摄像头参数设置出错（参数不支持或其它）
    ERR_CAMERA_OCCUPY                               = -1316,    ///< 摄像头正在被占用中，可尝试打开其他摄像头
    ERR_MIC_START_FAIL                              = -1302,    ///< 打开麦克风失败，例如在 Windows 或 Mac 设备，麦克风的配置程序（驱动程序）异常，禁用后重新启用设备，或者重启机器，或者更新配置程序
    ERR_MIC_NOT_AUTHORIZED                          = -1317,    ///< 麦克风设备未授权，通常在移动设备出现，可能是权限被用户拒绝了
    ERR_MIC_SET_PARAM_FAIL                          = -1318,    ///< 麦克风设置参数失败
    ERR_MIC_OCCUPY                                  = -1319,    ///< 麦克风正在被占用中，例如移动设备正在通话时，打开麦克风会失败
    ERR_MIC_STOP_FAIL                               = -1320,    ///< 停止麦克风失败
    ERR_SPEAKER_START_FAIL                          = -1321,    ///< 打开扬声器失败，例如在 Windows 或 Mac 设备，扬声器的配置程序（驱动程序）异常，禁用后重新启用设备，或者重启机器，或者更新配置程序
    ERR_SPEAKER_SET_PARAM_FAIL                      = -1322,    ///< 扬声器设置参数失败
    ERR_SPEAKER_STOP_FAIL                           = -1323,    ///< 停止扬声器失败
    
    /////////////////////////////////////////////////////////////////////////////////
    //
    //       系统声音采集相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##onSystemAudioLoopbackError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_AUDIO_PLUGIN_START_FAIL                     = -1330,    ///< 开启系统声音录制失败，例如音频驱动插件不可用
    ERR_AUDIO_PLUGIN_INSTALL_NOT_AUTHORIZED         = -1331,    ///< 安装音频驱动插件未授权
    ERR_AUDIO_PLUGIN_INSTALL_FAILED                 = -1332,    ///< 安装音频驱动插件失败

    /////////////////////////////////////////////////////////////////////////////////
    //
    //       屏幕分享相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##OnError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_SCREEN_CAPTURE_START_FAIL                   = -1308,    ///< 开始录屏失败，如果在移动设备出现，可能是权限被用户拒绝了，如果在 Windows 或 Mac 系统的设备出现，请检查录屏接口的参数是否符合要求
    ERR_SCREEN_CAPTURE_UNSURPORT                    = -1309,    ///< 录屏失败，在 Android 平台，需要5.0以上的系统，在 iOS 平台，需要11.0以上的系统
    ERR_SERVER_CENTER_NO_PRIVILEDGE_PUSH_SUB_VIDEO  = -102015,  ///< 没有权限上行辅路
    ERR_SERVER_CENTER_ANOTHER_USER_PUSH_SUB_VIDEO   = -102016,  ///< 其他用户正在上行辅路
    ERR_SCREEN_CAPTURE_STOPPED                      = -7001,    ///< 录屏被系统中止

    /////////////////////////////////////////////////////////////////////////////////
    //
    //       编解码相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##OnError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_VIDEO_ENCODE_FAIL                           = -1303,    ///< 视频帧编码失败，例如 iOS 设备切换到其他应用时，硬编码器可能被系统释放，再切换回来时，硬编码器重启前，可能会抛出
    ERR_UNSUPPORTED_RESOLUTION                      = -1305,    ///< 不支持的视频分辨率
    ERR_AUDIO_ENCODE_FAIL                           = -1304,    ///< 音频帧编码失败，例如传入自定义音频数据，SDK 无法处理
    ERR_UNSUPPORTED_SAMPLERATE                      = -1306,    ///< 不支持的音频采样率

    /////////////////////////////////////////////////////////////////////////////////
    //
    //       自定义采集相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##OnError() 通知
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_PIXEL_FORMAT_UNSUPPORTED                    = -1327,    ///< 设置的 pixel format 不支持
    ERR_BUFFER_TYPE_UNSUPPORTED                     = -1328,    ///< 设置的 buffer type 不支持


    /////////////////////////////////////////////////////////////////////////////////
    //
    //       CDN 绑定和混流相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##onStartPublishing() 和  TRTCCloudDelegate##onSetMixTranscodingConfig 通知。
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_PUBLISH_CDN_STREAM_REQUEST_TIME_OUT         = -3321,    ///< 旁路转推请求超时
    ERR_CLOUD_MIX_TRANSCODING_REQUEST_TIME_OUT      = -3322,    ///< 云端混流请求超时
    ERR_PUBLISH_CDN_STREAM_SERVER_FAILED            = -3323,    ///< 旁路转推回包异常
    ERR_CLOUD_MIX_TRANSCODING_SERVER_FAILED         = -3324,    ///< 云端混流回包异常
    ERR_ROOM_REQUEST_START_PUBLISHING_TIMEOUT       = -3333,    ///< 开始向腾讯云的直播 CDN 推流信令超时
    ERR_ROOM_REQUEST_START_PUBLISHING_ERROR         = -3334,    ///< 开始向腾讯云的直播 CDN 推流信令异常
    ERR_ROOM_REQUEST_STOP_PUBLISHING_TIMEOUT        = -3335,    ///< 停止向腾讯云的直播 CDN 推流信令超时
    ERR_ROOM_REQUEST_STOP_PUBLISHING_ERROR          = -3336,    ///< 停止向腾讯云的直播 CDN 推流信令异常
    

    /////////////////////////////////////////////////////////////////////////////////
    //
    //       跨房连麦（ConnectOtherRoom）相关错误码
    //       NOTE: 通过回调函数 TRTCCloudDelegate##onConnectOtherRoom() 通知。
    //
    /////////////////////////////////////////////////////////////////////////////////
    ERR_ROOM_REQUEST_CONN_ROOM_TIMEOUT              = -3326,    ///< 请求连麦超时
    ERR_ROOM_REQUEST_DISCONN_ROOM_TIMEOUT           = -3327,    ///< 请求退出连麦超时
    ERR_ROOM_REQUEST_CONN_ROOM_INVALID_PARAM        = -3328,    ///< 无效参数
    ERR_CONNECT_OTHER_ROOM_AS_AUDIENCE              = -3330,    ///< 当前是观众角色，不能请求或断开跨房连麦，需要先 switchRole() 到主播
    ERR_SERVER_CENTER_CONN_ROOM_NOT_SUPPORT         = -102031,  ///< 不支持跨房间连麦
    ERR_SERVER_CENTER_CONN_ROOM_REACH_MAX_NUM       = -102032,  ///< 达到跨房间连麦上限
    ERR_SERVER_CENTER_CONN_ROOM_REACH_MAX_RETRY_TIMES   = -102033,  ///< 跨房间连麦重试次数耗尽
    ERR_SERVER_CENTER_CONN_ROOM_REQ_TIMEOUT         = -102034,  ///< 跨房间连麦请求超时
    ERR_SERVER_CENTER_CONN_ROOM_REQ                 = -102035,  ///< 跨房间连麦请求格式错误
    ERR_SERVER_CENTER_CONN_ROOM_NO_SIG              = -102036,  ///< 跨房间连麦无签名
    ERR_SERVER_CENTER_CONN_ROOM_DECRYPT_SIG         = -102037,  ///< 跨房间连麦签名解密失败
    ERR_SERVER_CENTER_CONN_ROOM_NO_KEY              = -102038,  ///< 未找到跨房间连麦签名解密密钥
    ERR_SERVER_CENTER_CONN_ROOM_PARSE_SIG           = -102039,  ///< 跨房间连麦签名解析错误
    ERR_SERVER_CENTER_CONN_ROOM_INVALID_SIG_TIME    = -102040,  ///< 跨房间连麦签名时间戳错误
    ERR_SERVER_CENTER_CONN_ROOM_SIG_GROUPID         = -102041,  ///< 跨房间连麦签名不匹配
    ERR_SERVER_CENTER_CONN_ROOM_NOT_CONNED          = -102042,  ///< 本房间无连麦
    ERR_SERVER_CENTER_CONN_ROOM_USER_NOT_CONNED     = -102043,  ///< 本用户未发起连麦
    ERR_SERVER_CENTER_CONN_ROOM_FAILED              = -102044,  ///< 跨房间连麦失败
    ERR_SERVER_CENTER_CONN_ROOM_CANCEL_FAILED       = -102045,  ///< 取消跨房间连麦失败
    ERR_SERVER_CENTER_CONN_ROOM_CONNED_ROOM_NOT_EXIST   = -102046,  ///< 被连麦房间不存在
    ERR_SERVER_CENTER_CONN_ROOM_CONNED_REACH_MAX_ROOM   = -102047,  ///< 被连麦房间达到连麦上限
    ERR_SERVER_CENTER_CONN_ROOM_CONNED_USER_NOT_EXIST   = -102048,  ///< 被连麦用户不存在
    ERR_SERVER_CENTER_CONN_ROOM_CONNED_USER_DELETED     = -102049,  ///< 被连麦用户已被删除
    ERR_SERVER_CENTER_CONN_ROOM_CONNED_USER_FULL        = -102050,  ///< 被连麦用户达到资源上限
    ERR_SERVER_CENTER_CONN_ROOM_INVALID_SEQ             = -102051,  ///< 连麦请求序号错乱
    
    
    /////////////////////////////////////////////////////////////////////////////////
    //
    //       客户无需关心的内部错误码
    //
    /////////////////////////////////////////////////////////////////////////////////

    // - Remove From Head
    ERR_RTMP_PUSH_NET_DISCONNECT                    = -1307,    ///< 直播，推流出现网络断开，且经过多次重试无法恢复
    ERR_RTMP_PUSH_INVALID_ADDRESS                   = -1313,    ///< 直播，推流地址非法，例如不是 RTMP 协议的地址
    ERR_RTMP_PUSH_NET_ALLADDRESS_FAIL               = -1324,    ///< 直播，连接推流服务器失败（若支持智能选路，IP 全部失败）
    ERR_RTMP_PUSH_NO_NETWORK                        = -1325,    ///< 直播，网络不可用，请确认 Wi-Fi、移动数据或者有线网络是否正常
    ERR_RTMP_PUSH_SERVER_REFUSE                     = -1326,    ///< 直播，服务器拒绝连接请求，可能是该推流地址已经被占用，或者 TXSecret 校验失败，或者是过期了，或者是欠费了
    
    ERR_PLAY_LIVE_STREAM_NET_DISCONNECT             = -2301,    ///< 直播，网络断连，且经多次重连抢救无效，可以放弃治疗，更多重试请自行重启播放
    ERR_GET_RTMP_ACC_URL_FAIL                       = -2302,    ///< 直播，获取加速拉流的地址失败
    ERR_FILE_NOT_FOUND                              = -2303,    ///< 播放的文件不存在
    ERR_HEVC_DECODE_FAIL                            = -2304,    ///< H265 解码失败
    ERR_VOD_DECRYPT_FAIL                            = -2305,    ///< 点播，音视频流解密失败
    ERR_GET_VODFILE_MEDIAINFO_FAIL                  = -2306,    ///< 点播，获取点播文件信息失败
    ERR_PLAY_LIVE_STREAM_SWITCH_FAIL                = -2307,    ///< 直播，切流失败（切流可以播放不同画面大小的视频）
    ERR_PLAY_LIVE_STREAM_SERVER_REFUSE              = -2308,    ///< 直播，服务器拒绝连接请求
    ERR_RTMP_ACC_FETCH_STREAM_FAIL                  = -2309,    ///< 直播，RTMPACC 低延时拉流失败，且经过多次重试无法恢复

    ERR_ROOM_HEARTBEAT_FAIL                         = -3302,    ///< 心跳失败，客户端定时向服务器发送数据包，告诉服务器自己活着，这个错误通常是发包超时
    ERR_ROOM_REQUEST_IP_FAIL                        = -3303,    ///< 拉取接口机服务器地址失败
    ERR_ROOM_CONNECT_FAIL                           = -3304,    ///< 连接接口机服务器失败
    ERR_ROOM_REQUEST_AVSEAT_FAIL                    = -3305,    ///< 请求视频位失败
    ERR_ROOM_REQUEST_TOKEN_HTTPS_TIMEOUT            = -3306,    ///< 请求 token HTTPS 超时，请检查网络是否正常，或网络防火墙是否放行 HTTPS 访问 official.opensso.tencent-cloud.com:443
    ERR_ROOM_REQUEST_IP_TIMEOUT                     = -3307,    ///< 请求 IP 和 sig 超时，请检查网络是否正常，或网络防火墙是否放行 UDP 访问下列 IP 和域名 query.tencent-cloud.com:8000 162.14.23.140:8000 162.14.7.49:8000
    ERR_ROOM_REQUEST_VIDEO_FLAG_TIMEOUT             = -3309,    ///< 请求视频位超时
    ERR_ROOM_REQUEST_VIDEO_DATA_ROOM_TIMEOUT        = -3310,    ///< 请求视频数据超时
    ERR_ROOM_REQUEST_CHANGE_ABILITY_TIMEOUT         = -3311,    ///< 请求修改视频能力项超时
    ERR_ROOM_REQUEST_STATUS_REPORT_TIMEOUT          = -3312,    ///< 请求状态上报超时
    ERR_ROOM_REQUEST_CLOSE_VIDEO_TIMEOUT            = -3313,    ///< 请求关闭视频超时
    ERR_ROOM_REQUEST_SET_RECEIVE_TIMEOUT            = -3314,    ///< 请求接收视频项超时
    ERR_ROOM_REQUEST_TOKEN_INVALID_PARAMETER        = -3315,    ///< 请求 token 无效参数，请检查 TRTCParams.userSig 是否填写正确

    ERR_ROOM_REQUEST_AES_TOKEN_RETURN_ERROR         = -3329,    ///< 请求 AES TOKEN 时，server 返回的内容是空的
    ERR_ACCIP_LIST_EMPTY                            = -3331,    ///< 请求接口机 IP 返回的列表为空的
    ERR_ROOM_REQUEST_SEND_JSON_CMD_TIMEOUT          = -3332,    ///< 请求发送Json 信令超时
    
    // Info 服务器（查询接口机 IP）, 服务器错误码，数值范围[-100000, -110000]
    ERR_SERVER_INFO_UNPACKING_ERROR                 = -100000,  ///< server 解包错误，可能请求数据被篡改
    ERR_SERVER_INFO_TOKEN_ERROR                     = -100001,  ///< TOKEN 错误
    ERR_SERVER_INFO_ALLOCATE_ACCESS_FAILED          = -100002,  ///< 分配接口机错误
    ERR_SERVER_INFO_GENERATE_SIGN_FAILED            = -100003,  ///< 生成签名错误
    ERR_SERVER_INFO_TOKEN_TIMEOUT                   = -100004,  ///< HTTPS token 超时
    ERR_SERVER_INFO_INVALID_COMMAND                 = -100005,  ///< 无效的命令字
    ERR_SERVER_INFO_PRIVILEGE_FLAG_ERROR            = -100006,  ///< 权限位校验失败
    ERR_SERVER_INFO_GENERATE_KEN_ERROR              = -100007,  ///< HTTPS 请求时，生成加密 key 错误
    ERR_SERVER_INFO_GENERATE_TOKEN_ERROR            = -100008,  ///< HTTPS 请求时，生成 token 错误
    ERR_SERVER_INFO_DATABASE                        = -100009,  ///< 数据库查询失败（房间相关存储信息）
    ERR_SERVER_INFO_BAD_ROOMID                      = -100010,  ///< 房间号错误
    ERR_SERVER_INFO_BAD_SCENE_OR_ROLE               = -100011,  ///< 场景或角色错误
    ERR_SERVER_INFO_ROOMID_EXCHANGE_FAILED          = -100012,  ///< 房间号转换出错
    ERR_SERVER_INFO_STRGROUP_HAS_INVALID_CHARS      = -100014,  ///< 房间号非法
    ERR_SERVER_INFO_LACK_SDKAPPID                   = -100015,  ///< 非法SDKAppid
    ERR_SERVER_INFO_INVALID                         = -100016,  ///< 无效请求, 分配接口机失败
    ERR_SERVER_INFO_ECDH_GET_KEY                    = -100017,  ///< 生成公钥失败
    ERR_SERVER_INFO_ECDH_GET_TINYID                 = -100018,  ///< userSig 校验失败，请检查 TRTCParams.userSig 是否填写正确
    
    // Access 接口机
    ERR_SERVER_ACC_TOKEN_TIMEOUT                    = -101000,  ///< token 过期
    ERR_SERVER_ACC_SIGN_ERROR                       = -101001,  ///< 签名错误
    ERR_SERVER_ACC_SIGN_TIMEOUT                     = -101002,  ///< 签名超时
    ERR_SERVER_ACC_ROOM_NOT_EXIST                   = -101003,  ///< 房间不存在
    ERR_SERVER_ACC_ROOMID                           = -101004,  ///< 后台房间标识 roomId 错误
    ERR_SERVER_ACC_LOCATIONID                       = -101005,  ///< 后台用户位置标识 locationId 错误
    ERR_SERVER_ACC_TOKEN_EORROR                     = -101006,  ///< token里面的tinyid和进房信令tinyid不同 或是 进房信令没有token

    // Center 服务器（信令和流控处理等任务）
    ERR_SERVER_CENTER_SYSTEM_ERROR                  = -102000,  ///< 后台错误
    
    ERR_SERVER_CENTER_INVALID_ROOMID                = -102001,  ///< 无效的房间 Id
    ERR_SERVER_CENTER_CREATE_ROOM_FAILED            = -102002,  ///< 创建房间失败
    ERR_SERVER_CENTER_SIGN_ERROR                    = -102003,  ///< 签名错误
    ERR_SERVER_CENTER_SIGN_TIMEOUT                  = -102004,  ///< 签名过期
    ERR_SERVER_CENTER_ROOM_NOT_EXIST                = -102005,  ///< 房间不存在
    ERR_SERVER_CENTER_ADD_USER_FAILED               = -102006,  ///< 房间添加用户失败
    ERR_SERVER_CENTER_FIND_USER_FAILED              = -102007,  ///< 查找用户失败
    ERR_SERVER_CENTER_SWITCH_TERMINATION_FREQUENTLY = -102008,  ///< 频繁切换终端
    ERR_SERVER_CENTER_LOCATION_NOT_EXIST            = -102009,  ///< locationid 错误
    ERR_SERVER_CENTER_NO_PRIVILEDGE_CREATE_ROOM     = -102010,  ///< 没有权限创建房间
    ERR_SERVER_CENTER_NO_PRIVILEDGE_ENTER_ROOM      = -102011,  ///< 没有权限进入房间
    ERR_SERVER_CENTER_INVALID_PARAMETER_SUB_VIDEO   = -102012,  ///< 辅路抢视频位、申请辅路请求类型参数错误
    ERR_SERVER_CENTER_NO_PRIVILEDGE_PUSH_VIDEO      = -102013,  ///< 没有权限上视频
    ERR_SERVER_CENTER_ROUTE_TABLE_ERROR             = -102014,  ///< 没有空闲路由表
    ERR_SERVER_CENTER_NOT_PUSH_SUB_VIDEO            = -102017,  ///< 当前用户没有上行辅路
    ERR_SERVER_CENTER_USER_WAS_DELETED              = -102018,  ///< 用户被删除状态
    ERR_SERVER_CENTER_NO_PRIVILEDGE_REQUEST_VIDEO   = -102019,  ///< 没有权限请求视频
    ERR_SERVER_CENTER_INVALID_PARAMETER             = -102023,  ///< 进房参数 bussInfo 错误
    ERR_SERVER_CENTER_I_FRAME_UNKNOW_TYPE           = -102024,  ///< 请求 I 帧未知 opType
    ERR_SERVER_CENTER_I_FRAME_INVALID_PACKET        = -102025,  ///< 请求 I 帧包格式错误
    ERR_SERVER_CENTER_I_FRAME_DEST_USER_NOT_EXIST   = -102026,  ///< 请求 I 帧目标用户不存在
    ERR_SERVER_CENTER_I_FRAME_ROOM_TOO_BIG          = -102027,  ///< 请求 I 帧房间用户太多
    ERR_SERVER_CENTER_I_FRAME_RPS_INVALID_PARAMETER = -102028,  ///< 请求 I 帧参数错误
    ERR_SERVER_CENTER_INVALID_ROOM_ID               = -102029,  ///< 房间号非法
    ERR_SERVER_CENTER_ROOM_ID_TOO_LONG              = -102030,  ///< 房间号超过限制
    ERR_SERVER_CENTER_ROOM_FULL                     = -102052,  ///< 房间满员
    ERR_SERVER_CENTER_DECODE_JSON_FAIL              = -102053,  ///< JSON 串解析失败
    ERR_SERVER_CENTER_UNKNOWN_SUB_CMD               = -102054,  ///< 未定义命令字
    ERR_SERVER_CENTER_INVALID_ROLE                  = -102055,  ///< 未定义角色
    ERR_SERVER_CENTER_REACH_PROXY_MAX               = -102056,  ///< 代理机超出限制
    ERR_SERVER_CENTER_RECORDID_STORE                = -102057,  ///< 无法保存用户自定义 recordId
    ERR_SERVER_CENTER_PB_SERIALIZE                  = -102058,  ///< Protobuf 序列化错误
    
    ERR_SERVER_SSO_SIG_EXPIRED                      = -70001,   ///< sig 过期，请尝试重新生成。如果是刚生成，就过期，请检查有效期填写的是否过小，或者填的 0
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_1        = -70003,   ///< sig 校验失败，请确认下 sig 内容是否被截断，如缓冲区长度不够导致的内容截断
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_2        = -70004,   ///< sig 校验失败，请确认下 sig 内容是否被截断，如缓冲区长度不够导致的内容截断
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_3        = -70005,   ///< sig 校验失败，可用工具自行验证生成的 sig 是否正确
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_4        = -70006,   ///< sig 校验失败，可用工具自行验证生成的 sig 是否正确
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_5        = -70007,   ///< sig 校验失败，可用工具自行验证生成的 sig 是否正确
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_6        = -70008,   ///< sig 校验失败，可用工具自行验证生成的 sig 是否正确
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_7        = -70009,   ///< 用业务公钥验证 sig 失败，请确认生成的 usersig 使用的私钥和 sdkAppId 是否对应
    ERR_SERVER_SSO_SIG_VERIFICATION_FAILED_8        = -70010,   ///< sig 校验失败，可用工具自行验证生成的 sig 是否正确
    ERR_SERVER_SSO_SIG_VERIFICATION_ID_NOT_MATCH    = -70013,   ///< sig 中 identifier 与请求时的 identifier 不匹配，请检查登录时填写的 identifier 与 sig 中的是否一致
    ERR_SERVER_SSO_APPID_NOT_MATCH                  = -70014,   ///< sig 中 sdkAppId 与请求时的 sdkAppId 不匹配，请检查登录时填写的 sdkAppId 与 sig 中的是否一致
    ERR_SERVER_SSO_VERIFICATION_EXPIRED             = -70017,   ///< 内部第三方票据验证超时，请重试，如多次重试不成功，请@TLS 帐号支持，QQ 3268519604
    ERR_SERVER_SSO_VERIFICATION_FAILED              = -70018,   ///< 内部第三方票据验证超时，请重试，如多次重试不成功，请@TLS 帐号支持，QQ 3268519604

    ERR_SERVER_SSO_APPID_NOT_FOUND                  = -70020,   ///< sdkAppId 未找到，请确认是否已经在腾讯云上配置
    ERR_SERVER_SSO_ACCOUNT_IN_BLACKLIST             = -70051,   ///< 帐号已被拉入黑名单，请联系 TLS 帐号支持 QQ 3268519604
    ERR_SERVER_SSO_SIG_INVALID                      = -70052,   ///< usersig 已经失效，请重新生成，再次尝试
    ERR_SERVER_SSO_LIMITED_BY_SECURITY              = -70114,   ///< 安全原因被限制
    ERR_SERVER_SSO_INVALID_LOGIN_STATUS             = -70221,   ///< 登录状态无效，请使用 usersig 重新鉴权
    ERR_SERVER_SSO_APPID_ERROR                      = -70252,   ///< sdkAppId 填写错误
    ERR_SERVER_SSO_TICKET_VERIFICATION_FAILED       = -70346,   ///< 票据校验失败，请检查各项参数是否正确
    ERR_SERVER_SSO_TICKET_EXPIRED                   = -70347,   ///< 票据因过期原因校验失败
    ERR_SERVER_SSO_ACCOUNT_EXCEED_PURCHASES         = -70398,   ///< 创建账号数量超过已购买预付费数量限制
    ERR_SERVER_SSO_INTERNAL_ERROR                   = -70500,   ///< 服务器内部错误，请重试
    
    //秒级监控上报错误码
    ERR_REQUEST_QUERY_CONFIG_TIMEOUT           = -4001,    ///< 请求通用配置超时
    ERR_CUSTOM_STREAM_INVALID                  = -4002,    ///< 自定义流id错误
    ERR_USER_DEFINE_RECORD_ID_INVALID          = -4003,    ///< userDefineRecordId错误
    ERR_MIX_PARAM_INVALID                      = -4004,    ///< 混流参数校验失败
    ERR_REQUEST_ACC_BY_HOST_IP                 = -4005,    ///< 通过域名进行0x1请求
    // - /Remove From Head
} ArAVError;

/////////////////////////////////////////////////////////////////////////////////
//
//                     警告码
//
//> 不需要特别关注，但您可以根据其中某些感兴趣的警告码，对当前用户进行相应的提示
//
/////////////////////////////////////////////////////////////////////////////////

typedef enum ArAVWarning
{
    WARNING_HW_ENCODER_START_FAIL                   = 1103,     ///< 硬编码启动出现问题，自动切换到软编码
    WARNING_VIDEO_ENCODER_SW_TO_HW                  = 1107,     ///<  当前 CPU 使用率太高，无法满足软件编码需求，自动切换到硬件编码
    WARNING_INSUFFICIENT_CAPTURE_FPS                = 1108,     ///<  摄像头采集帧率不足，部分自带美颜算法的 Android 手机上会出现
    WARNING_SW_ENCODER_START_FAIL                   = 1109,     ///<  软编码启动失败
    WARNING_REDUCE_CAPTURE_RESOLUTION               = 1110,     ///<  摄像头采集分辨率被降低，以满足当前帧率和性能最优解。
    WARNING_CAMERA_DEVICE_EMPTY                     = 1111,     ///<  没有检测到可用的摄像头设备
    WARNING_CAMERA_NOT_AUTHORIZED                   = 1112,     ///<  用户未授权当前应用使用摄像头
    WARNING_MICROPHONE_DEVICE_EMPTY                 = 1201,     ///<  没有检测到可用的麦克风设备
    WARNING_SPEAKER_DEVICE_EMPTY                    = 1202,     ///<  没有检测到可用的扬声器设备
    WARNING_MICROPHONE_NOT_AUTHORIZED               = 1203,     ///<  用户未授权当前应用使用麦克风
    WARNING_MICROPHONE_DEVICE_ABNORMAL              = 1204,     ///<  音频采集设备不可用（例如被占用或者PC判定无效设备）
    WARNING_SPEAKER_DEVICE_ABNORMAL                 = 1205,     ///<  音频播放设备不可用（例如被占用或者PC判定无效设备）
    WARNING_VIDEO_FRAME_DECODE_FAIL                 = 2101,     ///<  当前视频帧解码失败
    WARNING_AUDIO_FRAME_DECODE_FAIL                 = 2102,     ///<  当前音频帧解码失败
    WARNING_VIDEO_PLAY_LAG                          = 2105,     ///<  当前视频播放出现卡顿
    WARNING_HW_DECODER_START_FAIL                   = 2106,     ///<  硬解启动失败，采用软解码
    WARNING_VIDEO_DECODER_HW_TO_SW                  = 2108,     ///<  当前流硬解第一个 I 帧失败，SDK 自动切软解
    WARNING_SW_DECODER_START_FAIL                   = 2109,     ///<  软解码器启动失败
    WARNING_VIDEO_RENDER_FAIL                       = 2110,     ///<  视频渲染失败
    WARNING_START_CAPTURE_IGNORED                   = 4000,     ///<  已经在采集，启动采集被忽略
    WARNING_AUDIO_RECORDING_WRITE_FAIL              = 7001,     ///<  音频录制写入文件失败
    WARNING_ROOM_DISCONNECT                         = 5101,     ///<  网络断开连接
    WARNING_IGNORE_UPSTREAM_FOR_AUDIENCE            = 6001,     ///<  当前是观众角色，忽略上行音视频数据
    
    // - Remove From Head
    WARNING_NET_BUSY                                = 1101,     ///< 网络状况不佳：上行带宽太小，上传数据受阻
    WARNING_RTMP_SERVER_RECONNECT                   = 1102,     ///<  直播，网络断连, 已启动自动重连（自动重连连续失败超过三次会放弃）
    WARNING_LIVE_STREAM_SERVER_RECONNECT            = 2103,     ///<  直播，网络断连, 已启动自动重连（自动重连连续失败超过三次会放弃）
    WARNING_RECV_DATA_LAG                           = 2104,     ///<  网络来包不稳：可能是下行带宽不足，或由于主播端出流不均匀
    WARNING_RTMP_DNS_FAIL                           = 3001,     ///<  直播，DNS 解析失败
    WARNING_RTMP_SEVER_CONN_FAIL                    = 3002,     ///<  直播，服务器连接失败
    WARNING_RTMP_SHAKE_FAIL                         = 3003,     ///<  直播，与 RTMP 服务器握手失败
    WARNING_RTMP_SERVER_BREAK_CONNECT               = 3004,     ///<  直播，服务器主动断开
    WARNING_RTMP_READ_WRITE_FAIL                    = 3005,     ///<  直播，RTMP 读/写失败，将会断开连接
    WARNING_RTMP_WRITE_FAIL                         = 3006,     ///<  直播，RTMP 写失败（SDK 内部错误码，不会对外抛出）
    WARNING_RTMP_READ_FAIL                          = 3007,     ///<  直播，RTMP 读失败（SDK 内部错误码，不会对外抛出）
    WARNING_RTMP_NO_DATA                            = 3008,     ///<  直播，超过30s 没有数据发送，主动断开连接
    WARNING_PLAY_LIVE_STREAM_INFO_CONNECT_FAIL      = 3009,     ///<  直播，connect 服务器调用失败（SDK 内部错误码，不会对外抛出）
    WARNING_NO_STEAM_SOURCE_FAIL                    = 3010,     ///<  直播，连接失败，该流地址无视频（SDK 内部错误码，不会对外抛出）
    WARNING_ROOM_RECONNECT                          = 5102,     ///<  网络断连，已启动自动重连
    WARNING_ROOM_NET_BUSY                           = 5103,     ///<  网络状况不佳：上行带宽太小，上传数据受阻
    // - /Remove From Head
} ArAVWarning;

// - Remove From Head
/////////////////////////////////////////////////////////////////////////////////
//
//                     （三）事件列表
//
/////////////////////////////////////////////////////////////////////////////////

typedef enum ArAVEvent
{
    EVT_RTMP_PUSH_CONNECT_SUCC                      = 1001,     ///<  直播，已经连接 RTMP 推流服务器
    EVT_RTMP_PUSH_BEGIN                             = 1002,     ///<  直播，已经与 RTMP 服务器握手完毕，开始推流
    EVT_CAMERA_START_SUCC                           = 1003,     ///<  打开摄像头成功
    EVT_SCREEN_CAPTURE_SUCC                         = 1004,     ///<  录屏启动成功
    EVT_UP_CHANGE_RESOLUTION                        = 1005,     ///<  上行动态调整分辨率
    EVT_UP_CHANGE_BITRATE                           = 1006,     ///<  码率动态调整
    EVT_FIRST_FRAME_AVAILABLE                       = 1007,     ///<  首帧画面采集完成
    EVT_START_VIDEO_ENCODER                         = 1008,     ///<  编码器启动成功
    EVT_SNAPSHOT_COMPLETE                           = 1022,     ///<  一帧截图完成
    EVT_CAMERA_REMOVED                              = 1023,     ///<  摄像头设备已被移出（Windows 和 Mac 版 SDK 使用）
    EVT_CAMERA_AVAILABLE                            = 1024,     ///<  摄像头设备重新可用（Windows 和 Mac 版 SDK 使用）
    EVT_CAMERA_CLOSE                                = 1025,     ///<  关闭摄像头完成（Windows 和 Mac 版 SDK 使用）
    EVT_RTMP_PUSH_PUBLISH_START                     = 1026,     ///<  直播，与 RTMP 服务器连接后，收到 NetStream.Publish.Start 消息，表明流发布成功（SDK 内部事件，不会对外抛出）
    EVT_HW_ENCODER_START_SUCC                       = 1027,     ///<  硬编码器启动成功
    EVT_SW_ENCODER_START_SUCC                       = 1028,     ///<  软编码器启动成功
    EVT_LOCAL_RECORD_RESULT                         = 1029,     ///<  本地录制结果
    EVT_LOCAL_RECORD_PROGRESS                       = 1030,     ///<  本地录制状态通知

    EVT_PLAY_LIVE_STREAM_CONNECT_SUCC               = 2001,     ///<  直播，已经连接 RTMP 拉流服务器
    EVT_PLAY_LIVE_STREAM_BEGIN                      = 2002,     ///<  直播，已经与 RTMP 服务器握手完毕，开始拉流
    EVT_RENDER_FIRST_I_FRAME                        = 2003,     ///<  渲染首个视频数据包（IDR）
    EVT_VIDEO_PLAY_BEGIN                            = 2004,     ///<  视频播放开始
    EVT_VIDEO_PLAY_PROGRESS                         = 2005,     ///<  视频播放进度
    EVT_VIDEO_PLAY_END                              = 2006,     ///<  视频播放结束
    EVT_VIDEO_PLAY_LOADING                          = 2007,     ///<  视频播放 loading
    EVT_START_VIDEO_DECODER                         = 2008,     ///<  解码器启动
    EVT_DOWN_CHANGE_RESOLUTION                      = 2009,     ///<  下行视频分辨率改变
    EVT_GET_VODFILE_MEDIAINFO_SUCC                  = 2010,     ///<  点播，获取点播文件信息成功
    EVT_VIDEO_CHANGE_ROTATION                       = 2011,     ///<  视频旋转角度发生改变
    EVT_PLAY_GET_MESSAGE                            = 2012,     ///<  消息事件
    EVT_VOD_PLAY_PREPARED                           = 2013,     ///<  点播，视频加载完毕
    EVT_VOD_PLAY_LOADING_END                        = 2014,     ///<  点播，loading 结束
    EVT_PLAY_LIVE_STREAM_SWITCH_SUCC                = 2015,     ///<  直播，切流成功（切流可以播放不同画面大小的视频）
    EVT_VOD_PLAY_TCP_CONNECT_SUCC                   = 2016,     ///<  点播，TCP 连接成功（SDK 内部事件，不会对外抛出）
    EVT_VOD_PLAY_FIRST_VIDEO_PACKET                 = 2017,     ///<  点播，收到首帧数据（SDK 内部事件，不会对外抛出）
    EVT_VOD_PLAY_DNS_RESOLVED                       = 2018,     ///<  点播，DNS 解析完成（SDK 内部事件，不会对外抛出）
    EVT_VOD_PLAY_SEEK_COMPLETE                      = 2019,     ///<  点播，视频播放 Seek 完成（SDK 内部事件，不会对外抛出）
    EVT_VIDEO_DECODER_CACHE_TOO_MANY_FRAMES         = 2020,     ///<  视频解码器缓存帧数过多，超过40帧（SDK 内部事件，不会对外抛出）
    EVT_HW_DECODER_START_SUCC                       = 2021,     ///<  硬解码器启动成功（SDK 内部事件，不会对外抛出）
    EVT_SW_DECODER_START_SUCC                       = 2022,     ///<  软解码器启动成功（SDK 内部事件，不会对外抛出）
    EVT_AUDIO_JITTER_STATE_FIRST_LOADING            = 2023,     ///<  音频首次加载（SDK 内部事件，不会对外抛出）
    EVT_AUDIO_JITTER_STATE_LOADING                  = 2024,     ///<  音频正在加载（SDK 内部事件，不会对外抛出）
    EVT_AUDIO_JITTER_STATE_PLAYING                  = 2025,     ///<  音频正在播放（SDK 内部事件，不会对外抛出）
    EVT_AUDIO_JITTER_STATE_FIRST_PLAY               = 2026,     ///<  音频首次播放（SDK 内部事件，不会对外抛出）
    EVT_MIC_START_SUCC                              = 2027,     ///<  麦克风启动成功
    EVT_PLAY_GET_METADATA                           = 2028,     ///<  视频流MetaData事件
    EVT_MIC_RELEASE_SUCC                            = 2029,     ///<  释放麦克风占用
    EVT_AUDIO_DEVICE_ROUTE_CHANGED                  = 2030,     ///<  音频设备的route发生改变，即当前的输入输出设备发生改变，比如耳机被拔出
    EVT_PLAY_GET_FLVSESSIONKEY                      = 2031,     ///<  ArLivePlayer 接收到http响应头中的 flvSessionKey 信息

    EVT_ROOM_ENTER                                  = 1018,     ///<  进入房间成功
    EVT_ROOM_EXIT                                   = 1019,     ///<  退出房间
    EVT_ROOM_USERLIST                               = 1020,     ///<  下发房间成员列表（不包括自己）
    EVT_ROOM_NEED_REENTER                           = 1021,     ///<  WiFi 切换到4G 会触发断线重连，此时需要重新进入房间（拉取最优的服务器地址）
    EVT_ROOM_ENTER_FAILED                           = 1022,     ///<  自己进入房间失败
    EVT_ROOM_USER_ENTER                             = 1031,     ///<  进房通知
    EVT_ROOM_USER_EXIT                              = 1032,     ///<  退房通知
    EVT_ROOM_USER_VIDEO_STATE                       = 1033,     ///<  视频状态位变化通知
    EVT_ROOM_USER_AUDIO_STATE                       = 1034,     ///<  音频状态位变化通知
    
    EVT_ROOM_REQUEST_IP_SUCC                        = 8001,     ///<  拉取接口机服务器地址成功
    EVT_ROOM_CONNECT_SUCC                           = 8002,     ///<  连接接口机服务器成功
    EVT_ROOM_REQUEST_AVSEAT_SUCC                    = 8003,     ///<  请求视频位成功
} ArAVEvent;
// - /Remove From Head

#endif /* __AR_AVCODE_H__ */
