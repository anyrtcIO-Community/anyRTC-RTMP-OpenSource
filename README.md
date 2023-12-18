## anyLive

![](https://teameeting.oss-cn-shanghai.aliyuncs.com/ar/github-source/anylive-logo.png)

anyLive 是 [anyRTC](https://www.anyrtc.io/) 开源的推拉流项目。采用跨平台架构设计（采用WebRTC(93)版本为基础框架），一套代码支持Android、iOS、Windows、Mac、Ubuntu等平台。



### 功能特性

| 类型              | 功能说明                                                     |
| ----------------- | ------------------------------------------------------------ |
| **风格**          | 统一C++核心库代码风格采用：Google code style                 |
| **框架**          | WebRTC-93                                                    |
| **协议**          | rtmp、http/https、rtsp、hls、m3u8、mkv、mp3、mp4等           |
| **布局自定义**    | SDK 和 UI 分离，可以自定义添加视频UI层                       |
| **滤镜**          | 支持基于GPUImage美颜滤镜，可自定义滤镜                       |
| **帧图**          | 视频第一帧、视频帧截图功能                                   |
| **播放**          | 单例播放、多个同时播放、视频列表滑动自动播放、列表切换详情页面无缝播放 |
| **自采集/自渲染** | 可自定义音视频采集层和渲染层，方便接入第三方美颜、美声等     |
| **推流**          | 不限制用户的推流、拉流地址                                   |
| **图片推流**      | 支持特殊场景下关闭摄像头，图片进行推流                       |
| **屏幕共享**      | 支持屏幕共享                                                 |
| **SEI**           | 支持自定义信息的发送与接收                                   |
| **音量检测**      | 支持音量大小检测提示                                         |
| **镜像**      | 支持本地预览镜像以及编码镜像                                         |
| 编解码器 | H264/H265/Opus/AAC/G.711 |

### 平台兼容

| 系统              | 编译环境            | CPU架构                |
| ----------------- | ------------------- | ---------------------- |
| Android 4.4及以上 | Android Studio、NDK | armeabi-v7a、arm64-v8a |
| iOS 9.0及以上     | Xcode14             | arm64 |
| Windows 7及以上   | VS2015,VS2017       | x86、x86-64            |

### 第三方库

- libfaac 1.28
- libfaad2 2.7
- ffmpeg 4.3
- libsrtp
- libvpx
- pffft
- rapidjson
- usrsctplib
- libyuv newest
- openh264 1.6.0

### 配套规划

- [x]  支持 P2P-CDN 播放，为用户节省开支
- [x] 加入连麦功能
- [x] 美颜美型贴纸库
- [x] 低延迟直播推拉流

### 技术支持 

anyRTC官方网址：[https://www.anyrtc.io](https://www.anyrtc.io/)
QQ技术交流群：554714720(已满)  2群：698167259
联系电话:021-65650071-816
Email:[hi@dync.cc]()

技术问题：[开发者论坛](https://bbs.anyrtc.io/)

加微信入技术群交流：

<img src="https://teameeting.oss-cn-shanghai.aliyuncs.com/ar/github-source/weixincustomer.png" style="zoom:30%;" />

### 版权声明

若本开源项目涉及到其他软件的版权，请及时联系作者进行修正。

### 捐赠

本项目不接受任何形式的捐赠，您的支持就是最大的动力。

### License

anyLive is available under the GNU license. See the LICENSE file for more info.

mailto:hi@dync.cc)
