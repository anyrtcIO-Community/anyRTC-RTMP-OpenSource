#ifndef _amf3_h_
#define _amf3_h_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum AMF3DataType
{
	AMF3_UNDEFINED = 0x00,
	AMF3_NULL,
	AMF3_FALSE,
	AMF3_TRUE,
	AMF3_INTEGER,
	AMF3_DOUBLE,
	AMF3_STRING,
	AMF3_XML_DOCUMENT,
	AMF3_DATE,
	AMF3_ARRAY,
	AMF3_OBJECT,
	AMF3_XML,
	AMF3_BYTE_ARRAY,
	AMF3_VECTOR_INT,
	AMF3_VECTOR_UINT,
	AMF3_VECTOR_DOUBLE,
	AMF3_VECTOR_OBJECT,
	AMF3_DICTIONARY,
};

//uint8_t* AMF3WriteNull(uint8_t* ptr, const uint8_t* end);
//uint8_t* AMF3WriteObject(uint8_t* ptr, const uint8_t* end);
//uint8_t* AMF3WriteObjectEnd(uint8_t* ptr, const uint8_t* end);
//
//uint8_t* AMF3WriteBoolean(uint8_t* ptr, const uint8_t* end, uint8_t value);
//uint8_t* AMF3WriteInteger(uint8_t* ptr, const uint8_t* end, int32_t value);
//uint8_t* AMF3WriteDouble(uint8_t* ptr, const uint8_t* end, double value);
//uint8_t* AMF3WriteString(uint8_t* ptr, const uint8_t* end, const char* string, size_t length);
//
//uint8_t* AMF3WriteNamedBoolean(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, uint8_t value);
//uint8_t* AMF3WriteNamedInteger(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, int32_t value);
//uint8_t* AMF3WriteNamedString(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, const char* value, size_t length2);
//uint8_t* AM3FWriteNamedDouble(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, double value);

const uint8_t* AMF3ReadNull(const uint8_t* ptr, const uint8_t* end);
const uint8_t* AMF3ReadBoolean(const uint8_t* ptr, const uint8_t* end);
const uint8_t* AMF3ReadInteger(const uint8_t* ptr, const uint8_t* end, int32_t* value);
const uint8_t* AMF3ReadDouble(const uint8_t* ptr, const uint8_t* end, double* value);
const uint8_t* AMF3ReadString(const uint8_t* ptr, const uint8_t* end, char* string, uint32_t* length);

#ifdef __cplusplus
}
#endif
#endif /* !_amf3_h_ */
