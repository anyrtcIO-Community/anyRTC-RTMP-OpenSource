//
// Created by liu on 2021/9/18.
//

#ifndef LIVEPLAYER_ANDROIDCONTEXT_H
#define LIVEPLAYER_ANDROIDCONTEXT_H

#include "PlatformContext.h"
#include <jni.h>


namespace arlive {
    class AndroidContext final : public PlatformContext {
    public:
        AndroidContext(JNIEnv *env,jobject instance,bool isScreen);
        ~AndroidContext() override;
        jobject getJavaCapturer();
        jclass getJavaCapturerClass();
    private:
        jclass VideoCapturerDeviceClass = nullptr;
        jobject javaCapturer = nullptr;
    };
}


#endif //LIVEPLAYER_ANDROIDCONTEXT_H
