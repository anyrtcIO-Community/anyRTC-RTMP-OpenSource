#include "rtmp-netconnection.h"
#include "amf0.h"
#include <stdlib.h>
#include <string.h>

uint8_t* rtmp_netconnection_connect(uint8_t* out, size_t bytes, double transactionId, const struct rtmp_connect_t* connect)
{
	uint8_t* end = out + bytes;
	const char* command = "connect";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);

	out = AMFWriteObject(out, end);
	out = AMFWriteNamedString(out, end, "app", 3, connect->app, strlen(connect->app));
	out = AMFWriteNamedString(out, end, "flashVer", 8, connect->flashver, strlen(connect->flashver));
	if (connect->tcUrl[0]) out = AMFWriteNamedString(out, end, "tcUrl", 5, connect->tcUrl, strlen(connect->tcUrl));
	if (connect->swfUrl[0]) out = AMFWriteNamedString(out, end, "swfUrl", 6, connect->swfUrl, strlen(connect->swfUrl));
	if (connect->pageUrl[0]) out = AMFWriteNamedString(out, end, "pageUrl", 7, connect->pageUrl, strlen(connect->pageUrl));
	out = AMFWriteNamedBoolean(out, end, "fpad", 4, connect->fpad);
	out = AMFWriteNamedDouble(out, end, "capabilities", 12, connect->capabilities);
	out = AMFWriteNamedDouble(out, end, "audioCodecs", 11, connect->audioCodecs);
	out = AMFWriteNamedDouble(out, end, "videoCodecs", 11, connect->videoCodecs);
	out = AMFWriteNamedDouble(out, end, "videoFunction", 13, connect->videoFunction);
	out = AMFWriteNamedDouble(out, end, "objectEncoding", 14, connect->encoding);
	out = AMFWriteObjectEnd(out, end);
	return out;
}

uint8_t* rtmp_netconnection_connect_reply(uint8_t* out, size_t bytes, double transactionId, const char* fmsver, double capabilities, const char* code, const char* level, const char* description, double encoding)
{
	uint8_t* end = out + bytes;
	const char* command = "_result";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);

	out = AMFWriteObject(out, end);
	out = AMFWriteNamedString(out, end, "fmsVer", 6, fmsver, strlen(fmsver));
	out = AMFWriteNamedDouble(out, end, "capabilities", 12, capabilities);
	out = AMFWriteObjectEnd(out, end);

	out = AMFWriteObject(out, end);
	out = AMFWriteNamedString(out, end, "level", 5, level, strlen(level));
	out = AMFWriteNamedString(out, end, "code", 4, code, strlen(code));
	out = AMFWriteNamedString(out, end, "description", 11, description, strlen(description));
	out = AMFWriteNamedDouble(out, end, "objectEncoding", 14, encoding);
	out = AMFWriteObjectEnd(out, end);
	return out;
}

uint8_t* rtmp_netconnection_create_stream(uint8_t* out, size_t bytes, double transactionId)
{
	uint8_t* end = out + bytes;
	const char* command = "createStream";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);
	out = AMFWriteNull(out, end);
	return out;
}

uint8_t* rtmp_netconnection_create_stream_reply(uint8_t* out, size_t bytes, double transactionId, double stream_id)
{
	uint8_t* end = out + bytes;
	const char* command = "_result";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);
	out = AMFWriteNull(out, end);
	out = AMFWriteDouble(out, end, stream_id);
	return out;
}

uint8_t* rtmp_netconnection_get_stream_length(uint8_t* out, size_t bytes, double transactionId, const char* stream_name)
{
	uint8_t* end = out + bytes;
	const char* command = "getStreamLength";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);
	out = AMFWriteNull(out, end);
	out = AMFWriteString(out, end, stream_name, strlen(stream_name));
	return out;
}

uint8_t* rtmp_netconnection_get_stream_length_reply(uint8_t* out, size_t bytes, double transactionId, double duration)
{
	uint8_t* end = out + bytes;
	const char* command = "_result";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);
	out = AMFWriteNull(out, end);
	out = AMFWriteDouble(out, end, duration);
	return out;
}

uint8_t* rtmp_netconnection_error(uint8_t* out, size_t bytes, double transactionId, const char* code, const char* level, const char* description)
{
	uint8_t* end = out + bytes;
	const char* command = "_error";

	out = AMFWriteString(out, end, command, strlen(command));
	out = AMFWriteDouble(out, end, transactionId);
	out = AMFWriteNull(out, end);
	
	out = AMFWriteObject(out, end);
	out = AMFWriteNamedString(out, end, "code", 4, code, strlen(code));
	out = AMFWriteNamedString(out, end, "level", 5, level, strlen(level));
	out = AMFWriteNamedString(out, end, "description", 11, description, strlen(description));
	out = AMFWriteObjectEnd(out, end);

	return out;
}
