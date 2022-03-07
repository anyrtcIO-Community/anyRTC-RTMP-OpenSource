//
//  ARExtension.swift
//  AR-Voice-Tutorial-iOS
//
//  Created by 余生丶 on 2020/9/2.
//  Copyright © 2020 AR. All rights reserved.
//

import AVFoundation
import UIKit

let ARScreenHeight = UIScreen.main.bounds.size.height
let ARScreenWidth = UIScreen.main.bounds.size.width

let PingFang = "PingFang SC"
let PingFangBold = "PingFangSC-Semibold"
let ApplicationKeyWindow = UIApplication.shared.keyWindow
let ApplicationDelegate: AppDelegate = UIApplication.shared.delegate as! AppDelegate

public typealias CallBackNormal = () -> Void
public typealias CallBackWithParams<T> = (_ params: T) -> Void

extension NSObject {
    // RGBA转换
    func RGBA(r: CGFloat, g: CGFloat, b: CGFloat, a: CGFloat) -> UIColor {
        //
        return UIColor(red: r/225.0, green: g/225.0, blue: b/225.0, alpha: a)
    }

    func randomCharacter(length: NSInteger) -> String {
        var randomStr = ""
        for _ in 1 ... length {
            let num = 65 + arc4random() % 25 // 随机6位大写字母
            let randomCharacter = Character(UnicodeScalar(num)!)
            randomStr.append(randomCharacter)
        }
        return randomStr
    }

    func generateRandomNumber(num: Int) -> Int {
        var place = 1
        var finalNumber = 0

        for _ in 0 ..< num {
            place *= 10
            let randomNumber = arc4random_uniform(10)
            finalNumber += Int(randomNumber) * place
        }
        return finalNumber
    }

