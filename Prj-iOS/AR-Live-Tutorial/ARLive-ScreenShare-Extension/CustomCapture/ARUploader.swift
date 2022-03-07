//
//  ARUploader.swift
//  ARLive-ScreenShare-Extension
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

import ARLiveKit
import CoreMedia
import CoreVideo
import Foundation
import ReplayKit

let kAppGroup = "group.anyrtc.live"
let kUserDefaultPushUrl = "KEY_DEFAULT_PUSH_URL"

class ARUploader {
    private static let videoDimension: CGSize = {
        let screenSize = UIScreen.main.currentMode!.size
        var boundingSize = CGSize(width: 540, height: 960)
        let mW = boundingSize.width / screenSize.width
        let mH = boundingSize.height / screenSize.height
        if mH < mW {
            boundingSize.width = boundingSize.height / screenSize.height * screenSize.width
        } else if mW < mH {
            boundingSize.height = boundingSize.width / screenSize.width * screenSize.height
        }
        return boundingSize
    }()
    
    private static let audioSampleRate: UInt = 48000
    private static let audioChannels: UInt = 2
    private static let liveEngine: ARLiveEngineKit = .init(delegate: nil)
    private static let userDefaults = UserDefaults(suiteName: kAppGroup)
    
    private static let liverPusher: ARLivePusher = {
        let livePusher = liveEngine.createArLivePusher()
        
        let screenSize = UIScreen.main.currentMode?.size
        let screenWidth = screenSize?.width
        let screenHeight = screenSize?.height
        /// 设置推流视频编码参数
        let videoParam = ARLiveVideoEncoderParam()
        videoParam.videoResolution = .resolution640x480
        videoParam.videoResolutionMode = .portrait
        videoParam.videoScaleMode = .fit
        livePusher.setVideoQuality(videoParam)
        livePusher.startMicrophone()
        
        /// 开启自采集
        livePusher.enableCustomAudioCapture(true)
        livePusher.enableCustomVideoCapture(true)
        /// 开始推流
        let url = userDefaults?.object(forKey: kUserDefaultPushUrl)
        if url != nil {
            livePusher.startPush(url as! String)
        }
        return livePusher
    }()
    
    static func sendVideoBuffer(_ sampleBuffer: CMSampleBuffer) {
        guard let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)
        else {
            return
        }
        
        var rotation: ARLiveRotation = .rotation0
        if let orientationAttachment = CMGetAttachment(sampleBuffer, key: RPVideoSampleOrientationKey as CFString, attachmentModeOut: nil) as? NSNumber {
            if let orientation = CGImagePropertyOrientation(rawValue: orientationAttachment.uint32Value) {
                switch orientation {
                case .up, .upMirrored: rotation = .rotation0
                case .down, .downMirrored: rotation = .rotation180
                case .left, .leftMirrored: rotation = .rotation90
                case .right, .rightMirrored: rotation = .rotation270
                default: break
                }
            }
        }
        
        // let type: OSType = CVPixelBufferGetPixelFormatType(videoFrame)
        
        let frame = ARLiveVideoFrame()
        let flags = CVPixelBufferLockFlags(rawValue: 0)
        CVPixelBufferLockBaseAddress(pixelBuffer, flags)
        
        var height: size_t = 0
        var width: size_t = 0
        var bytesPerRow: size_t = 0
        
        if CVPixelBufferIsPlanar(pixelBuffer) {
            let basePlane = 0
            CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, basePlane)
            width = CVPixelBufferGetWidthOfPlane(pixelBuffer, basePlane)
            height = CVPixelBufferGetHeightOfPlane(pixelBuffer, basePlane)
            bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, basePlane)
        } else {
            width = CVPixelBufferGetWidth(pixelBuffer)
            height = CVPixelBufferGetHeight(pixelBuffer)
            bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer)
        }
        
        frame.width = UInt(width)
        frame.height = UInt(height)
        frame.stride = UInt(bytesPerRow)
        CVPixelBufferUnlockBaseAddress(pixelBuffer, flags)
        
        frame.pixelBuffer = pixelBuffer
        frame.pixelFormat = .NV12
        frame.rotation = rotation
        liverPusher.sendCustomVideoFrame(frame)
    }
    
    static func sendAudioAppBuffer(_ sampleBuffer: CMSampleBuffer) {
        ARAudioTube.liverPusher(liverPusher, pushAudioCMSampleBuffer: sampleBuffer, resampleRate: audioSampleRate, type: .app)
    }
    
    static func sendAudioMicBuffer(_ sampleBuffer: CMSampleBuffer) {
        ARAudioTube.liverPusher(liverPusher, pushAudioCMSampleBuffer: sampleBuffer, resampleRate: audioSampleRate, type: .mic)
    }
}
