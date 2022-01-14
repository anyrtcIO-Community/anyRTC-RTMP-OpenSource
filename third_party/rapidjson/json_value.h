#ifndef __JSON_VALUE_H__
#define __JSON_VALUE_H__
#include "document.h"
#define JS_NULL_STR ""

#define STRINGFY(x) _STRINGFY(x)
#define _STRINGFY(x) #x
#define F_AT __FILE__ ": " STRINGFY(__LINE__)

typedef void (*JsonGetErrorEvent)(const char*strLog);
static JsonGetErrorEvent gJsonGetErrorEvent = NULL;
static void SetJsonGetErrorEvent(JsonGetErrorEvent pEvent)
{
	gJsonGetErrorEvent = pEvent;
}

static bool JsStrNotNull(const char*strStr)
{
	if (strStr == NULL || strlen(strStr) == 0)
		return false;
	return true;
}

static bool HasJsonStr(rapidjson::Document&jsDoc, const char*strKey)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsString()) {
		return true;
	}
	return false;
}

static int HasJsonInt(rapidjson::Document&jsDoc, const char*strKey)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsInt()) {
		return true;
	}
	return false;
}

static int HasJsonBool(rapidjson::Document&jsDoc, const char*strKey)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsBool()) {
		return true;
	}
	return false;
}

static const char* GetJsonStr(rapidjson::Document&jsDoc, const char*strKey, const char* location)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsString()) {
		return jsDoc[strKey].GetString();
	}
	else {
		if (gJsonGetErrorEvent != NULL) {
			char strLog[512];
			sprintf(strLog, "Get str nil, key is: %s at: %s", strKey, location);
			gJsonGetErrorEvent(strLog);
		}
	}

	return JS_NULL_STR;
}

static int GetJsonInt(rapidjson::Document&jsDoc, const char*strKey, const char* location)
{
	if (jsDoc.HasMember(strKey) && (jsDoc[strKey].IsInt() || jsDoc[strKey].IsUint())) {
		if(jsDoc[strKey].IsInt()) 
			return jsDoc[strKey].GetInt();
		else 
			return jsDoc[strKey].GetUint();
	}
	else {
		if (gJsonGetErrorEvent != NULL) {
			char strLog[512];
			sprintf(strLog, "Get int nil, key is: %s at: %s", strKey, location);
			gJsonGetErrorEvent(strLog);
		}
	}
	return -1;
}

static int64_t GetJsonInt64(rapidjson::Document&jsDoc, const char*strKey, const char* location)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsInt64()) {
		return jsDoc[strKey].GetInt64();
	}
	else {
		if (gJsonGetErrorEvent != NULL) {
			char strLog[512];
			sprintf(strLog, "Get int nil, key is: %s at: %s", strKey, location);
			gJsonGetErrorEvent(strLog);
		}
	}
	return -1;
}

static bool GetJsonBool(rapidjson::Document&jsDoc, const char*strKey, const char* location)
{
	if (jsDoc.HasMember(strKey) && jsDoc[strKey].IsBool()) {
		return jsDoc[strKey].GetBool();
	}
	else {
		if (gJsonGetErrorEvent != NULL) {
			char strLog[512];
			sprintf(strLog, "Get bool nil, key is: %s at: %s", strKey, location);
			gJsonGetErrorEvent(strLog);
		}
	}
	return false;
}

#endif 