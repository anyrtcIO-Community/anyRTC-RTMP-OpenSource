#include "amf0.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

static double s_double = 1.0; // 3ff0 0000 0000 0000

static uint8_t* AMFWriteInt16(uint8_t* ptr, const uint8_t* end, uint16_t value)
{
	if (ptr + 2 > end) return NULL;
	ptr[0] = value >> 8;
	ptr[1] = value & 0xFF;
	return ptr + 2;
}

static uint8_t* AMFWriteInt32(uint8_t* ptr, const uint8_t* end, uint32_t value)
{
	if (ptr + 4 > end) return NULL;
	ptr[0] = (uint8_t)(value >> 24);
	ptr[1] = (uint8_t)(value >> 16);
	ptr[2] = (uint8_t)(value >> 8);
	ptr[3] = (uint8_t)(value & 0xFF);
	return ptr + 4;
}

static uint8_t* AMFWriteString16(uint8_t* ptr, const uint8_t* end, const char* string, size_t length)
{
	if (ptr + 2 + length > end) return NULL;
	ptr = AMFWriteInt16(ptr, end, (uint16_t)length);
	memcpy(ptr, string, length);
	return ptr + length;
}

static uint8_t* AMFWriteString32(uint8_t* ptr, const uint8_t* end, const char* string, size_t length)
{
	if (ptr + 4 + length > end) return NULL;
	ptr = AMFWriteInt32(ptr, end, (uint32_t)length);
	memcpy(ptr, string, length);
	return ptr + length;
}

uint8_t* AMFWriteNull(uint8_t* ptr, const uint8_t* end)
{
	if (!ptr || ptr + 1 > end) return NULL;

	*ptr++ = AMF_NULL;
	return ptr;
}

uint8_t* AMFWriteUndefined(uint8_t* ptr, const uint8_t* end)
{
    if (!ptr || ptr + 1 > end) return NULL;

    *ptr++ = AMF_UNDEFINED;
    return ptr;
}

uint8_t* AMFWriteObject(uint8_t* ptr, const uint8_t* end)
{
	if (!ptr || ptr + 1 > end) return NULL;

	*ptr++ = AMF_OBJECT;
	return ptr;
}

uint8_t* AMFWriteObjectEnd(uint8_t* ptr, const uint8_t* end)
{
	if (!ptr || ptr + 3 > end) return NULL;

	/* end of object - 0x00 0x00 0x09 */
	*ptr++ = 0;
	*ptr++ = 0;
	*ptr++ = AMF_OBJECT_END;
	return ptr;
}

uint8_t* AMFWriteTypedObject(uint8_t* ptr, const uint8_t* end)
{
    if (!ptr || ptr + 1 > end) return NULL;

    *ptr++ = AMF_TYPED_OBJECT;
    return ptr;
}

uint8_t* AMFWriteECMAArarry(uint8_t* ptr, const uint8_t* end)
{
	if (!ptr || ptr + 1 > end) return NULL;

	*ptr++ = AMF_ECMA_ARRAY;
	return AMFWriteInt32(ptr, end, 0); // U32 associative-count
}

uint8_t* AMFWriteBoolean(uint8_t* ptr, const uint8_t* end, uint8_t value)
{
	if (!ptr || ptr + 2 > end) return NULL;

	ptr[0] = AMF_BOOLEAN;
	ptr[1] = 0 == value ? 0 : 1;
	return ptr + 2;
}

uint8_t* AMFWriteDouble(uint8_t* ptr, const uint8_t* end, double value)
{
	if (!ptr || ptr + 9 > end) return NULL;

	assert(8 == sizeof(double));
	*ptr++ = AMF_NUMBER;

	// Little-Endian
	if (0x00 == *(char*)&s_double)
	{
		*ptr++ = ((uint8_t*)&value)[7];
		*ptr++ = ((uint8_t*)&value)[6];
		*ptr++ = ((uint8_t*)&value)[5];
		*ptr++ = ((uint8_t*)&value)[4];
		*ptr++ = ((uint8_t*)&value)[3];
		*ptr++ = ((uint8_t*)&value)[2];
		*ptr++ = ((uint8_t*)&value)[1];
		*ptr++ = ((uint8_t*)&value)[0];
	}
	else
	{
		memcpy(ptr, &value, 8);
	}
	return ptr;
}

