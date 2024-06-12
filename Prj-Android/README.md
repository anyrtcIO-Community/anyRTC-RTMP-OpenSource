<p align="left">
<a href="https://chromium.googlesource.com/external/webrtc/+/branch-heads/4515"><img src="https://img.shields.io/badge/libwebrtc-m93.4577-blue.svg"/></a>
<img src="https://img.shields.io/badge/ffmpeg-4.2-brightgreen"/>
<img src="https://img.shields.io/badge/minSdk-19-orange"/>
<img src="https://img.shields.io/badge/NDK-20.0.5594570-lightgrey">
</p>

## 快速体验🔜

点击下载[Apk](https://www.pgyer.com/qxPXQJiY)

<img src="https://www.pgyer.com/app/qrcode/qxPXQJiY">


## Run

下载本工程，使用 AndroidStudio 打开之前，需要下载 webRTC，ffmpeg库。

下载地址：👉[Lib](https://storage.agrtc.cn:1000/share/0v2et4RX)

下载解压缩后将lib文件夹移至：**liveplayer/src/main/cpp** 目录下即可

## ⚠️

请使用 **NDK 版本：20.0.5594570** 编译，否则可能会出现各种错误🙅‍

```cpp
ld: error: duplicate symbol: webrtc::CreatePeerConnectionFactory
  >>> defined at create_peerconnection_factory.cc:41 (../../api/create_peerconnection_factory.cc:41)
  >>>            create_peerconnection_factory.o:
  >>> defined at create_peerconnection_factory.cc:41 (../../api/create_peerconnection_factory.cc:41)
  >>>            create_peerconnection_factory.o:
  
```

