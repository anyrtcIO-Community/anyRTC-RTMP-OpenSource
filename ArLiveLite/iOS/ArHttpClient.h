#ifndef __AR_HTTP_CLIENT_H__
#define __AR_HTTP_CLIENT_H__
#include <map>
#include <string>

#include <stdint.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define HTP_CALL __cdecl
#if defined(ANYRTC_EXPORTS)
#define HTP_API extern "C" __declspec(dllexport)
#else
#define HTP_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define HTP_API __attribute__((visibility("default"))) extern "C"
#define HTP_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#include "jni.h"
#define HTP_API extern "C" __attribute__((visibility("default")))
#define HTP_CALL
#else
#define HTP_API extern "C"
#define HTP_CALL
#endif

enum ArHttpCode {
	AHC_OK = 200,
	AHC_NON_AUTHORITATIVE = 203,
	AHC_NO_CONTENT = 204,
	AHC_PARTIAL_CONTENT = 206,

	AHC_MULTIPLE_CHOICES = 300,
	AHC_MOVED_PERMANENTLY = 301,
	AHC_FOUND = 302,
	AHC_SEE_OTHER = 303,
	AHC_NOT_MODIFIED = 304,
	AHC_MOVED_TEMPORARILY = 307,

	AHC_BAD_REQUEST = 400,
	AHC_UNAUTHORIZED = 401,
	AHC_FORBIDDEN = 403,
	AHC_NOT_FOUND = 404,
	AHC_PROXY_AUTHENTICATION_REQUIRED = 407,
	AHC_GONE = 410,

	AHC_INTERNAL_SERVER_ERROR = 500,
	AHC_NOT_IMPLEMENTED = 501,
	AHC_SERVICE_UNAVAILABLE = 503,
};

enum ArHttpVersion {
	AHVER_1_0, AHVER_1_1, AHVER_UNKNOWN,
	AHVER_LAST = AHVER_UNKNOWN
};

enum ArHttpVerb {
	AHV_GET, AHV_POST, AHV_PUT, AHV_DELETE, AHV_CONNECT, AHV_OPTIONS, AHV_HEAD,	//@Eric - Add AHV_OPTIONS for http
	AHV_LAST = AHV_HEAD
};

typedef std::map<std::string, std::string> ArHttpHeaders;
typedef void(*ArHttpClientResponse)(void*ptr, ArHttpCode eCode, ArHttpHeaders&httpHeahders, const std::string&strContent);

HTP_API int ArHttpClientDoRequest(void*ptr, ArHttpClientResponse resp, ArHttpVerb eHttpVerb, int nTimeout, const std::string&strReqUrl, ArHttpHeaders&httpHeahders, const std::string&strContent);
HTP_API int ArHttpClientUnRequest(void*ptr);

#endif	// __AR_HTTP_CLIENT_H__