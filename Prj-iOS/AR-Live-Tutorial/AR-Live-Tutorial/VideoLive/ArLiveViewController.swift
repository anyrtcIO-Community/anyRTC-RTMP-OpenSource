//
//  ArLiveViewController.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import ARLiveKit
import UIKit

class ArLiveViewController: UIViewController {
    @IBOutlet var addressLabel: UILabel!
    @IBOutlet var renderView: UIView!
    @IBOutlet var stateLabel: UILabel!
    @IBOutlet var resolutionButton: UIButton!
    @IBOutlet weak var videoButton: UIButton!
    
    var resolution: ARLiveVideoResolution?
    var pushUrl: String!
    
    fileprivate let livePusher: ARLivePusher = {
        let pusher = liveEngine!.createArLivePusher()
        return pusher
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        addressLabel.text = pushUrl
        resolutionButton.setTitle(resolution?.description, for: .normal)
        
        initializePusher()
    }
    
    func initializePusher() {
        /// 实例化推流对象
        livePusher.setDelegate(self)
        /// 设置推流视频编码参数
        let param = ARLiveVideoEncoderParam(resolution!)
        livePusher.setVideoQuality(param)
        
        livePusher.startCamera(true)
        livePusher.startMicrophone()
        /// 设置本地摄像头预览 View
        livePusher.setupCameraRender(renderView)
        livePusher.setRenderFill(.fill)
        /// 开始推流
        livePusher.startPush(pushUrl)
        livePusher.setEncoderMirror(true)
    }
    
    @IBAction func switchCamera(_ sender: UIButton) {
        sender.isSelected.toggle()
        guard videoButton.isSelected else {
            return
        }
        livePusher.startCamera(sender.isSelected)
    }
    
    @IBAction func setBeautyEnable(_ sender: UIButton) {
        sender.isSelected.toggle()
        /// beauty
        livePusher.setBeautyEffect(sender.isSelected)
    }
    
    @IBAction func setMicrophoneEnable(_ sender: UIButton) {
        sender.isSelected.toggle()
        if sender.isSelected {
            livePusher.startMicrophone()
        } else {
            livePusher.stopMicrophone()
        }
    }
    
    @IBAction func setCameraEnable(_ sender: UIButton) {
        sender.isSelected.toggle()
        /// camera
        if sender.isSelected {
            livePusher.startCamera(true)
        } else {
            livePusher.stopCamera()
        }
    }
    
    @IBAction func setCameraMirror(_ sender: UIButton) {
        sender.isSelected.toggle()
        /// local mirror
        //livePusher.setCameraRenderMirror(sender.isSelected ? .enabled : .disabled)
        /// remote mirror
        livePusher.setEncoderMirror(sender.isSelected)
    }
    
    @IBAction func leaveChannel(_ sender: UIButton) {
        livePusher.stopPush()
        livePusher.setupCameraRender(nil)
        liveEngine.release(livePusher)
        navigationController?.popViewController(animated: true)
    }

    deinit {
        Logger.log(message: "pushVc deinit", level: .info)
    }
}

// MARK: - ARLivePushDelegate

extension ArLiveViewController: ARLivePushDelegate {
    func onError(_ code: ARLiveCode, message msg: String?, extraInfo: [AnyHashable: Any]?) {
        /// 直播推流器错误通知，推流器出现错误时，会回调该通知
        Logger.log(message: "onError \(code.rawValue)", level: .error)
        if code == .ERROR_INVALID_PARAMETER {
            ARToast.showText(text: "直播推流地址不合法", duration: 2.0)
        }
    }
    
    func onWarning(_ code: ARLiveCode, message msg: String?, extraInfo: [AnyHashable: Any]?) {
        /// 直播推流器警告通知
        Logger.log(message: "onWarning \(code.rawValue)", level: .warning)
    }
    
    func onCaptureFirstAudioFrame() {
        /// 首帧音频采集完成的回调通知
        Logger.log(message: "onCaptureFirstAudioFrame", level: .info)
    }
    
    func onCaptureFirstVideoFrame() {
        /// 首帧视频采集完成的回调通知
        Logger.log(message: "onCaptureFirstVideoFrame", level: .info)
    }
    
    func onMicrophoneVolumeUpdate(_ volume: Int) {
        /// 麦克风采集音量值回调
        Logger.log(message: "onMicrophoneVolumeUpdate volume = \(volume)", level: .info)
    }
    
    func onPushStatusUpdate(_ status: ARLivePushStatus, message msg: String?, extraInfo: [AnyHashable: Any]?) {
        /// 推流器连接状态回调通知
        Logger.log(message: "onPushStatusUpdate status = \(status.rawValue)", level: .info)
        stateLabel.text = "\(status.description)"
    }
    
    func onStatisticsUpdate(_ statistics: ARLivePusherStatistics) {
        /// 直播推流器统计数据回调
        // Logger.log(message: "onStatisticsUpdate width = \(statistics.width), height = \(statistics.height), fps = \(statistics.fps), videoBitrate = \(statistics.videoBitrate), audioBitrate = \(statistics.audioBitrate)", level: .info)
    }
    
    func onSnapshotComplete(_ image: UIImage) {
        /// 截图回调
        Logger.log(message: "onSnapshotComplete", level: .info)
    }
}
