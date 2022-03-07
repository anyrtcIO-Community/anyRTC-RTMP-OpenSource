//
//  ARToast.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import UIKit

class ARToast: NSObject {
    private static let DEFAULT_DURATION = 2.0

    class func showText(text: String) {
        let toast = ARToast()
        toast.text = text
        toast.showToast()
    }

    class func showText(text: String, duration: Double) {
        let toast = ARToast()
        toast.text = text
        toast.duration = duration
        toast.showToast()
    }

    class func showText(text: String, topOffset: CGFloat) {
        let toast = ARToast()
        toast.text = text
        toast.topOffset = topOffset
        toast.showToast()
    }

    class func showText(text: String, topOffset: CGFloat, duration: Double) {
        let toast = ARToast()
        toast.text = text
        toast.topOffset = topOffset
        toast.duration = duration
        toast.showToast()
    }

    class func showText(text: String, bottomOffset: CGFloat) {
        let toast = ARToast()
        toast.text = text
        toast.bottomOffset = bottomOffset
        toast.showToast()
    }

    class func showText(text: String, bottomOffset: CGFloat, duration: Double) {
        let toast = ARToast()
        toast.text = text
        toast.bottomOffset = bottomOffset
        toast.duration = duration
        toast.showToast()
    }

    private var text: String = "" {
        didSet {
            let font = UIFont.boldSystemFont(ofSize: 14)
            let paragraphStyle = NSMutableParagraphStyle()
            paragraphStyle.lineBreakMode = .byWordWrapping
            let attrDict = [NSAttributedString.Key.font: font, NSAttributedString.Key.paragraphStyle: paragraphStyle]

            let expectedRect = text.boundingRect(with: CGSize(width: 280, height: CGFloat(MAXFLOAT)), options: NSStringDrawingOptions.usesLineFragmentOrigin, attributes: attrDict, context: nil)
            let textSize = expectedRect.size

            textLabel.frame = CGRect(x: 0, y: 0, width: textSize.width + 12, height: textSize.height + 12)
            textLabel.backgroundColor = UIColor.clear
            textLabel.textColor = UIColor.white
            textLabel.textAlignment = .center
            textLabel.font = font
            textLabel.text = text as String
            textLabel.numberOfLines = 0

            contentView.frame = CGRect(x: 0, y: 0, width: textLabel.frame.size.width, height: textLabel.frame.size.height)
            contentView.layer.cornerRadius = 5.0
            contentView.layer.borderWidth = 1.0
            contentView.layer.borderColor = UIColor.gray.withAlphaComponent(0.5).cgColor
            contentView.backgroundColor = UIColor(red: 0.2, green: 0.2, blue: 0.2, alpha: 0.75)
            contentView.addSubview(textLabel)
            contentView.autoresizingMask = .flexibleWidth
            contentView.addTarget(self, action: #selector(toastTapped), for: .touchDown)
            contentView.alpha = 0.0
            if let _window = UIApplication.shared.keyWindow {
                contentView.center = _window.center
            }
        }
    }

    private var duration = ARToast.DEFAULT_DURATION
    private var textLabel: UILabel = .init()
    private var contentView: UIButton = .init()
    private var topOffset: CGFloat = 0 {
        didSet {
            let window = UIApplication.shared.keyWindow
            if let _window = window {
                contentView.center = CGPoint(x: _window.center.x, y: topOffset + contentView.frame.size.height/2)
            }
        }
    }

    private var bottomOffset: CGFloat = 0 {
        didSet {
            let window = UIApplication.shared.keyWindow
            if let _window = window {
                contentView.center = CGPoint(x: _window.center.x, y: _window.frame.size.height - (bottomOffset + self.contentView.frame.size.height/2))
            }
        }
    }

    override private init() {
        super.init()
        NotificationCenter.default.addObserver(self, selector: #selector(deviceOrientationDidChanged), name: UIDevice.orientationDidChangeNotification, object: UIDevice.current)
    }

    deinit {
        NotificationCenter.default.removeObserver(self, name: UIDevice.orientationDidChangeNotification, object: UIDevice.current)
    }

    @objc func deviceOrientationDidChanged(notify: NSNotification) {
        hideAnimation()
    }

    private func showAnimation() {
        UIView.beginAnimations("show", context: nil)
        UIView.setAnimationCurve(.easeIn)
        UIView.setAnimationDuration(0.3)
        contentView.alpha = 1.0
        UIView.commitAnimations()
    }

    @objc func hideAnimation() {
        UIView.beginAnimations("hide", context: nil)
        UIView.setAnimationCurve(.easeOut)
        UIView.setAnimationDelegate(self)
        UIView.setAnimationDidStop(#selector(dismissToast))
        UIView.setAnimationDuration(0.3)
        contentView.alpha = 0.0
        UIView.commitAnimations()
    }

    @objc func dismissToast() {
        contentView.removeFromSuperview()
    }

    @objc func toastTapped(sender: UIButton) {
        hideAnimation()
    }

    private func showToast() {
        let window = UIApplication.shared.keyWindow
        if let _window = window {
            _window.addSubview(contentView)
            showAnimation()
            perform(#selector(hideAnimation), with: nil, afterDelay: duration)
        }
    }
}
