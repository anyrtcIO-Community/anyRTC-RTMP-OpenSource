//
//  ARMainViewController.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import ARLiveKit
import UIKit

struct MenuItem {
    var imageName: String
    var title: String
    var subTitle: String
    var subImageName: String?
}

var liveEngine: ARLiveEngineKit!

class ARMainViewController: UITableViewController {
    var menus = [
        [MenuItem(imageName: "icon_push", title: "直播推流", subTitle: "采用WebRTC底层架构，支持RTMP/HLS/HTTP-FLV")],
        [MenuItem(imageName: "icon_pull", title: "直播拉流（播放）", subTitle: "低功直播播放器，支持软硬解切换，横竖切换、低延迟等")],
        [MenuItem(imageName: "icon_video", title: "小视频播放", subTitle: "支持首屏秒开、清晰度无缝切换、码率自适应等多种特性")]
    ]

    let identifier = "ARLiveMainCell"
    lazy var identifierArr: [String] = {
        ["Live_JoinVC", "Player_JoinVC", "Video_JoinVC"]
    }()

    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem
        let label = UILabel(frame: CGRect(x: (ARScreenWidth - 150) / 2, y: ARScreenHeight - 110, width: 150, height: 20))

        label.textColor = UIColor(hexString: "#C4C4CE")
        label.font = UIFont(name: PingFang, size: 12)
        label.textAlignment = .center
        label.text = "Power by anyRTC"
        view.addSubview(label)

        liveEngine = ARLiveEngineKit(delegate: nil)
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.setNavigationBarHidden(true, animated: true)
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        return menus.count
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // #warning Incomplete implementation, return the number of rows
        return menus[section].count
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell: ARMainCell = tableView.dequeueReusableCell(withIdentifier: identifier, for: indexPath) as! ARMainCell

        // Configure the cell...
        let menuItem = menus[indexPath.section][indexPath.row]
        cell.mainImageView.image = UIImage(named: menuItem.imageName)
        cell.mainLabel.text = menuItem.title
        cell.subLabel.text = menuItem.subTitle
        cell.expectedImageView.isHidden = (indexPath.section != 2)
        return cell
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if indexPath.section != 2 {
            guard let vc = storyboard?.instantiateViewController(withIdentifier: identifierArr[indexPath.section]) else { return }
            navigationController?.pushViewController(vc, animated: true)
        } else {
            ARToast.showText(text: " Please look forward!", duration: 1.0)
        }
    }
}

class ARMainCell: UITableViewCell {
    @IBOutlet var mainImageView: UIImageView!
    @IBOutlet var mainLabel: UILabel!
    @IBOutlet var subLabel: UILabel!
    @IBOutlet weak var expectedImageView: UIImageView!
    
    override var frame: CGRect {
        get {
            return super.frame
        }
        set {
            var frame = newValue
            frame.origin.x += 16
            frame.size.width -= 2 * 16
            super.frame = frame
        }
    }
}
