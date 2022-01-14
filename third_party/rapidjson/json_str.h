#ifndef __JOSN_STR_H__
#define __JOSN_STR_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSON_DFT "{}"

class JsonStr
{
public:
	JsonStr(): Ptr(NULL), Len(0){};
	JsonStr(const char*pData, int nLen): Ptr(NULL), Len(0){
		SetData(pData, nLen);
	};
	virtual ~JsonStr(){
		//#3 The C++ language guarantees that delete p will do nothing if p is null
		{
			delete[] Ptr;
			Ptr = NULL;
		}
		Len = 0;
	};
	
	void SetData(const char*pData, int nLen)
	{
		if (Ptr != NULL)
		{
			delete[] Ptr;
			Ptr = NULL;
		}

		if (pData == NULL || nLen <= 0)
		{
			nLen = strlen(JSON_DFT);
			Len = nLen + 1;
			Ptr = new char[Len];
			memcpy(Ptr, JSON_DFT, nLen);
			Ptr[nLen] = '\0';
			return;
		}
			
		Len = nLen + 1;
		Ptr = new char[Len];
		memcpy(Ptr, pData, nLen);
		Ptr[nLen] = '\0';
	}
	
	char* 	Ptr;
	int 	Len;
};

#endif 	//__JOSN_STR_H__