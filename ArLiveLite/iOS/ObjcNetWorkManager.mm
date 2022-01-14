//
//  ObjcNetWorkManager.m
//  ARLiveLibrary
//
//  Created by 余生丶 on 2021/10/29.
//

#import "ObjcNetWorkManager.h"

NSString *const ResponseErrorKey = @"com.alamofire.serialization.response.error.response";
NSInteger const Interval = 3;

@implementation ObjcNetWorkManager

+ (void)getRequestWithUrl:(NSString *_Nonnull)url params:(NSDictionary *)params success:(ARNetSuccessBlock)success failure:(ARNetFailureBlock)failure
{
    NSString *urlString = [NSString string];
    if (params) {
        //参数拼接url
        NSString *paramStr = [self dealWithParam:params];
        urlString = [url stringByAppendingString:paramStr];
    } else {
        urlString = url;
    }
    //对URL中的中文进行转码
    NSString *pathStr = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:pathStr]];
    
    request.timeoutInterval = Interval;
    
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (data) {
                //利用iOS自带原生JSON解析data数据 保存为Dictionary
                NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                success(dict);
            } else {
                NSHTTPURLResponse *httpResponse = error.userInfo[ResponseErrorKey];
                
                if (httpResponse.statusCode != 0) {
                    failure(httpResponse.statusCode);
                } else {
                    failure(error.code);
                }
            }

        });
    }];
    
    [task resume];
}

+ (void)postRequestWithUrl:(NSString *_Nonnull)url params:(NSString *)params success:(ARNetSuccessBlock)success failure:(ARNetFailureBlock)failure {

    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    [request setHTTPMethod:@"POST"];
    
    //把字典中的参数进行拼接
    //NSString *body = [self dealWithParam:params];
    NSData *bodyData = [params dataUsingEncoding:NSUTF8StringEncoding];
    
    //设置请求体
    [request setHTTPBody:bodyData];
    //设置本次请求的数据请求格式
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    
    // 设置本次请求请求体的长度(因为服务器会根据你这个设定的长度去解析你的请求体中的参数内容)
    [request setValue:[NSString stringWithFormat:@"%ld", bodyData.length] forHTTPHeaderField:@"Content-Length"];
    //设置请求最长时间
    request.timeoutInterval = Interval;
    
    NSURLSessionTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (data) {
            NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            
            NSString * str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            
            success(str);
        } else {
            NSHTTPURLResponse *httpResponse = error.userInfo[ResponseErrorKey];
            if (httpResponse.statusCode != 0) {
                failure(httpResponse.statusCode);
            } else {
                failure(error.code);
            }
        }
    }];
    [task resume];
}

// MARK: -- 拼接参数

+ (NSString *)dealWithParam:(NSDictionary *)param {
    NSArray *allkeys = [param allKeys];
    NSMutableString *result = [NSMutableString string];
    
    for (NSString *key in allkeys) {
        NSString *string = [NSString stringWithFormat:@"%@=%@&", key, param[key]];
        [result appendString:string];
    }
    return result;
}

//MARK: - Json 解析

// JSON字符串解析
+ (id)fromJsonStr:(NSString*)jsonStr {
    //转换为NSData
    NSData *data= [jsonStr dataUsingEncoding:NSUTF8StringEncoding];
    
    //设置错误对象
    NSError *error = nil;
    NSArray *arr;
    NSMutableDictionary *dict = [[NSMutableDictionary alloc] init];
    //解析JSON字符串
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:&error];
    if ([jsonObject isKindOfClass:[NSDictionary class]]){
        //字典类型
        NSDictionary *dictionary = (NSDictionary *)jsonObject;
        [dict setDictionary:dictionary];
        return dict;
    } else if ([jsonObject isKindOfClass:[NSArray class]]){
        //数组类型
        arr= (NSArray *)jsonObject;
        return arr;
    }
    //解析错误
    return nil;
}

//将字典转换为JSON字符串
+ (NSString *)fromDicToJSONStr:(NSDictionary *)dic {
    //字典转json去空格换行符
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:&error];
    NSString *jsonString;
    if (!jsonData) {
        NSLog(@"%@",error);
    } else {
        jsonString = [[NSString alloc]initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
    NSMutableString *JSONStr = [NSMutableString stringWithString:jsonString];
    NSRange range = {0,jsonString.length};
    //去掉字符串中的空格
    [JSONStr replaceOccurrencesOfString:@" " withString:@"" options:NSLiteralSearch range:range];
    NSRange range2 = {0,JSONStr.length};
    //去掉字符串中的换行符
    [JSONStr replaceOccurrencesOfString:@"\n" withString:@"" options:NSLiteralSearch range:range2];
    return JSONStr;
}

@end
