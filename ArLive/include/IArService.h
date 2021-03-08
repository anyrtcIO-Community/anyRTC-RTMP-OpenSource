//  AR SDK
//
//  Copyright (c) 2019 AR.io. All rights reserved.
//

#ifndef __I_AR_SERVICE_H__
#define __I_AR_SERVICE_H__
#include "ArBase.h"

namespace ar {
    namespace rtc {
        class IRtcEngine;
    }
    namespace rtm {
        class IRtmService;
    }
namespace base {

struct ARServiceContext
{
};


class IARService
{
protected:
    virtual ~IARService(){}
public:
    virtual void release() = 0;

	/** Initializes the engine.
     
    @param context RtcEngine context.
    @return
     - 0: Success.
     - < 0: Failure.
    */
    virtual int initialize(const ARServiceContext& context) = 0;

    /** Retrieves the SDK version number.
    * @param build Build number.
    * @return The current SDK version in the string format. For example, 2.4.0
    */
    virtual const char* getVersion(int* build) = 0;

    virtual rtm::IRtmService* createRtmService() = 0;
};

} //namespace base
} // namespace ar

/** Gets the SDK version number.
 
 @param build Build number of the AR SDK.
 @return
 - 0: Success.
 - < 0: Failure.
*/
AR_API const char* AR_CALL getARSdkVersion(int* build);

/**
* Creates the RtcEngine object and returns the pointer.
* @param err Error code
* @return returns Description of the error code
*/
AR_API const char* AR_CALL getARSdkErrorDescription(int err);

/**
* Creates the AR Service object and returns the pointer.
* @return returns Pointer of the AR Service object
*/
AR_API ar::base::IARService* AR_CALL createARService();

AR_API int AR_CALL setARSdkExternalSymbolLoader(void* (*func)(const char* symname));

#endif
