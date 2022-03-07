//
//  ARBaseNavigationController.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/09.
//

import UIKit

class ARBaseNavigationController: UINavigationController {
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        titleColor = UIColor.black
        navgationBarColor = UIColor.white
    }

    override var childForStatusBarStyle: UIViewController? {
        return topViewController
    }

    /*
     // MARK: - Navigation

     // In a storyboard-based application, you will often want to do a little preparation before navigation
     override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
         // Get the new view controller using segue.destination.
         // Pass the selected object to the new view controller.
     }
     */
}

public extension UINavigationController {
    private enum AssociatedKey {
        static var navgationBarColor = UIColor.white
        static var titleColor = UIColor.black
    }
    
    var navgationBarColor: UIColor {
        get {
            return objc_getAssociatedObject(self, &AssociatedKey.navgationBarColor) as? UIColor ?? UIColor.white
        }
        
        set {
            if #available(iOS 15.0, *) {
                let appearance = UINavigationBarAppearance()
                appearance.configureWithOpaqueBackground()
                appearance.backgroundColor = newValue
                
                var textAttributes: [NSAttributedString.Key: AnyObject] = [:]
                textAttributes[.foregroundColor] = titleColor
                appearance.titleTextAttributes = textAttributes
                
                navigationBar.standardAppearance = appearance
                navigationBar.scrollEdgeAppearance = navigationBar.standardAppearance
            } else {
                // Fallback on earlier versions
                navigationBar.setBackgroundImage(createImage(newValue), for: .any, barMetrics: .default)
            }
            
            objc_setAssociatedObject(self, &AssociatedKey.navgationBarColor, newValue, .OBJC_ASSOCIATION_COPY_NONATOMIC)
        }
    }
    
    var titleColor: UIColor {
        get {
            return objc_getAssociatedObject(self, &AssociatedKey.titleColor) as? UIColor ?? UIColor.black
        }
        
        set {
            if #available(iOS 15.0, *) {
                let appearance = UINavigationBarAppearance()
                appearance.configureWithOpaqueBackground()
                appearance.backgroundColor = navgationBarColor
                
                var textAttributes: [NSAttributedString.Key: AnyObject] = [:]
                textAttributes[.foregroundColor] = newValue
                appearance.titleTextAttributes = textAttributes
                
                navigationBar.standardAppearance = appearance
                navigationBar.scrollEdgeAppearance = navigationBar.standardAppearance
            } else {
                // Fallback on earlier versions
                navigationBar.titleTextAttributes = [NSAttributedString.Key.foregroundColor: newValue]
            }
            objc_setAssociatedObject(self, &AssociatedKey.titleColor, newValue, .OBJC_ASSOCIATION_COPY_NONATOMIC)
        }
    }
}