    // 更改状态栏背景颜色
    func changeStatusBarBackColor(color: UIColor!) {
        if #available(iOS 13.0, *) {
            let statusBar = UIView(frame: UIApplication.shared.keyWindow?.windowScene?.statusBarManager?.statusBarFrame ?? CGRect(x: 0, y: 0, width: ARScreenWidth, height: 20))
            statusBar.backgroundColor = color
            UIApplication.shared.keyWindow?.addSubview(statusBar)

        } else {
            // Fallback on earlier versions
            let statusBarWindow: UIView = UIApplication.shared.value(forKey: "statusBarWindow") as! UIView
            let statusBar: UIView = statusBarWindow.value(forKey: "statusBar") as! UIView
            if statusBar.responds(to: #selector(setter: UIView.backgroundColor)) {
                statusBar.backgroundColor = color
            }
        }
    }

    // json转字典
    func getDictionaryFromJSONString(jsonString: String) -> NSDictionary {
        let jsonData: Data = jsonString.data(using: .utf8)!

        let dict = try? JSONSerialization.jsonObject(with: jsonData, options: .mutableContainers)
        if dict != nil {
            return dict as! NSDictionary
        }
        return NSDictionary()
    }

    // 字典转json
    func getJSONStringFromDictionary(dictionary: NSDictionary) -> String {
        var result: String = ""
        do {
            // 如果设置options为JSONSerialization.WritingOptions.prettyPrinted，则打印格式更好阅读
            let jsonData = try JSONSerialization.data(withJSONObject: dictionary, options: JSONSerialization.WritingOptions(rawValue: 0))

            if let JSONString = String(data: jsonData, encoding: String.Encoding.utf8) {
                result = JSONString
            }

        } catch {
            result = ""
        }
        return result
    }

    // JSONString转换为数组
    func getArrayFromJSONString(jsonString: String) -> NSArray {
        let jsonData: Data = jsonString.data(using: .utf8)!

        let array = try? JSONSerialization.jsonObject(with: jsonData, options: .mutableContainers)
        if array != nil {
            return array as! NSArray
        }
        return array as! NSArray
    }

    // 数组转json
    func getJSONStringFromArray(array: NSArray) -> String {
        if !JSONSerialization.isValidJSONObject(array) {
            print("无法解析出JSONString")
            return ""
        }

        let data: NSData! = try? JSONSerialization.data(withJSONObject: array, options: []) as NSData?
        let JSONString = NSString(data: data as Data, encoding: String.Encoding.utf8.rawValue)
        return JSONString! as String
    }

    // 获取当前时间戳
    func getLocalDateTime() -> String {
        let date = Date()
        let timeFormatter = DateFormatter()
        // "yyy-MM-dd' at 'HH:mm:ss.SSS"
        timeFormatter.dateFormat = "yyy-MM-dd' 'HH:mm"
        let localTime = timeFormatter.string(from: date) as String
        return localTime
    }

    func isFullScreen() -> Bool {
        if #available(iOS 11, *) {
            guard let w = UIApplication.shared.delegate?.window, let unwrapedWindow = w else {
                return false
            }

            if unwrapedWindow.safeAreaInsets.left > 0 || unwrapedWindow.safeAreaInsets.bottom > 0 {
                print(unwrapedWindow.safeAreaInsets)
                return true
            }
        }
        return false
    }

    // 富文本
    func getAttributedString(text: String, image: UIImage, index: NSInteger) -> NSMutableAttributedString {
        if text.isEmpty {
            return NSMutableAttributedString()
        }

        let attri = NSMutableAttributedString(string: text)
        let attch = NSTextAttachment()
        attch.image = image
        attch.bounds = CGRect(x: 3, y: -3, width: 15, height: 15)

        let attrString = NSAttributedString(attachment: attch)
        attri.insert(attrString, at: index)
        return attri
    }

    // 颜色
    public func changeFontColor(totalString: String, subString: String, font: UIFont, textColor: UIColor) -> NSMutableAttributedString {
        let attStr = NSMutableAttributedString(string: totalString)
        attStr.addAttributes([NSAttributedString.Key.foregroundColor: textColor, NSAttributedString.Key.font: font], range: NSRange(location: 0, length: subString.count))
        return attStr
    }

    // 颜色生成图片
    func createImage(_ color: UIColor) -> UIImage {
        let rect = CGRect(x: 0.0, y: 0.0, width: 1.0, height: 1.0)
        UIGraphicsBeginImageContext(rect.size)
        let context = UIGraphicsGetCurrentContext()
        context?.setFillColor(color.cgColor)
        context?.fill(rect)
        let image = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        return image!
    }

    // 是否为空
    func isBlank(text: String?) -> Bool {
        if text == nil {
            return true
        }
        return text!.isEmpty
    }

    // 取值
    func getAttributeValue(text: String?) -> String! {
        if text == nil || self.isBlank(text: text) {
            return ""
        }
        return text
    }

    // 创建or获取 录音地址
    func creatRecordPath() -> String {
        let manager = FileManager.default
        let baseUrl = NSHomeDirectory() + "/Library/Caches/Record/"
        let exist = manager.fileExists(atPath: baseUrl)
        if !exist {
            do {
                try manager.createDirectory(atPath: baseUrl, withIntermediateDirectories: true, attributes: nil)
                print("Succes to create folder")
            } catch {
                print("Error to create folder")
            }
        }
        return baseUrl
    }

    // 检测是否为 有线耳机
    func isWiredHeadset() -> Bool {
        let route = AVAudioSession.sharedInstance().currentRoute
        for desc in route.outputs {
            if desc.portType == .headphones {
                return true
            }
        }
        return false
    }

    // 判断字符串是否全是空格 true 全是空格
    func stringAllIsEmpty(string: String) -> Bool {
        let trimmedStr = string.trimmingCharacters(in: .whitespacesAndNewlines)
        return trimmedStr.isEmpty
    }

    // 计算 label 宽度
    func getLableWidth(labelText: String, font: UIFont, height: CGFloat) -> CGFloat {
        let size = CGSize(width: 900, height: height)
        let dic = NSDictionary(object: font, forKey: NSAttributedString.Key.font as NSCopying)
        let strSize = labelText.boundingRect(with: size, options: .usesLineFragmentOrigin, attributes: dic as? [NSAttributedString.Key: Any], context: nil).size
        return strSize.width
    }

    // 获取分辨率
    func getVideoDimensions(index: NSInteger) -> CGSize {
        switch index {
        case 1: return CGSize(width: 360, height: 640)
        case 2: return CGSize(width: 540, height: 960)
        case 3: return CGSize(width: 720, height: 1280)
        default:
            return CGSize.zero
        }
    }

    // 字符串是否为数字 or 字母组合
    func isLetterWithDigital(_ string: String) -> Bool {
        let numberRegex = NSPredicate(format: "SELF MATCHES %@", "^.*[0-9]+.*$")
        let letterRegex = NSPredicate(format: "SELF MATCHES %@", "^.*[A-Za-z]+.*$")
        return numberRegex.evaluate(with: string) || letterRegex.evaluate(with: string)
    }
}

