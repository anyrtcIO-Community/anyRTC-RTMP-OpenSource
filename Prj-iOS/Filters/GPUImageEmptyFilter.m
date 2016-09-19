//
//  GPUImageEmptyFilter.m
//  BeautifyFace
//
//  Created by jianqiangzhang on 16/5/27.
//  Copyright © 2016年 ClaudeLi. All rights reserved.
//

#import "GPUImageEmptyFilter.h"
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
NSString *const kLFGPUImageEmptyFragmentShaderString = SHADER_STRING
(
 varying highp vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 
 void main()
 {
     lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((textureColor.rgb), textureColor.w);
 }
 );
#else
NSString *const kGPUImageInvertFragmentShaderString = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 
 void main()
 {
     vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((textureColor.rgb), textureColor.w);
 }
 );
#endif
@implementation GPUImageEmptyFilter
- (id)init;
{
    if (!(self = [super initWithFragmentShaderFromString:kLFGPUImageEmptyFragmentShaderString]))
    {
        return nil;
    }
    
    return self;
}
@end
