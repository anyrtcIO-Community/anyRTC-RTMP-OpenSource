//
//  SampleHandler.swift
//  ARLive-ScreenShare-Extension
//
//  Created by 余生丶 on 2022/1/11.
//

import ARLiveKit
import ReplayKit

class SampleHandler: RPBroadcastSampleHandler {
    override func broadcastStarted(withSetupInfo setupInfo: [String: NSObject]?) {
        // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    }
    
    override func broadcastPaused() {
        // User has requested to pause the broadcast. Samples will stop being delivered.
    }
    
    override func broadcastResumed() {
        // User has requested to resume the broadcast. Samples delivery will resume.
    }
    
    override func broadcastFinished() {
        // User has requested to finish the broadcast.
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        DispatchQueue.main.async {
            switch sampleBufferType {
            case RPSampleBufferType.video:
                // Handle video sample buffer
                ARUploader.sendVideoBuffer(sampleBuffer)
            case RPSampleBufferType.audioApp:
                // Handle audio sample buffer for app audio
                ARUploader.sendAudioAppBuffer(sampleBuffer)
            case RPSampleBufferType.audioMic:
                // Handle audio sample buffer for mic audio
                ARUploader.sendAudioMicBuffer(sampleBuffer)
                break
            @unknown default:
                // Handle other sample buffer types
                fatalError("Unknown type of sample buffer")
            }
        }
    }
}