uint8_t* AMFWriteString(uint8_t* ptr, const uint8_t* end, const char* string, size_t length)
{
	if (!ptr || ptr + 1 + (length < 65536 ? 2 : 4) + length > end || length > UINT32_MAX)
		return NULL;

	if (length < 65536)
	{
		*ptr++ = AMF_STRING;
		AMFWriteString16(ptr, end, string, length);
		ptr += 2;
	}
	else
	{
		*ptr++ = AMF_LONG_STRING;
		AMFWriteString32(ptr, end, string, length);
		ptr += 4;
	}
	return ptr + length;
}

uint8_t* AMFWriteDate(uint8_t* ptr, const uint8_t* end, double milliseconds, int16_t timezone)
{
    if (!ptr || ptr + 11 > end)
        return NULL;

    AMFWriteDouble(ptr, end, milliseconds);
    *ptr = AMF_DATE; // rewrite to date
    return AMFWriteInt16(ptr + 8, end, timezone);
}

uint8_t* AMFWriteNamedBoolean(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, uint8_t value)
{
	if (ptr + length + 2 + 2 > end)
		return NULL;

	ptr = AMFWriteString16(ptr, end, name, length);
	return ptr ? AMFWriteBoolean(ptr, end, value) : NULL;
}

uint8_t* AMFWriteNamedDouble(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, double value)
{
	if (ptr + length + 2 + 8 + 1 > end)
		return NULL;

	ptr = AMFWriteString16(ptr, end, name, length);
	return ptr ? AMFWriteDouble(ptr, end, value) : NULL;
}

uint8_t* AMFWriteNamedString(uint8_t* ptr, const uint8_t* end, const char* name, size_t length, const char* value, size_t length2)
{
	if (ptr + length + 2 + length2 + 3 > end)
		return NULL;

	ptr = AMFWriteString16(ptr, end, name, length);
	return ptr ? AMFWriteString(ptr, end, value, length2) : NULL;
}

static const uint8_t* AMFReadInt16(const uint8_t* ptr, const uint8_t* end, uint32_t* value)
{
	if (!ptr || ptr + 2 > end)
		return NULL;

	if (value)
	{
		*value = ((uint32_t)ptr[0] << 8) | ptr[1];
	}
	return ptr + 2;
}

static const uint8_t* AMFReadInt32(const uint8_t* ptr, const uint8_t* end, uint32_t* value)
{
	if (!ptr || ptr + 4 > end)
		return NULL;

	if (value)
	{
		*value = ((uint32_t)ptr[0] << 24) | ((uint32_t)ptr[1] << 16) | ((uint32_t)ptr[2] << 8) | ptr[3];
	}
	return ptr + 4;
}

const uint8_t* AMFReadNull(const uint8_t* ptr, const uint8_t* end)
{
	(void)end;
	return ptr;
}

const uint8_t* AMFReadUndefined(const uint8_t* ptr, const uint8_t* end)
{
    (void)end;
    return ptr;
}

const uint8_t* AMFReadBoolean(const uint8_t* ptr, const uint8_t* end, uint8_t* value)
{
	if (!ptr || ptr + 1 > end)
		return NULL;

	if (value)
	{
		*value = ptr[0];
	}
	return ptr + 1;
}

const uint8_t* AMFReadDouble(const uint8_t* ptr, const uint8_t* end, double* value)
{
	uint8_t* p = (uint8_t*)value;
	if (!ptr || ptr + 8 > end)
		return NULL;

	if (value)
	{
		if (0x00 == *(char*)&s_double)
		{// Little-Endian
			*p++ = ptr[7];
			*p++ = ptr[6];
			*p++ = ptr[5];
			*p++ = ptr[4];
			*p++ = ptr[3];
			*p++ = ptr[2];
			*p++ = ptr[1];
			*p++ = ptr[0];
		}
		else
		{
			memcpy(value, ptr, 8);
		}
	}
	return ptr + 8;
}

const uint8_t* AMFReadString(const uint8_t* ptr, const uint8_t* end, int isLongString, char* string, size_t length)
{ 
	uint32_t len = 0;
	if (0 == isLongString)
		ptr = AMFReadInt16(ptr, end, &len);
	else
		ptr = AMFReadInt32(ptr, end, &len);

	if (!ptr || ptr + len > end)
		return NULL;

	if (string && length > len)
	{
		memcpy(string, ptr, len);
		string[len] = 0;
	}
	return ptr + len;
}

