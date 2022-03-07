//
//  ArScreenShareController.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2022/1/19.
//

import ARLiveKit
import SpriteKit
import UIKit

let kAppGroup = "group.anyrtc.live"
let preferredExtension = "com.anyrtc.Live.ScreenShare"
let kUserDefaultPushUrl = "KEY_DEFAULT_PUSH_URL"

class ArScreenShareController: UIViewController {
    @IBOutlet var renderView: UIView!
    @IBOutlet var addressLabel: UILabel!
    @IBOutlet weak var sceneView: SKView!

    fileprivate var livePusher: ARLivePusher!
    fileprivate let userDefaults = UserDefaults(suiteName: kAppGroup)
    var pushUrl: String?

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        addressLabel.text = pushUrl
        userDefaults?.setValue(pushUrl, forKey: kUserDefaultPushUrl)
        
        initializePusher()
    }

    private func initializePusher() {
        /// 实例化推流对象
        livePusher = liveEngine!.createArLivePusher()
#if true
        initializeGameScene()
#else
        sceneView.removeFromSuperview()
        livePusher.startCamera(true)
        livePusher.startMicrophone()
        /// 设置本地摄像头预览 View
        livePusher.setupCameraRender(renderView)
        livePusher.setRenderFill(.fill)
#endif
    }
    
    private func initializeGameScene() {
        sceneView.showsFPS = true
        sceneView.showsNodeCount = true
        let scene = GameScene.init(size: sceneView.bounds.size)
        sceneView.presentScene(scene)
    }

    @IBAction func didClickShareScreenButton(_ sender: Any) {
        livePusher.startScreenCapture(preferredExtension)
    }

    @IBAction func leaveChannel(_ sender: Any) {
        livePusher.setupCameraRender(nil)
        liveEngine.release(livePusher)
        navigationController?.popViewController(animated: true)
    }
}
