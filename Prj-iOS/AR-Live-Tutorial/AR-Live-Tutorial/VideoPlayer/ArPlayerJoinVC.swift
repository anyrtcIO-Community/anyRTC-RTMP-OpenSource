//
//  ArPlayerJoinVC.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import UIKit

class ArPlayerJoinVC: UIViewController {
    @IBOutlet var addressTextField: UITextField!
    @IBOutlet weak var joinButton: UIButton!
    
    private let titleLabel: UILabel = {
        let label = UILabel()
        label.text = "直播拉流（播放）"
        label.font = UIFont(name: PingFang, size: 16)
        label.textColor = UIColor(hexString: "#1A1A1E")
        return label
    }()
    
    private let leftButton: UIButton = {
        let button = UIButton(type: .custom)
        button.setImage(UIImage(named: "icon_back"), for: .normal)
        return button
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
    }
    
    @objc func textFieldValueChange() {
        joinButton.backgroundColor = (addressTextField.text?.count != 0) ? UIColor(hexString: "#0241FF") : UIColor(hexString: "#869ff7")
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        view.endEditing(true)
        let playerVc = segue.destination as! ArPlayerViewController
        playerVc.pullUrl = addressTextField.text
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        return addressTextField.text?.count != 0
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