const uint8_t* AMFReadDate(const uint8_t* ptr, const uint8_t* end, double *milliseconds, int16_t *timezone)
{
    uint32_t v;
    ptr = AMFReadDouble(ptr, end, milliseconds);
    if (ptr)
    {
        ptr = AMFReadInt16(ptr, end, &v);
        *timezone = (int16_t)v;
    }
    return ptr;
}

static const uint8_t* amf_read_object(const uint8_t* data, const uint8_t* end, struct amf_object_item_t* items, size_t n);
static const uint8_t* amf_read_ecma_array(const uint8_t* data, const uint8_t* end, struct amf_object_item_t* items, size_t n);
static const uint8_t* amf_read_strict_array(const uint8_t* ptr, const uint8_t* end, struct amf_object_item_t* items, size_t n);

static const uint8_t* amf_read_item(const uint8_t* data, const uint8_t* end, enum AMFDataType type, struct amf_object_item_t* item)
{
	switch (type)
	{
	case AMF_BOOLEAN:
		return AMFReadBoolean(data, end, (uint8_t*)(item ? item->value : NULL));

	case AMF_NUMBER:
		return AMFReadDouble(data, end, (double*)(item ? item->value : NULL));

	case AMF_STRING:
		return AMFReadString(data, end, 0, (char*)(item ? item->value : NULL), item ? item->size : 0);

	case AMF_LONG_STRING:
		return AMFReadString(data, end, 1, (char*)(item ? item->value : NULL), item ? item->size : 0);

    case AMF_DATE:
        return AMFReadDate(data, end, (double*)(item ? item->value : NULL), (int16_t*)(item ? (char*)item->value + 8 : NULL));

	case AMF_OBJECT:
		return amf_read_object(data, end, (struct amf_object_item_t*)(item ? item->value : NULL), item ? item->size : 0);

	case AMF_NULL:
		return data;

	case AMF_UNDEFINED:
		return data;

	case AMF_ECMA_ARRAY:
		return amf_read_ecma_array(data, end, (struct amf_object_item_t*)(item ? item->value : NULL), item ? item->size : 0);

	case AMF_STRICT_ARRAY:
		return amf_read_strict_array(data, end, (struct amf_object_item_t*)(item ? item->value : NULL), item ? item->size : 0);

	default:
		assert(0);
		return NULL;
	}
}

static inline int amf_read_item_type_check(uint8_t type0, uint8_t itemtype)
{
    // decode AMF_ECMA_ARRAY as AMF_OBJECT
    return (type0 == itemtype || (AMF_OBJECT == itemtype && (AMF_ECMA_ARRAY == type0 || AMF_NULL == type0))) ? 1 : 0;
}

static const uint8_t* amf_read_strict_array(const uint8_t* ptr, const uint8_t* end, struct amf_object_item_t* items, size_t n)
{
	uint8_t type;
	uint32_t i, count;
	if (!ptr || ptr + 4 > end)
		return NULL;

	ptr = AMFReadInt32(ptr, end, &count); // U32 array-count
	for (i = 0; i < count && ptr && ptr < end; i++)
	{
		type = *ptr++;
		ptr = amf_read_item(ptr, end, type, (i < n && amf_read_item_type_check(type, items[i].type)) ? &items[i] : NULL);
	}

	return ptr;
}

static const uint8_t* amf_read_ecma_array(const uint8_t* ptr, const uint8_t* end, struct amf_object_item_t* items, size_t n)
{
	if (!ptr || ptr + 4 > end)
		return NULL;
	ptr += 4; // U32 associative-count
	return amf_read_object(ptr, end, items, n);
}

static const uint8_t* amf_read_object(const uint8_t* data, const uint8_t* end, struct amf_object_item_t* items, size_t n)
{
	uint8_t type;
	uint32_t len;
	size_t i;

	while (data && data + 2 <= end)
	{
		len = *data++ << 8;
		len |= *data++;
		if (0 == len)
			break; // last item

		if (data + len + 1 > end)
			return NULL; // invalid

		for (i = 0; i < n; i++)
		{
			if (strlen(items[i].name) == len && 0 == memcmp(items[i].name, data, len) && amf_read_item_type_check(data[len], items[i].type))
				break;
		}

		data += len; // skip name string
		type = *data++; // value type
		data = amf_read_item(data, end, type, i < n ? &items[i] : NULL);
	}

	if (data && data < end && AMF_OBJECT_END == *data)
		return data + 1;
	return NULL; // invalid object
}

