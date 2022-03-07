//
//  LoseScene.swift
//  AR-Live-Tutorial
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

import UIKit
import SpriteKit

class LoseScene: SKScene {
    private var scoreLb = SKLabelNode.init(fontNamed: "Chalkduster")
    var score = 0 {
        didSet {
            scoreLb.text = "Score:"+String(score)
        }
    }
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    override init(size: CGSize) {
        super.init(size: size)
        setup()
        addNotes()
    }
    private func setup() {
        backgroundColor = .white
        let action = SKAction.playSoundFileNamed("gameover.caf", waitForCompletion: false)
        run(action)
    }
    private func addNotes() {
        scoreLb.text = "Score:"+String(score)
        scoreLb.fontSize = 30;
        scoreLb.fontColor = .black
        scoreLb.position = .init(x: size.width/2, y: size.height/2+50)
        addChild(scoreLb)
        
        let resultlb = SKLabelNode.init(fontNamed: "Chalkduster")
        resultlb.text = "Game Over"
        resultlb.fontSize = 30;
        resultlb.fontColor = .black
        resultlb.position = .init(x: size.width/2, y: size.height/2)
        addChild(resultlb)
        
        let retryLabel = SKLabelNode.init(fontNamed: "Chalkduster")
        retryLabel.text = "Try again"
        retryLabel.fontSize = 20
        retryLabel.fontColor = .blue
        retryLabel.position = .init(x: resultlb.position.x, y: resultlb.position.y * 0.8);
        retryLabel.name = "retryLabel"
        addChild(retryLabel)
    }
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        let touch = touches.first!
        let loaction = touch.location(in: self)
        let array = nodes(at: loaction)
        for node in array {
            if node.name == "retryLabel" {
                retry()
                break
            }
        }
    }
    private func retry(){
        let sence = GameScene.init(size: size)
        let transation = SKTransition.reveal(with: .up, duration: 0.5)
        view?.presentScene(sence, transition: transation)
    }
}
