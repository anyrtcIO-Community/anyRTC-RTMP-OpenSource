//
//  ARCustomVideoFilter.h
//  ARLiveKit
//
//  Copyright © 2021年 anyRTC. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ARCustomVideoFilter : NSObject


@property (nonatomic, assign) BOOL isBeauty;

// 磨皮-平滑度（[0.0,1.0]）默认0.5
@property (nonatomic,assign) CGFloat smoothnessLevel;

// 亮度明暗对比度 0：低对比度 1：（默认）正常对比度 2：高对比度
@property (nonatomic,assign) CGFloat lighteningContrastLevel;

// 默认值 0.7，取值范围为 [0.0,1.0]，其中 0.0 表示原始亮度。可用来实现美白等视觉效果。
@property (nonatomic,assign) CGFloat lighteningLevel;

// 默认值 0.1，取值范围为 [0.0,1.0]，其中 0.0 表示原始红色度
@property (nonatomic,assign) CGFloat rednessLevel;


// 需要美化的数据
@property (nonatomic, assign) CVPixelBufferRef pixelBuffer;

// 美化后的数据
- (CVPixelBufferRef)outputPixelBuffer;

@end

NS_ASSUME_NONNULL_END
