//
//  ArPlayerViewController.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import ARLiveKit
import UIKit

class ArPlayerViewController: UIViewController {
    @IBOutlet var renderView: UIView!
    @IBOutlet var stateLabel: UILabel!
    @IBOutlet var modeLabel: UILabel!
    
    fileprivate var liveStatus: ARLivePlayStatus = .stopped
    fileprivate var renderMode: ARLiveRenderMode = .fit
    /// 拉流url
    var pullUrl: String!
    
    fileprivate let livePlayer: ARLivePlayer = {
        let player = liveEngine!.createArLivePlayer()
        return player
    }()
    
    lazy var snapImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.layer.borderWidth = 1.0
        imageView.layer.borderColor = UIColor.white.cgColor
        imageView.contentMode = .scaleAspectFit
        return imageView
    }()

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        initializePlayer()
    }
    
    func initializePlayer() {
        /// 创建拉流实例对象
        livePlayer.setDelegate(self)
        /// 设置播放器的视频渲染 View
        livePlayer.setRenderView(renderView)
        livePlayer.setRenderFill(renderMode)
        /// 设置视频播放模式
        livePlayer.setPlayMode(.live)
        
        /// 设置播放器缓存自动调整的最小和最大时间 ( 单位：秒 )
        livePlayer.setCacheParams(1.0, maxTime: 100)
        /// 开始播放音视频流
        livePlayer.startPlay(pullUrl)
    }
    
    @objc func removeSnapshot() {
        snapImageView.removeFromSuperview()
    }
    
    @IBAction func leaveChannel(_ sender: Any) {
        livePlayer.stopPlay()
        livePlayer.setRenderView(nil)
        liveEngine.release(livePlayer)
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction func snapShot(_ sender: Any) {
        if liveStatus == .playing {
            /// 截取播放过程中的视频画面
            livePlayer.snapshot()
        } else {
            let text = "Player is not playing, SnapShot failure"
            ARToast.showText(text: text, duration: 1.0)
            Logger.log(message: text, level: .error)
        }
    }
    
    @IBAction func changeRenderMode(_ sender: Any) {
        let mode: ARLiveRenderMode = (renderMode == .hidden) ? .fit : .hidden
        livePlayer.setRenderFill(mode)
        modeLabel.text = mode.description
        renderMode = mode
    }
    
    @objc private func saveImage(image: UIImage, didFinishSavingWithError error: NSError?, contextInfo: AnyObject) {
        Logger.log(message: "Save image", level: error == nil ? .info : .error)
    }
    
    override var preferredStatusBarStyle: UIStatusBarStyle {
        return .lightContent
    }
}

// MARK: - ARLivePlayDelegate

extension ArPlayerViewController: ARLivePlayDelegate {
    func onError(_ player: ARLivePlayer, code: ARLiveCode, message msg: String?, extraInfo: [AnyHashable: Any]?) {
        /// 直播播放器错误通知，播放器出现错误时，会回调该通知
        Logger.log(message: "onError code = \(code.rawValue)", level: .info)
    }
    
    func onWarning(_ player: ARLivePlayer, code: ARLiveCode, message msg: String?, extraInfo: [AnyHashable: Any]?) {
        /// 直播播放器警告通知
        Logger.log(message: "onWarning code = \(code.rawValue)", level: .info)
    }
    
    func onVideoPlayStatusUpdate(_ player: ARLivePlayer, status: ARLivePlayStatus, reason: ARLiveStatusChangeReason, extraInfo: [AnyHashable: Any]?) {
        /// 直播播放器视频状态变化通知
        Logger.log(message: "onVideoPlayStatusUpdate status = \(status.rawValue), reason = \(reason.rawValue)", level: .info)
        liveStatus = status
        stateLabel.text = "\(status.description)"
    }
    
    func onAudioPlayStatusUpdate(_ player: ARLivePlayer, status: ARLivePlayStatus, reason: ARLiveStatusChangeReason, extraInfo: [AnyHashable: Any]?) {
        /// 直播播放器音频状态变化通知
        Logger.log(message: "onAudioPlayStatusUpdate status = \(status.rawValue) reason = \(reason.rawValue)", level: .info)
    }
    
    func onPlayoutVolumeUpdate(_ player: ARLivePlayer, volume: Int) {
        /// 播放器音量大小回调
        Logger.log(message: "onPlayoutVolumeUpdate volume = \(volume)", level: .info)
    }
    
    func onStatisticsUpdate(_ player: ARLivePlayer, statistics: ARLivePlayerStatistics?) {
        /// 直播播放器统计数据回调
        if statistics != nil {
            Logger.log(message: "onStatisticsUpdate width = \(statistics!.width), height =\(statistics!.height), fps = \(statistics!.fps), videoBitrate = \(statistics!.videoBitrate), audioBitrate = \(statistics!.audioBitrate)", level: .info)
        }
    }
    
    func onSnapshotComplete(_ player: ARLivePlayer, image: UIImage) {
        /// 截图回调
        UIImageWriteToSavedPhotosAlbum(image, self, #selector(saveImage(image:didFinishSavingWithError:contextInfo:)), nil)
        
        NSObject.cancelPreviousPerformRequests(withTarget: self, selector: #selector(removeSnapshot), object: nil)
        snapImageView.image = image
        
        let imageWidth = image.size.width/2
        let imageHeight = image.size.height/2
        snapImageView.frame = CGRect(x: ARScreenWidth - imageWidth - 24, y: 150, width: imageWidth, height: imageHeight)
        view.addSubview(snapImageView)
        perform(#selector(removeSnapshot), with: nil, afterDelay: 2)
        
        Logger.log(message: "onSnapshotComplete sucess, imageWidth = \(image.size.width), imageHeight = \(image.size.height)", level: .info)
    }
    
    func onRenderVideoFrame(_ player: ARLivePlayer, frame videoFrame: ARLiveVideoFrame?) {
        /// 自定义视频渲染回调
        Logger.log(message: "onRenderVideoFrame", level: .info)
    }
    
    func onReceiveSeiMessage(_ player: ARLivePlayer, payloadType: Int32, data: Data?) {
        /// 收到 SEI 消息的回调
        Logger.log(message: "onReceiveSeiMessage payloadType = \(payloadType)", level: .info)
    }
}
