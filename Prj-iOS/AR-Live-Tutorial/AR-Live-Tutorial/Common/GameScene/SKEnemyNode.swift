//
//  SKElementNode.swift
//  AR-Live-Tutorial
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

import UIKit
import SpriteKit

enum SKElementNodeBuff: Int {
    case none
    case doubleBullet//双子弹
}

class SKElementNode: SKSpriteNode {
    var hp: UInt = 1
    var type:SKTextureType = .unknow
    var buff:SKElementNodeBuff = .none
    var shouldShoot = true
    
    convenience init(type:SKTextureType) {
        let t = TextureManager.shared.texture(type)
        self.init(texture: t)
        self.hp = hp(type)
        self.type = type
    }
    func alive() -> Bool {
        return hp > 0
    }
    private func hp(_ withType:SKTextureType) -> UInt {
        var value:UInt = 1
        switch withType {
        case .bullet:
            value = 1
            break
        case .hero:
            value = 1
            break
        case .small:
            value = 1
            break
        case .mid:
            value = 5
            break
        case .big:
            value = 7
            break
        case .boss:
            value = 80
            break
        default:
            value = 1
            break
        }
        return value
    }
}
