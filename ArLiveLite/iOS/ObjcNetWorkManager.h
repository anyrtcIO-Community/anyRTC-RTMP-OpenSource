//
//  ObjcNetWorkManager.h
//  ARLiveLibrary
//
//  Created by 余生丶 on 2021/10/29.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^ARNetSuccessBlock)(id result);
typedef void (^ARNetFailureBlock)(NSInteger code);

@interface ObjcNetWorkManager : NSObject

/// GET 请求
/// @param url url 请求地址
/// @param params 请求参数
/// @param success 成功回调
/// @param failure 失败回调
+ (void)getRequestWithUrl:(NSString *_Nonnull)url params:(NSDictionary *)params success:(ARNetSuccessBlock)success failure:(ARNetFailureBlock)failure;

/// POST 请求
/// @param url url 请求地址
/// @param params 请求参数
/// @param success 成功回调
/// @param failure 失败回调
+ (void)postRequestWithUrl:(NSString *_Nonnull)url params:(NSString *)params success:(ARNetSuccessBlock)success failure:(ARNetFailureBlock)failure;

/// JSON字符串解析
+ (NSString *)fromDicToJSONStr:(NSDictionary *)dic;
+ (id)fromJsonStr:(NSString*)jsonStr;

@end

NS_ASSUME_NONNULL_END
