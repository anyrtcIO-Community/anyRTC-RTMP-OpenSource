//
//  ARLiveMacros.h
//  ARLiveKit
//
//  Created by 余生丶 on 2021/11/11.
//

#ifndef ARLiveMacros_h
#define ARLiveMacros_h

#define CallForMainQueue(__function__)\
if([NSThread isMainThread]){\
    __function__;\
} else {\
    dispatch_async(dispatch_get_main_queue(),^{\
        __function__;\
        });\
}\


#endif /* ARLiveMacros_h */
