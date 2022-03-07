//
//  AppDelegate.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/9/23.
//

import UIKit

@main
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?
    var rotation: UIInterfaceOrientationMask = .portrait

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.
        Thread.sleep(forTimeInterval: 1.3)
        return true
    }

    func application(_ application: UIApplication, supportedInterfaceOrientationsFor window: UIWindow?) -> UIInterfaceOrientationMask {
        return rotation
    }
}
