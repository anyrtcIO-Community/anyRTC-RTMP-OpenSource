/*
*  Copyright (c) 2021 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/
#ifndef __AR_LIVE_BASE_H__
#define __AR_LIVE_BASE_H__

#include <stdint.h>
#include <stdio.h>
#include <map>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define AR_LV_CALL __cdecl
#if defined(ARRTC_EXPORT)
#define AR_LV_API extern "C" __declspec(dllexport)
#else
#define AR_LV_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define AR_LV_API __attribute__((visibility("default"))) extern "C"
#define AR_LV_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define AR_LV_API extern "C" __attribute__((visibility("default")))
#define AR_LV_CALL
#else
#define AR_LV_API extern "C"
#define AR_LV_CPP_API
#define AR_LV_CALL
#endif

#define AL	ar::live

namespace ar {
namespace live {


enum FUNC_RET_CODE
{//* 函数调用返回值
	FRC_OK = 0,		// Success
	FRC_Not_Init,	// 未初始化
	FRC_Has_Init,	// 已初始化(重复初始化)
	FRC_Err_Param,	// 参数有误
	FRC_Err_State,	// 状态不匹配，比如重复多次调用SetMute(true)
	FRC_Null_Str,	// 字符串为空
	FRC_Unknow = 100,// 未知错误
};
	
class ArParams
{
public:
	ArParams(void) {};
	virtual ~ArParams(void) {};

	void SetKeyStrVal(const std::string&strKey, const std::string&strVal) {
		map_str_[strKey] = strVal;
	};
	void SetKeyIntVal(const std::string&strKey, int nVal) {
		map_int_[strKey] = nVal;
	};

	std::string GetStrVal(const std::string&strKey) {
		if (map_str_.find(strKey) != map_str_.end()) {
			return map_str_[strKey];
		}
		return "";
	};
	int GetIntVal(const std::string&strKey) {
		int ret = -1;
		if (map_int_.find(strKey) != map_int_.end()) {
			ret = map_int_[strKey];
		}
		return ret;
	};

	std::string ToJson() {
		return "{}";
	}

private:
	std::map<std::string, std::string> map_str_;
	std::map<std::string, int> map_int_;
};

/** Video display rotation. */
enum RENDER_ROTATION
{
	RENDER_ROTATION_PORTRAIT = 0,
	RENDER_ROTATION_LANDSCAPE = 1
};

}
}

#endif	// __AR_LIVE_BASE_H__