var gloabWindow: UIWindow?
var toastLabel: ARMarginLabel?

class ARMarginLabel: UILabel {
    var contentInset: UIEdgeInsets = .zero

    override func textRect(forBounds bounds: CGRect, limitedToNumberOfLines numberOfLines: Int) -> CGRect {
        var rect: CGRect = super.textRect(forBounds: bounds.inset(by: self.contentInset), limitedToNumberOfLines: numberOfLines)
        rect.origin.x -= self.contentInset.left
        rect.origin.y -= self.contentInset.top
        rect.size.width += self.contentInset.left + self.contentInset.right
        rect.size.height += self.contentInset.top + self.contentInset.bottom
        return rect
    }

    override func drawText(in rect: CGRect) {
        super.drawText(in: rect.inset(by: self.contentInset))
    }
}

extension UIViewController {
    func getGloabWindow() -> UIWindow! {
        // 自定义window
        if gloabWindow == nil {
            gloabWindow = UIWindow(frame: UIScreen.main.bounds)
            let currentKeyWindow = UIApplication.shared.keyWindow
            gloabWindow?.backgroundColor = UIColor.clear
            gloabWindow?.isHidden = false
            gloabWindow?.makeKeyAndVisible()
            gloabWindow?.windowLevel = .normal
            currentKeyWindow?.makeKey()
        }
        return gloabWindow
    }

    func dismissGloabWindow() {
        if gloabWindow != nil {
            gloabWindow?.removeFromSuperview()
            gloabWindow = nil
        }
    }

    func topViewController() -> UIViewController {
        var resultVc: UIViewController
        resultVc = self.topViewController(vc: UIApplication.shared.keyWindow!.rootViewController!)!
        while (resultVc.presentedViewController) != nil {
            resultVc = self.topViewController(vc: resultVc.presentedViewController!)!
        }
        return resultVc
    }

    func topViewController(vc: UIViewController) -> UIViewController? {
        if vc is UINavigationController {
            let navVc: UINavigationController! = (vc as! UINavigationController)
            return self.topViewController(vc: navVc.topViewController!)
        } else if vc is UITabBarController {
            let tabBarVc: UITabBarController! = (vc as! UITabBarController)
            return self.topViewController(vc: tabBarVc.selectedViewController!)
        } else {
            return vc
        }
    }
}

extension CALayer {
    @objc var borderColorFromUIColor: UIColor {
        get {
            return UIColor(cgColor: self.borderColor!)
        } set {
            self.borderColor = newValue.cgColor
        }
    }
}

extension UIView {
    // x position
    var x: CGFloat {
        get {
            return frame.origin.x
        }

        set(newVal) {
            var tempFrame: CGRect = frame
            tempFrame.origin.x = newVal
            frame = tempFrame
        }
    }

    // y position
    var y: CGFloat {
        get {
            return frame.origin.y
        }

        set(newVal) {
            var tempFrame: CGRect = frame
            tempFrame.origin.y = newVal
            frame = tempFrame
        }
    }

    // height
    var height: CGFloat {
        get {
            return frame.size.height
        }

        set(newVal) {
            var tmpFrame: CGRect = frame
            tmpFrame.size.height = newVal
            frame = tmpFrame
        }
    }

    // width
    var width: CGFloat {
        get {
            return frame.size.width
        }

        set(newVal) {
            var tmpFrame: CGRect = frame
            tmpFrame.size.width = newVal
            frame = tmpFrame
        }
    }

    // left
    var left: CGFloat {
        get {
            return self.x
        }

        set(newVal) {
            self.x = newVal
        }
    }

    // right
    var right: CGFloat {
        get {
            return self.x + self.width
        }

        set(newVal) {
            self.x = newVal - self.width
        }
    }

    // top
    var top: CGFloat {
        get {
            return self.y
        }

        set(newVal) {
            self.y = newVal
        }
    }

    // bottom
    var bottom: CGFloat {
        get {
            return self.y + self.height
        }

        set(newVal) {
            self.y = newVal - self.height
        }
    }

    // centerX
    var centerX: CGFloat {
        get {
            return center.x
        }

        set(newVal) {
            center = CGPoint(x: newVal, y: center.y)
        }
    }

