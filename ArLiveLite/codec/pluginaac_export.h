#ifndef __PLUGIN_AAC_EXPORT_H__
#define __PLUGIN_AAC_EXPORT_H__

#ifdef PLUGIN_AAC_EXPORT
#define PLUGIN_AAC_API _declspec(dllexport)
#elif PLUGIN_AAC_DLL
#define PLUGIN_AAC_API _declspec(dllimport)
#else
#define PLUGIN_AAC_API
#endif

#endif	// __PLUGIN_AAC_EXPORT_H__