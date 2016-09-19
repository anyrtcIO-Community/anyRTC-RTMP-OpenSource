//LIV_Export.h

#ifndef __LIV_EXPORT_H__
#define __LIV_EXPORT_H__

#ifdef LIV_EXPORTS
#define LIV_API _declspec(dllexport)
#elif LIV_DLL
#define LIV_API _declspec(dllimport)
#else
#define LIV_API
#endif

#endif	// __LIV_EXPORT_H__