    // centerY
    var centerY: CGFloat {
        get {
            return center.y
        }

        set(newVal) {
            center = CGPoint(x: center.x, y: newVal)
        }
    }

    // middleX
    var middleX: CGFloat {
        return self.width/2
    }

    // middleY
    var middleY: CGFloat {
        return self.height/2
    }

    // middlePoint
    var middlePoint: CGPoint {
        return CGPoint(x: self.middleX, y: self.middleY)
    }
}

// MARK: - 按钮扩展

enum EdgeInsetsStyle: Int {
    case Top, Left, Bottom, Right
}

extension UIButton {
    // 按钮文字图片显示
    func layoutButtonWithEdgeInsetsStyle(style: EdgeInsetsStyle, space: CGFloat) {
        let imageWith: CGFloat = self.imageView!.frame.size.width
        let imageHeight: CGFloat = self.imageView!.frame.size.height

        var labelWidth: CGFloat = 0.0
        var labelHeight: CGFloat = 0.0
        if #available(iOS 8.0, *) {
            // 由于iOS8中titleLabel的size为0，用下面的这种设置
            labelWidth = self.titleLabel!.intrinsicContentSize.width
            labelHeight = self.titleLabel!.intrinsicContentSize.height
        } else {
            labelWidth = self.titleLabel!.frame.size.width
            labelHeight = self.titleLabel!.frame.size.height
        }

        var imageEdgeInsets = UIEdgeInsets.zero
        var labelEdgeInsets = UIEdgeInsets.zero

        switch style {
        case .Top:
            imageEdgeInsets = UIEdgeInsets(top: -labelHeight - space/2.0, left: 0, bottom: 0, right: -labelWidth)
            labelEdgeInsets = UIEdgeInsets(top: 0, left: -imageWith, bottom: -imageHeight - space/2.0, right: 0)
        case .Left:
            imageEdgeInsets = UIEdgeInsets(top: 0, left: -space/2.0, bottom: 0, right: space/2.0)
            labelEdgeInsets = UIEdgeInsets(top: 0, left: space/2.0, bottom: 0, right: -space/2.0)

        case .Bottom:
            imageEdgeInsets = UIEdgeInsets(top: 0, left: 0, bottom: -labelHeight - space/2.0, right: -labelWidth)
            labelEdgeInsets = UIEdgeInsets(top: -imageHeight - space/2.0, left: -imageWith, bottom: 0, right: 0)

        case .Right:
            imageEdgeInsets = UIEdgeInsets(top: 0, left: labelWidth + space/2.0, bottom: 0, right: -labelWidth - space/2.0)
            labelEdgeInsets = UIEdgeInsets(top: 0, left: -imageWith - space/2.0, bottom: 0, right: imageWith + space/2.0)
        }

        self.titleEdgeInsets = labelEdgeInsets
        self.imageEdgeInsets = imageEdgeInsets
    }
}

private var UIButton_badgeKey: Void?
private var UIButton_badgeBGColorKey: Void?
private var UIButton_badgeTextColorKey: Void?
private var UIButton_badgeFontKey: Void?
private var UIButton_badgePaddingKey: Void?
private var UIButton_badgeMinSizeKey: Void?
private var UIButton_badgeOriginXKey: Void?
private var UIButton_badgeOriginYKey: Void?
private var UIButton_shouldHideBadgeAtZeroKey: Void?
private var UIButton_shouldAnimateBadgeKey: Void?
private var UIButton_badgeValueKey: Void?

extension UIButton {
    // MARK: - 角标

