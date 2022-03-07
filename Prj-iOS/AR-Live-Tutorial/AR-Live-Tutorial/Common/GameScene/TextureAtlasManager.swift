//
//  TextureManager.swift
//  AR-Live-Tutorial
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

import UIKit
import SpriteKit

enum SKTextureType: Int {
    case unknow
    case bg = 1
    case bullet
    case hero
    case small
    case mid
    case big
    case boss
    case supply
}
class TextureManager: NSObject {
    static let shared = TextureManager.init()
    var atlas = SKTextureAtlas.init(named: "gameArts-hd")
    //MARK: -
    //MARK: Texture
    func texture(_ withType: SKTextureType) -> SKTexture {
        var name = ""
        switch withType {
        case .bg:
            name = "background_2.png"
            break
        case .bullet:
            name = "bullet1.png"
            break
        case .hero:
            name = "hero_fly_1.png"
            break
        case .small:
            name = "enemy1_fly_1.png"
            break
        case .mid:
            name = "enemy3_fly_1.png"
            break
        case .big:
            name = "enemy2_fly_1.png"
            break
        case .supply:
            name = "enemy5_fly_1.png"
            break
        case .boss:
            name = "enemy2_fly_1.png"
            break
        default:
            name = ""
            break
        }
        let texture = atlas.textureNamed(name)
        return texture
    }
    func node(_ withType:SKTextureType) -> SKSpriteNode {
        let t = texture(withType)
        let n = SKSpriteNode.init(texture: t)
        return n
    }
}