const uint8_t* amf_read_items(const uint8_t* data, const uint8_t* end, struct amf_object_item_t* items, size_t count)
{
	size_t i;
	uint8_t type;
	for (i = 0; i < count && data && data < end; i++)
	{
		type = *data++;
		if (!amf_read_item_type_check(type, items[i].type))
			return NULL;

		data = amf_read_item(data, end, type, &items[i]);
	}

	return data;
}

#if defined(_DEBUG) || defined(DEBUG)
struct rtmp_amf0_command_t
{
	char fmsVer[64];
	double capabilities;
	double mode;
};
struct rtmp_amf0_data_t
{
	char version[64];
};
struct rtmp_amf0_information_t
{
	char code[64]; // NetStream.Play.Start
	char level[8]; // warning/status/error
	char description[256];
	double clientid;
	double objectEncoding;
	struct rtmp_amf0_data_t data;
};
static void amf0_test_1(void)
{
    const uint8_t amf0[] = {
        0x02, 0x00, 0x07, 0x5F, 0x72, 0x65, 0x73, 0x75, 0x6C, 0x74, 
		0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

        0x03, 
			0x00, 0x06, 0x66, 0x6D, 0x73, 0x56, 0x65, 0x72, 0x02, 0x00, 0x0E, 0x46, 0x4D, 0x53, 0x2F, 0x33, 0x2C, 0x35, 0x2C, 0x35, 0x2C, 0x32, 0x30, 0x30, 0x34, 
			0x00, 0x0C, 0x63, 0x61, 0x70,0x61, 0x62, 0x69, 0x6C, 0x69, 0x74, 0x69, 0x65, 0x73, 0x00, 0x40, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x04, 0x6D, 0x6F, 0x64, 0x65, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x09, 
		
		0x03, 
			0x00, 0x05, 0x6C, 0x65, 0x76, 0x65, 0x6C, 0x02, 0x00, 0x06, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 
			0x00, 0x04, 0x63, 0x6F, 0x64, 0x65, 0x02, 0x00, 0x1D, 0x4E, 0x65, 0x74, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x2E, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x2E, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 
			0x00, 0x0B, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x02, 0x00, 0x15, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x73, 0x75, 0x63, 0x63, 0x65, 0x65, 0x64, 0x65, 0x64, 0x2E, 
			0x00, 0x04, 0x64, 0x61, 0x74, 0x61, 
				0x08, 0x00, 0x00, 0x00, 0x01, 
					0x00, 0x07, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0x00, 0x0A, 0x33, 0x2C, 0x35, 0x2C, 0x35, 0x2C, 0x32, 0x30, 0x30, 0x34, 
				0x00, 0x00, 0x09, 
			0x00, 0x08, 0x63, 0x6C, 0x69, 0x65, 0x6E, 0x74, 0x69, 0x64, 0x00, 0x41, 0xD7, 0x9B, 0x78, 0x7C, 0xC0, 0x00, 0x00, 
			0x00, 0x0E, 0x6F, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x45, 0x6E, 0x63, 0x6F, 0x64, 0x69, 0x6E, 0x67, 0x00, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x09,
    };

	char reply[8];
	const uint8_t* end;
	double transactionId;
	struct rtmp_amf0_command_t fms;
	struct rtmp_amf0_information_t result;
	struct amf_object_item_t cmd[3];
	struct amf_object_item_t data[1];
	struct amf_object_item_t info[6];
	struct amf_object_item_t items[4];

#define AMF_OBJECT_ITEM_VALUE(v, amf_type, amf_name, amf_value, amf_size) { v.type=amf_type; v.name=amf_name; v.value=amf_value; v.size=amf_size; }
	AMF_OBJECT_ITEM_VALUE(cmd[0], AMF_STRING, "fmsVer", fms.fmsVer, sizeof(fms.fmsVer));
	AMF_OBJECT_ITEM_VALUE(cmd[1], AMF_NUMBER, "capabilities", &fms.capabilities, sizeof(fms.capabilities));
	AMF_OBJECT_ITEM_VALUE(cmd[2], AMF_NUMBER, "mode", &fms.mode, sizeof(fms.mode));

	AMF_OBJECT_ITEM_VALUE(data[0], AMF_STRING, "version", result.data.version, sizeof(result.data.version));

	AMF_OBJECT_ITEM_VALUE(info[0], AMF_STRING, "code", result.code, sizeof(result.code));
	AMF_OBJECT_ITEM_VALUE(info[1], AMF_STRING, "level", result.level, sizeof(result.level));
	AMF_OBJECT_ITEM_VALUE(info[2], AMF_STRING, "description", result.description, sizeof(result.description));
	AMF_OBJECT_ITEM_VALUE(info[3], AMF_ECMA_ARRAY, "data", data, sizeof(data)/sizeof(data[0]));
	AMF_OBJECT_ITEM_VALUE(info[4], AMF_NUMBER, "clientid", &result.clientid, sizeof(result.clientid));
	AMF_OBJECT_ITEM_VALUE(info[5], AMF_NUMBER, "objectEncoding", &result.objectEncoding, sizeof(result.objectEncoding));

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_STRING, "reply", reply, sizeof(reply)); // Command object
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "transaction", &transactionId, sizeof(transactionId)); // Command object
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_OBJECT, "command", cmd, sizeof(cmd)/sizeof(cmd[0])); // Command object
	AMF_OBJECT_ITEM_VALUE(items[3], AMF_OBJECT, "information", info, sizeof(info) / sizeof(info[0])); // Information object

	end = amf0 + sizeof(amf0);
	assert(end == amf_read_items(amf0, end, items, sizeof(items) / sizeof(items[0])));
	assert(0 == strcmp(fms.fmsVer, "FMS/3,5,5,2004"));
	assert(fms.capabilities == 31.0);
	assert(fms.mode == 1.0);
	assert(0 == strcmp(result.code, "NetConnection.Connect.Success"));
	assert(0 == strcmp(result.level, "status"));
	assert(0 == strcmp(result.description, "Connection succeeded."));
	assert(0 == strcmp(result.data.version, "3,5,5,2004"));
	assert(1584259571.0 == result.clientid);
	assert(3.0 == result.objectEncoding);
}