    fileprivate var badgeLabel: UILabel? {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeKey) as? UILabel
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
        }
    }

    // MARK: - 角标

    /**
     * 角标值
     */
    var badgeValue: String? {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeValueKey) as? String
        }

        set(badgeValue) {
            objc_setAssociatedObject(self, &UIButton_badgeValueKey, badgeValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)

            if (badgeValue?.isEmpty)! || (badgeValue == "") || ((badgeValue == "0") && self.shouldHideBadgeAtZero) {
                self.removeBadge()
            } else if self.badgeLabel == nil {
                self.badgeLabel = UILabel(frame: CGRect(x: self.badgeOriginX, y: self.badgeOriginY, width: 20, height: 20))
                self.badgeLabel?.textColor = self.badgeTextColor
                self.badgeLabel?.backgroundColor = self.badgeBGColor
                self.badgeLabel?.font = self.badgeFont
                self.badgeLabel?.textAlignment = .center
                self.badgeInit()
                addSubview(self.badgeLabel!)
                self.updateBadgeValue(animated: false)
            } else {
                self.updateBadgeValue(animated: true)
            }
        }
    }

    /**
     * Badge background color
     */
    var badgeBGColor: UIColor? {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeBGColorKey) as? UIColor ?? .red
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeBGColorKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil { self.refreshBadge() }
        }
    }

    /**
     * Badge text color
     */
    var badgeTextColor: UIColor? {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeTextColorKey) as? UIColor ?? .white
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeTextColorKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil { self.refreshBadge() }
        }
    }

    /**
     * Badge font
     */
    var badgeFont: UIFont? {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeFontKey) as? UIFont ?? UIFont.systemFont(ofSize: 10)
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeFontKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil { self.refreshBadge() }
        }
    }

    /**
     *  Padding value for the badge
     */
    var badgePadding: CGFloat {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgePaddingKey) as? CGFloat ?? 6
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgePaddingKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil { self.updateBadgeFrame() }
        }
    }

    /**
     * badgeLabel 最小尺寸
     */
    var badgeMinSize: CGFloat {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeMinSizeKey) as? CGFloat ?? 8
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeMinSizeKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil { self.updateBadgeFrame() }
        }
    }

    /**
     *  badgeLabel OriginX
     */
    var badgeOriginX: CGFloat {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeOriginXKey) as? CGFloat ?? 0
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeOriginXKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil {
                self.updateBadgeFrame()
            }
        }
    }

    /**
     * badgeLabel OriginY
     */
    var badgeOriginY: CGFloat {
        get {
            return objc_getAssociatedObject(self, &UIButton_badgeOriginYKey) as? CGFloat ?? -6
        }
        set {
            objc_setAssociatedObject(self, &UIButton_badgeOriginYKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
            if self.badgeLabel != nil {
                self.updateBadgeFrame()
            }
        }
    }

    /**
     * In case of numbers, remove the badge when reaching zero
     */
    var shouldHideBadgeAtZero: Bool {
        get {
            return objc_getAssociatedObject(self, &UIButton_shouldHideBadgeAtZeroKey) as? Bool ?? true
        }
        set {
            objc_setAssociatedObject(self, &UIButton_shouldHideBadgeAtZeroKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
        }
    }

    /**
     * Badge has a bounce animation when value changes
     */
    var shouldAnimateBadge: Bool {
        get {
            return objc_getAssociatedObject(self, &UIButton_shouldAnimateBadgeKey) as? Bool ?? true
        }
        set {
            objc_setAssociatedObject(self, &UIButton_shouldAnimateBadgeKey, newValue, .OBJC_ASSOCIATION_RETAIN_NONATOMIC)
        }
    }

    fileprivate func badgeInit() {
        if let label = self.badgeLabel {
            self.badgeOriginX = self.frame.size.width - label.frame.size.width/2
        }

        self.clipsToBounds = false
    }

    fileprivate func refreshBadge() {
        guard let tempLabel = self.badgeLabel else { return }
        tempLabel.textColor = self.badgeTextColor
        tempLabel.backgroundColor = self.badgeBGColor
        tempLabel.font = self.badgeFont
    }

    fileprivate func removeBadge() {
        UIView.animate(withDuration: 0.2, animations: {
            self.badgeLabel?.transform = CGAffineTransform(scaleX: 0, y: 0)
        }) { (_: Bool) in
            self.badgeLabel?.removeFromSuperview()
            if self.badgeLabel != nil { self.badgeLabel = nil }
        }
    }

    fileprivate func updateBadgeValue(animated: Bool) {
        if animated, self.shouldAnimateBadge, !(self.badgeLabel?.text == self.badgeValue) {
            let animation = CABasicAnimation(keyPath: "transform.scale")
            animation.fromValue = 1.5
            animation.toValue = 1
            animation.duration = 0.2
            animation.timingFunction = CAMediaTimingFunction(controlPoints: 0.4, 1.3, 1.0, 1.0)
            self.badgeLabel?.layer.add(animation, forKey: "bounceAnimation")
        }

        var badgeValue = 0
        if let badgeStr = self.badgeValue, let value = Int(badgeStr) {
            badgeValue = value
        }
        self.badgeLabel?.text = badgeValue >= 99 ? "99+" : self.badgeValue
        self.badgeLabel?.text = self.badgeValue
        let duration: TimeInterval = (animated && self.shouldAnimateBadge) ? 0.2 : 0
        UIView.animate(withDuration: duration, animations: {
            self.updateBadgeFrame()
        })
    }

    fileprivate func updateBadgeFrame() {
        let expectedLabelSize: CGSize = self.badgeExpectedSize()
        var minHeight: CGFloat = expectedLabelSize.height
        minHeight = (minHeight < self.badgeMinSize) ? self.badgeMinSize : expectedLabelSize.height

        var minWidth: CGFloat = expectedLabelSize.width
        let padding = self.badgePadding
        minWidth = (minWidth < minHeight) ? minHeight : expectedLabelSize.width

        let badgeWidth = minWidth + padding
        let badgeHeight = minHeight + padding

        if self.badgeOriginX > self.frame.size.width {
            self.badgeOriginX = self.frame.size.width - badgeWidth/2
        }

        if self.badgeOriginY > self.frame.size.height {
            self.badgeOriginY = self.frame.size.height - badgeHeight/2
        }

        self.badgeLabel?.frame = CGRect(x: self.badgeOriginX - 3, y: self.badgeOriginY + 8, width: badgeWidth, height: badgeHeight)
        self.badgeLabel?.layer.cornerRadius = badgeHeight/2
        self.badgeLabel?.layer.masksToBounds = true
    }

    fileprivate func badgeExpectedSize() -> CGSize {
        let frameLabel: UILabel = self.duplicate(self.badgeLabel)
        frameLabel.sizeToFit()
        let expectedLabelSize: CGSize = frameLabel.frame.size
        return expectedLabelSize
    }

    fileprivate func duplicate(_ labelToCopy: UILabel?) -> UILabel {
        guard let temp = labelToCopy else { fatalError("xxxx") }
        let duplicateLabel = UILabel(frame: temp.frame)
        duplicateLabel.text = temp.text
        duplicateLabel.font = temp.font
        return duplicateLabel
    }
}

extension String {
    func characterAtIndex(index: Int) -> Character? {
        var cur = 0
        for char in self {
            if cur == index {
                return char
            }
            cur += 1
        }
        return "0"
    }
}

extension UIColor {
    convenience init(hexString: String) {
        // 处理数值
        var cString = hexString.uppercased().trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)

        let length = (cString as NSString).length
        // 错误处理
        if length < 6 || length > 7 || (!cString.hasPrefix("#") && length == 7) {
            // 返回whiteColor
            self.init(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
            return
        }

        if cString.hasPrefix("#") {
            cString = (cString as NSString).substring(from: 1)
        }

        // 字符chuan截取
        var range = NSRange()
        range.location = 0
        range.length = 2

        let rString = (cString as NSString).substring(with: range)

        range.location = 2
        let gString = (cString as NSString).substring(with: range)

        range.location = 4
        let bString = (cString as NSString).substring(with: range)

        // 存储转换后的数值
        var r: UInt32 = 0, g: UInt32 = 0, b: UInt32 = 0
        // 进行转换
        Scanner(string: rString).scanHexInt32(&r)
        Scanner(string: gString).scanHexInt32(&g)
        Scanner(string: bString).scanHexInt32(&b)
        // 根据颜色值创建UIColor
        self.init(red: CGFloat(r)/255.0, green: CGFloat(g)/255.0, blue: CGFloat(b)/255.0, alpha: 1.0)
    }
}

extension UIImage {
    class func imageWithColor(_ color: UIColor) -> UIImage {
        let rect = CGRect(x: 0, y: 0, width: 1.0, height: 1.0)
        UIGraphicsBeginImageContext(rect.size)
        let context = UIGraphicsGetCurrentContext()
        context?.setFillColor(color.cgColor)
        context?.fill(rect)
        let image = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()

        return image!
    }
}

extension UIColor {
    // 返回随机颜色
    open class var randomColor: UIColor {
        let red = CGFloat(arc4random() % 256)/255.0
        let green = CGFloat(arc4random() % 256)/255.0
        let blue = CGFloat(arc4random() % 256)/255.0
        return UIColor(red: red, green: green, blue: blue, alpha: 1.0)
    }
}

extension UITextField {
    @IBInspectable var placeHolderColor: UIColor? {
        get {
            return self.placeHolderColor
        }
        set {
            self.attributedPlaceholder = NSAttributedString(string: self.placeholder != nil ? self.placeholder! : "", attributes: [NSAttributedString.Key.foregroundColor: newValue!])
        }
    }
}

extension UserDefaults {
    enum AccountKeys: String {
        case uid
        case userToken
        case userName
        case appid
        case avatar
        case isLogin
    }

    static func set(value: String, forKey key: AccountKeys) {
        let key = key.rawValue
        UserDefaults.standard.set(value, forKey: key)
    }

    static func string(forKey key: AccountKeys) -> String? {
        let key = key.rawValue
        return UserDefaults.standard.string(forKey: key)
    }
}

extension UIViewController {
    func createBarButtonItem(title: String?) -> UIBarButtonItem {
        let leftButton = UIButton(type: .custom)
        leftButton.setTitle(title, for: .normal)
        leftButton.setImage(UIImage(named: "icon_back"), for: .normal)
        leftButton.setTitleColor(UIColor(hexString: "#1A1A1E"), for: .normal)
        leftButton.titleLabel?.font = UIFont(name: PingFangBold, size: 14)
        leftButton.layoutButtonWithEdgeInsetsStyle(style: .Left, space: 0)
        leftButton.addTarget(self, action: #selector(self.popBack), for: .touchUpInside)
        self.navigationItem.leftBarButtonItem = UIBarButtonItem(customView: leftButton)
        return UIBarButtonItem(customView: leftButton)
    }

    @objc func popBack() {
        self.navigationController?.popViewController(animated: true)
    }
}

class ARSourceTimer: NSObject {
    static var sourceTimer: DispatchSourceTimer?
    class func start(_ timeInterval: TimeInterval = 1, timeCount: @escaping (_ time: String) -> Void) {
        ARSourceTimer.sourceTimer = DispatchSource.makeTimerSource(queue: DispatchQueue.global())
        ARSourceTimer.sourceTimer?.schedule(deadline: .now(), repeating: timeInterval)

        var index = 0.0
        ARSourceTimer.sourceTimer?.setEventHandler(handler: {
            index += 1.0

            DispatchQueue.main.async {
                timeCount(transToHourMinSec(time: Float(index)))
            }
        })
        ARSourceTimer.sourceTimer?.resume()
    }

    class func stop() {
        ARSourceTimer.sourceTimer?.cancel()
    }
}

func transToHourMinSec(time: Float) -> String {
    let allTime = Int(time)
    var hours = 0
    var minutes = 0
    var seconds = 0
    var hoursText = ""
    var minutesText = ""
    var secondsText = ""

    hours = allTime/3600
    hoursText = hours > 9 ? "\(hours)" : "0\(hours)"

    minutes = allTime % 3600/60
    minutesText = minutes > 9 ? "\(minutes)" : "0\(minutes)"

    seconds = allTime % 3600 % 60
    secondsText = seconds > 9 ? "\(seconds)" : "0\(seconds)"

    return (hoursText != "00") ? "\(hoursText):\(minutesText):\(secondsText)" : "\(minutesText):\(secondsText)"
}

enum ARAlertActionSheet {
    static func showAlert(titleStr: String?, msgStr: String?, style: UIAlertController.Style = .alert, currentVC: UIViewController, cancelBtn: String = "取消", cancelHandler: ((UIAlertAction) -> Void)?, otherBtns: [String]?, otherHandler: ((Int) -> Void)?) {
        DispatchQueue.main.async {
            let alertVc = UIAlertController(title: titleStr, message: msgStr, preferredStyle: style)

            let cancelAction = UIAlertAction(title: cancelBtn, style: .cancel, handler: { action -> Void in
                cancelHandler?(action)
            })
            cancelAction.setValue(UIColor(hexString: "#787878"), forKey: "titleTextColor")
            alertVc.addAction(cancelAction)

            if otherBtns != nil {
                for (index, value) in (otherBtns?.enumerated())! {
                    let otherAction = UIAlertAction(title: value, style: .default, handler: { _ in
                        otherHandler!(index)
                    })
                    otherAction.setValue(UIColor(hexString: "#181914"), forKey: "titleTextColor")
                    alertVc.addAction(otherAction)
                }
            }
            currentVC.present(alertVc, animated: true, completion: nil)
        }
    }
}
