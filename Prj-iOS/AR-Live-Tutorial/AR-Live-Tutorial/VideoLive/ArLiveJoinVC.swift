//
//  ArLiveJoinVC.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import UIKit
import ARLiveKit

enum ArPushType {
    case camera, screen
    
    func getIndex() -> Int {
        var index: NSInteger
        
        switch self {
        case .camera: index = 0
        case .screen: index = 1
        }
        return index
    }
}

class ArLiveJoinVC: UIViewController {
    /// 推流url textField
    @IBOutlet var addressTextField: UITextField!
    @IBOutlet var stackView0: UIStackView!
    @IBOutlet var stackView1: UIStackView!
    @IBOutlet weak var joinButton: UIButton!
    
    /// 分辨率
    var resolution: ARLiveVideoResolution!
    /// 推流类型
    var type: ArPushType = .camera
    
    private let titleLabel: UILabel = {
        let label = UILabel()
        label.text = "直播推流"
        label.font = UIFont(name: PingFang, size: 16)
        label.textColor = UIColor(hexString: "#1A1A1E")
        return label
    }()
    
    private let leftButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setImage(UIImage(named: "icon_back"), for: .normal)
        return button
    }()
    
    lazy var videoResolutions: [ARLiveVideoResolution] = {
        return [.resolution1280x720, .resolution960x540, .resolution480x360]
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        navigationController?.interactivePopGestureRecognizer?.isEnabled = false
        addressTextField.leftViewMode = .always
        addressTextField.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 8, height: 0))
        addressTextField.addTarget(self, action: #selector(textFieldValueChange), for: .editingChanged)

        navigationItem.titleView = titleLabel
        leftButton.addTarget(self, action: #selector(popBack), for: .touchUpInside)
        navigationItem.leftBarButtonItem = UIBarButtonItem(customView: leftButton)
        resolution = videoResolutions[1]
    }
    
    @IBAction func didClickResolutionButton(_ sender: UIButton) {
        /// resolution
        let index = sender.tag - 1
        if resolution != videoResolutions[index] {
            stackView0.subviews.forEach { subView in
                let button = subView as! UIButton
                button.isSelected = (button == sender)
                button.layer.borderColor = (button == sender) ? UIColor(hexString: "#0241FF").cgColor : UIColor(hexString: "#878799").cgColor
            }
            resolution = videoResolutions[index]
        }
    }
    
    @IBAction func didClickPushTypeButton(_ sender: UIButton) {
        /// camera、screen
        if type.getIndex() != sender.tag - 1 {
            type = (type == .camera) ? .screen : .camera
            
            stackView1.subviews.forEach { subView in
                let button = subView as! UIButton
                button.isSelected = (button == sender)
                button.layer.borderColor = (button == sender) ? UIColor(hexString: "#0241FF").cgColor : UIColor(hexString: "#878799").cgColor
            }
        }
    }
    
    @objc func textFieldValueChange() {
        joinButton.backgroundColor = (addressTextField.text?.count != 0) ? UIColor(hexString: "#0241FF") : UIColor(hexString: "#869ff7")
    }
    
    
    @IBAction func didClickJoinButton(_ sender: Any) {
        view.endEditing(true)
        
        if addressTextField.text?.count != 0 {
            let storyBoard = UIStoryboard(name: "Main", bundle: nil)
            
            if type == .camera {
                guard let liveVc = storyBoard.instantiateViewController(withIdentifier: "ARLive_Push") as? ArLiveViewController else { return }

                liveVc.pushUrl = addressTextField.text
                liveVc.resolution = resolution
                navigationController?.pushViewController(liveVc, animated: true)
            } else {
                guard let screenVc = storyBoard.instantiateViewController(withIdentifier: "ARLive_Screen") as? ArScreenShareController else {return}
                screenVc.pushUrl = addressTextField.text
                
                navigationController?.pushViewController(screenVc, animated: true)
            }
        }
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.setNavigationBarHidden(false, animated: true)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        navigationController?.setNavigationBarHidden(true, animated: true)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
}
