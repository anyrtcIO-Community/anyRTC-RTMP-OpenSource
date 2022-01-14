#ifndef __ARP_DEF_H__
#define __ARP_DEF_H__

#include <stdint.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define ARP_CALL __cdecl
#if defined(ANYRTC_EXPORTS)
#define ARP_API extern "C" __declspec(dllexport)
#else
#define ARP_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define ARP_API __attribute__((visibility("default"))) extern "C"
#define ARP_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define ARP_API extern "C" __attribute__((visibility("default")))
#define ARP_CALL
#else
#define ARP_API extern "C"
#define ARP_CALL
#endif

#endif	// __ARP_DEF_H__