struct rtmp_amf0_connect_t
{
	char app[64]; // Server application name, e.g.: testapp
	char flashver[32]; // Flash Player version, FMSc/1.0
	char swfUrl[256]; // URL of the source SWF file
	char tcUrl[256]; // URL of the Server, rtmp://host:1935/testapp/instance1
	uint8_t fpad; // boolean: True if proxy is being used.
	double capabilities; // double default: 15
	double audioCodecs; // double default: 4071
	double videoCodecs; // double default: 252
	double videoFunction; // double default: 1
	double encoding;
	char pageUrl[256]; // http://host/sample.html
};

static void amf0_test_2(void)
{
	const uint8_t amf0[] = {
		0x02, 0x00, 0x07, 0x63, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00,
		0x03, 0x61, 0x70,
		0x70, 0x02, 0x00, 0x06, 0x6d, 0x65, 0x64, 0x69, 0x61, 0x2f, 0x00, 0x05, 0x74, 0x63, 0x55, 0x72,
		0x6c, 0x02, 0x00, 0x28, 0x72, 0x74, 0x6d, 0x70, 0x3a, 0x2f, 0x2f, 0x70, 0x75, 0x73, 0x68, 0x2d,
		0x72, 0x74, 0x6d, 0x70, 0x2d, 0x66, 0x35, 0x2d, 0x78, 0x67, 0x2e, 0x69, 0x78, 0x69, 0x67, 0x75,
		0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x6d, 0x65, 0x64, 0x69, 0x61, 0x2f, 0x00, 0x04, 0x74, 0x79,
		0x70, 0x65, 0x02, 0x00, 0x0a, 0x6e, 0x6f, 0x6e, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0x00,
		0x08, 0x66, 0x6c, 0x61, 0x73, 0x68, 0x56, 0x65, 0x72, 0x02, 0x00, 0x1f, 0x46, 0x4d, 0x4c, 0x45,
		0x2f, 0x33, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c,
		0x65, 0x3b, 0x20, 0x46, 0x4d, 0x53, 0x63, 0x2f, 0x31, 0x2e, 0x30, 0x29, 0x00, 0x06, 0x73, 0x77,
		0x66, 0x55, 0x72, 0x6c, 0x02, 0x00, 0x28, 0x72, 0x74, 0x6d, 0x70, 0x3a, 0x2f, 0x2f, 0x70, 0x75,
		0x73, 0x68, 0x2d, 0x72, 0x74, 0x6d, 0x70, 0x2d, 0x66, 0x35, 0x2d, 0x78, 0x67, 0x2e, 0x69, 0x78,
		0x69, 0x67, 0x75, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x6d, 0x65, 0x64, 0x69, 0x61, 0x2f, 0x00,
		0x04, 0x66, 0x70, 0x61, 0x64, 0x01, 0x00, 0x00, 0x0c, 0x63, 0x61, 0x70, 0x61, 0x62, 0x69, 0x6c,
		0x69, 0x74, 0x69, 0x65, 0x73, 0x00, 0x40, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,
		0x61, 0x75, 0x64, 0x69, 0x6f, 0x43, 0x6f, 0x64, 0x65, 0x63, 0x73, 0x00, 0x40, 0xa8, 0xee, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x76, 0x69, 0x64, 0x65, 0x6f, 0x43, 0x6f, 0x64, 0x65,
		0x63, 0x73, 0x00, 0x40, 0x6f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x76, 0x69, 0x64,
		0x65, 0x6f, 0x46, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x07, 0x70, 0x61, 0x67, 0x65, 0x55, 0x72, 0x6c, 0x06, 0x00, 0x0e, 0x6f,
		0x62, 0x6a, 0x65, 0x63, 0x74, 0x45, 0x6e, 0x63, 0x6f, 0x64, 0x69, 0x6e, 0x67, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09,
	};

	char reply[8];
	const uint8_t* end;
	double transactionId;
	struct rtmp_amf0_connect_t connect;
	struct amf_object_item_t commands[11];
	struct amf_object_item_t items[3];

#define AMF_OBJECT_ITEM_VALUE(v, amf_type, amf_name, amf_value, amf_size) { v.type=amf_type; v.name=amf_name; v.value=amf_value; v.size=amf_size; }
	AMF_OBJECT_ITEM_VALUE(commands[0], AMF_STRING, "app", connect.app, sizeof(connect.app));
	AMF_OBJECT_ITEM_VALUE(commands[1], AMF_STRING, "flashVer", connect.flashver, sizeof(connect.flashver));
	AMF_OBJECT_ITEM_VALUE(commands[2], AMF_STRING, "tcUrl", connect.tcUrl, sizeof(connect.tcUrl));
	AMF_OBJECT_ITEM_VALUE(commands[3], AMF_BOOLEAN, "fpad", &connect.fpad, 1);
	AMF_OBJECT_ITEM_VALUE(commands[4], AMF_NUMBER, "audioCodecs", &connect.audioCodecs, 8);
	AMF_OBJECT_ITEM_VALUE(commands[5], AMF_NUMBER, "videoCodecs", &connect.videoCodecs, 8);
	AMF_OBJECT_ITEM_VALUE(commands[6], AMF_NUMBER, "videoFunction", &connect.videoFunction, 8);
	AMF_OBJECT_ITEM_VALUE(commands[7], AMF_NUMBER, "objectEncoding", &connect.encoding, 8);
	AMF_OBJECT_ITEM_VALUE(commands[8], AMF_NUMBER, "capabilities", &connect.capabilities, 8);
	AMF_OBJECT_ITEM_VALUE(commands[9], AMF_STRING, "pageUrl", &connect.pageUrl, sizeof(connect.pageUrl));
	AMF_OBJECT_ITEM_VALUE(commands[10], AMF_STRING, "swfUrl", &connect.swfUrl, sizeof(connect.swfUrl));

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_STRING, "reply", reply, sizeof(reply)); // Command object
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "transaction", &transactionId, sizeof(transactionId)); // Command object
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_OBJECT, "command", commands, sizeof(commands) / sizeof(commands[0])); // Command object
	
	end = amf0 + sizeof(amf0);
	memset(&connect, 0, sizeof(connect));
	assert(end == amf_read_items(amf0, end, items, sizeof(items) / sizeof(items[0])));
	assert(0 == strcmp(connect.app, "media/"));
	assert(0 == strcmp(connect.tcUrl, "rtmp://push-rtmp-f5-xg.ixigua.com/media/"));
	assert(0 == strcmp(connect.flashver, "FMLE/3.0 (compatible; FMSc/1.0)"));
	assert(0 == strcmp(connect.swfUrl, "rtmp://push-rtmp-f5-xg.ixigua.com/media/"));
	assert(0 == strcmp(connect.pageUrl, "")); // pageUrl undefined
	assert(connect.fpad == 0);
	assert(connect.capabilities == 15);
	assert(connect.audioCodecs == 3191);
	assert(connect.videoCodecs == 252);
	assert(connect.videoFunction == 1);
	assert(connect.encoding == 0);
}

void amf0_test(void)
{
	amf0_test_1();
	amf0_test_2();
}
#endif
