## Run

### 开发环境
- 开发工具：Xcode13
- 运行环境：iOS 11.0 以上真机运行
- 开发语言：Objective-C、Swift

### 编译
1. 使用 Xcode 打开Live_All.xcworkspace，真机运行；
2. 将以下两个.cpp文件改为.mm
```
anyRTC-RTMP-OpenSource/ArLiveLite/ArLive2Engine.cpp
anyRTC-RTMP-OpenSource/ArLiveLite/PlatformImpl.cpp
```
**如下图所示：**
![icon_cpp](https://img-blog.csdnimg.cn/4e142972ba9844b39a79cd71f31bbaa3.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5L2Z55Sf5Li2OTE=,size_20,color_FFFFFF,t_70,g_se,x_16#pic_center)

3. 选择证书，选择真机设备，编译即可运行。

### CocoaPods

```
# Uncomment the next line to define a global platform for your project
platform :ios,'11.0'

target ‘Your App Name’ do
  use_frameworks!
  pod 'ARLiveKit’
end
```

### anyLive 体验地址

[点击下载](https://www.pgyer.com/gqfR)
