//
//  ARLiveEngineKit.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/9/19.
//

#import <Foundation/Foundation.h>
#import "ARLivePusher.h"
#import "ARLivePlayer.h"
#import "ARLiveDelegate.h"
#import "ARLiveEnumerates.h"
#import "ARLiveObjects.h"

NS_ASSUME_NONNULL_BEGIN

@interface ARLiveEngineKit : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

/**
 * 初始化 ARLiveEngineKit 对象。
 *
 * @param delegate 回调目标对象
 * @return 一个 ARLiveEngineKit 实例对象
 */
- (instancetype)initWithDelegate:(id<ARLiveEngineDelegate> _Nullable)delegate;

/**
 * 销毁 ARLiveEngineKit 实例
 */
+ (void)destroy;

/**
 * 创建推流对象。
 *
 * @return 一个 ARLivePusher 实例对象
 */
- (ARLivePusher *)createArLivePusher;

/**
 * 创建拉流对象。
 *
 * @return 一个 ARLivePlayer 实例对象
 */
- (ARLivePlayer *)createArLivePlayer;

/**
 * 析构 ARLivePusher 对象。
 *
 * @param pusher 一个 ARLivePusher 实例对象
 */
- (void)releaseArLivePusher:(ARLivePusher *)pusher;

/**
 * 析构 ARLivePlayer 对象。
 *
 * @param player 一个 ARLivePlayer 实例对象
 */
- (void)releaseArLivePlayer: (ARLivePlayer *)player;

@end

NS_ASSUME_NONNULL_END
