# AnyRTC-RTMP
<img src="http://123.59.68.21/download/anyrtc-default.png" width="313" alt="AnyRTC-RTMP" /> </br>
本次开源的客户端基于RTMP协议的推流拉流客户端，由我司CTO亲自操刀设计，采用跨平台架构一套代码支持Android、iOS、Windows等平台。</br>
直播涉及的流程：『采集->编码->传输->解码->播放』本项目统统包含，这不是软文，这是实实在在的商业级实战代码；无论是你新手还是老司机，我们都热烈欢迎您前来筑码。

# 项目特点
**1，商业级开源代码，高效稳定**</br>
**2，超小内存占有率，移动直播针对性极致优化，代码冗余率极低**</br>
**3，打破平台壁垒，真正的跨平台，一套核心代码支持Android、iOS、Windows等**</br>
**4，超过200+Android手机、iOS全平台适配，硬件编解码可保证99%的可用性**</br>
**5，接口极简，推流：2个   拉流：2个**</br>
**6，底层库C++核心库代码风格采用：Google code style**</br>
**7，极简内核，无需再去深扒复杂的FFMpeg代码**</br>
**8，实用主义，那些什么坑什么优化等概念请搜索相关文章**</br>

# 为什么开源？
本公司此次开源移动直播解决方案的根本目的：回馈开源社区，特别是SRS和WebRTC项目，大家也可以看到本次开源项目的框架使用了WebRTC，RTMP协议部分使用的是srs_librtmp；这两个开源项目可以说在流媒体领域给予了大家太多，所以适当的回馈是理所应当。

##编译环境
Android Studio、NDK</br>
XCode</br>
VS2015</br>

##支持的系统平台
Android 4.0及以上</br>
iOS 8.0及以上</br>
Windows 7及以上</br>

##支持的CPU架构
Android armv7 arm64</br>
iOS armv7 armv7s arm64</br>
Windows win32、x64</br>

##支持的流媒体服务端
fms, wowza, evostream, red5, crtmpserver, nginx-rtmp-module, srs及其他标准RTMP协议服务端

##支持的流媒体云服务器
[网宿](http://www.wangsucloud.com/)、[UCloud](https://www.ucloud.cn/)及其他标准RTMP协议云服务器

##直播发布特性
* H.264/AAC 组合的RTMP协议音视频流发布
* 全屏视频采集，原画质缩放
* 集优化H.264软/硬件编码器，性能强劲，兼容性极强
* 视频分辨率以及码率自选
* 支持发布中途切换前后摄像头
* 支持发布中网络自适应，根据带宽大小来自动适应分辨率以及码率，让视频更顺畅
* 支持基于GPU加速的实时美颜滤镜

##直播播放特性
* 只为RTMP/FLV协议优化的码流解析器，极短的分析时间，秒开RTMP视频流
* 支持的视频解码器:H.264
* 支持的音频解码器:AAC
* OpenGL ES视频渲染
* 支持主播停止推流后，播放端立即获取到结束状态（RTMP协议下）

##第三方库版本
libfaac		1.28</br>
libfaad2	2.7</br>
ffmpeg		3.0</br>
libyuv		newest</br>
openh264	1.6.0</br>

##商用授权
本次开源在未授权情况下不可应用于任何的闭源商业项目，具体请参照GNU License中的声明。</br>
咨询QQ:2628840833 </br>
联系电话:021-65650071</br>

##技术交流
直播技术QQ群：554714720</br>
连麦技术QQ群：580477436</br>

##捐赠
本项目不接受任何形式的捐赠，您的支持就是最大的动力。</br>

## License
AnyRTC-RTMP is available under the GNU license. See the LICENSE file for more info.

