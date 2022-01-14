
#include "ArHttpClient.h"
#include "rtc_base/synchronization/mutex.h"
#import "ObjcNetWorkManager.h"

class ArHttpClient {
    
    public:
	ArHttpClient(void);
	virtual ~ArHttpClient(void);
	static ArHttpClient*The();

	int DoRequest(void*ptr, ArHttpClientResponse resp, ArHttpVerb eHttpVerb, int nTimeout, const std::string&strReqUrl, ArHttpHeaders&httpHeahders, const std::string&strContent);
	int UnRequest(void*ptr);

    protected:
	struct HttpReq
	{
		HttpReq() {
		
		}

		void StartReq(int nTimeout) {
            NSString *url = [NSString stringWithUTF8String:strReqUrl.c_str()];
            NSString *content = [NSString stringWithUTF8String:strContent.c_str()];
            
            [ObjcNetWorkManager postRequestWithUrl:url params:content success:^(id  _Nonnull result) {
                NSLog(@"POST_success____%@", result);
                cbResp(ptr, AHC_OK, httpHeahders, [result UTF8String]);
            } failure:^(NSInteger code) {
                NSLog(@"POST_failure____%ld", (long)code);
            }];
        }
        
		void* ptr;
		ArHttpVerb eHttpVerb;
		ArHttpClientResponse cbResp;

		std::string strReqUrl;
		ArHttpHeaders httpHeahders;
		std::string strContent;
	};
    
    private:
    typedef std::map<void*, HttpReq> MapHttpReq;
    webrtc::Mutex cs_map_http_req_;
    MapHttpReq	map_http_req_;
};

int ArHttpClientDoRequest(void*ptr, ArHttpClientResponse resp, ArHttpVerb eHttpVerb, int nTimeout, const std::string&strReqUrl, ArHttpHeaders&httpHeahders, const std::string&strContent) {
	return ArHttpClient::The()->DoRequest(ptr, resp, eHttpVerb, nTimeout, strReqUrl, httpHeahders, strContent);
}

int ArHttpClientUnRequest(void*ptr) {
	return ArHttpClient::The()->UnRequest(ptr);
}

ArHttpClient*ArHttpClient::The() {
	static ArHttpClient gShareThread;
	return &gShareThread;
}

ArHttpClient::ArHttpClient(void){};

ArHttpClient::~ArHttpClient(void){};

int ArHttpClient::DoRequest(void*ptr, ArHttpClientResponse cbResp, ArHttpVerb eHttpVerb, int nTimeout, const std::string&strReqUrl, ArHttpHeaders&httpHeahders, const std::string&strContent) {
	if (nTimeout < 0) {
		nTimeout = 1000;
	}
    
	if (nTimeout > 90000) {
		nTimeout = 90000;
	}
    
	webrtc::MutexLock l(&cs_map_http_req_);
	if (map_http_req_.find(ptr) != map_http_req_.end()) {
		return -1;
	}

	HttpReq&httpReq = map_http_req_[ptr];
	httpReq.ptr = ptr;
	httpReq.eHttpVerb = eHttpVerb;
	httpReq.cbResp = cbResp;
	httpReq.strReqUrl = strReqUrl;
	httpReq.httpHeahders = httpHeahders;
	httpReq.strContent = strContent;

    httpReq.StartReq(nTimeout);
	return 0;
}

int ArHttpClient::UnRequest(void*ptr) {
	return 0;
}



