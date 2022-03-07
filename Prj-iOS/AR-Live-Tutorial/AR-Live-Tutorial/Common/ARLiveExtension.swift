//
//  ARLiveExtension.swift
//  AR-Live-Tutorial
//
//  Created by 余生丶 on 2021/11/9.
//

import ARLiveKit

extension ARLiveVideoResolution {
    var description: String {
        var text: String
        
        switch self {
        case .resolution1280x720: text = "分辨率 720P"
        case .resolution960x540: text = "分辨率 540P"
        case .resolution480x360: text = "分辨率 360P"
        default:
            text = "\(self.rawValue)"
        }
        
        return text
    }
}

extension ARLivePushStatus {
    var description: String {
        var text: String
        
        switch self {
        case .disconnected: text = "连接失败"
        case .connecting: text = "连接中..."
        case .connectSuccess: text = "连接成功"
        case .reconnecting: text = "重连中..."
        default:
            text = "\(self.rawValue)"
        }
        
        return text
    }
}

extension ARLivePlayStatus {
    var description: String {
        var text: String
        
        switch self {
        case .stopped: text = "播放停止"
        case .playing: text = "正在播放"
        case .loading: text = "正在缓冲"
        default:
            text = "\(self.rawValue)"
        }
        
        return text
    }
}

extension ARLiveRenderMode {
    var description: String {
        var text: String
        
        switch self {
        case .fit: text = "Fit"
        case .hidden: text = "Hidden"
        case .fill: text = "Fill"
        default:
            text = "\(self.rawValue)"
        }
        
        return text
    }
}
