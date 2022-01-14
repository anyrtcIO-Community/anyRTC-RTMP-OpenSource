#ifndef _amf0_h_
#define _amf0_h_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum AMFDataType
{
	AMF_NUMBER = 0x00,
	AMF_BOOLEAN,
	AMF_STRING,
	AMF_OBJECT,
	AMF_MOVIECLIP,
	AMF_NULL,
	AMF_UNDEFINED,
	AMF_REFERENCE,
	AMF_ECMA_ARRAY,
	AMF_OBJECT_END,
	AMF_STRICT_ARRAY,
	AMF_DATE,
	AMF_LONG_STRING,
	AMF_UNSUPPORTED,
	AMF_RECORDSET,
	AMF_XML_DOCUMENT,
	AMF_TYPED_OBJECT,
	AMF_AVMPLUS_OBJECT,
};

uint8_t* AMFWriteNull(uint8_t* ptr, const uint8_t* end);
uint8_t* AMFWriteUndefined(uint8_t* ptr, const uint8_t* end);
uint8_t* AMFWriteObject(uint8_t* ptr, const uint8_t* end);
uint8_t* AMFWriteObjectEnd(uint8_t* ptr, const uint8_t* end);
uint8_t* AMFWriteTypedObject(uint8_t* ptr, const uint8_t* end);
uint8_t* AMFWriteECMAArarry(uint8_t* ptr, const uint8_t* end);

uint8_t* AMFWriteBoolean(uint8_t* ptr, const uint8_t* end, uint8_t value);
uint8_t* AMFWriteDouble(uint8_t* ptr, const uint8_t* end, double value);
uint8_t* AMFWriteString(uint8_t* ptr, const uint8_t* end, const char* string, size_t length);
uint8_t* AMFWriteDate(uint8_t* ptr, const uint8_t* end, double milliseconds, int16_t timezone);

uint8_t* AMFWriteNamedString(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, const char* value, size_t length2);
uint8_t* AMFWriteNamedDouble(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, double value);
uint8_t* AMFWriteNamedBoolean(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, uint8_t value);

const uint8_t* AMFReadNull(const uint8_t* ptr, const uint8_t* end);
const uint8_t* AMFReadUndefined(const uint8_t* ptr, const uint8_t* end);
const uint8_t* AMFReadBoolean(const uint8_t* ptr, const uint8_t* end, uint8_t* value);
const uint8_t* AMFReadDouble(const uint8_t* ptr, const uint8_t* end, double* value);
const uint8_t* AMFReadString(const uint8_t* ptr, const uint8_t* end, int isLongString, char* string, size_t length);
const uint8_t* AMFReadDate(const uint8_t* ptr, const uint8_t* end, double *milliseconds, int16_t *timezone);


struct amf_object_item_t
{
	enum AMFDataType type;
	const char* name;
	void* value;
	size_t size;
};
const uint8_t* amf_read_items(const uint8_t* data, const uint8_t* end, struct amf_object_item_t* items, size_t count);

#ifdef __cplusplus
}
#endif
#endif /* !_amf0_h_ */
