#include "amf3.h"
#include <string.h>
#include <assert.h>

static double s_double = 1.0; // 3ff0 0000 0000 0000

const uint8_t* AMF3ReadNull(const uint8_t* ptr, const uint8_t* end)
{
	(void)end;
	return ptr;
}

const uint8_t* AMF3ReadBoolean(const uint8_t* ptr, const uint8_t* end)
{
	(void)end;
	return ptr;
}

const uint8_t* AMF3ReadInteger(const uint8_t* ptr, const uint8_t* end, int32_t* value)
{
	int i;
	int32_t v = 0;

	for (i = 0; i < 3 && ptr + i < end && (0x80 & ptr[i]); i++)
	{
		v <<= 7;
		v |= (ptr[i] & 0x7F);
	}

	if (ptr + i >= end)
		return NULL;

	if (3 == i)
	{
		v <<= 8;
		v |= ptr[i];

		if (v >= (1 << 28))
			v -= (1 << 29);
	}
	else
	{
		v <<= 7;
		v |= ptr[i];
	}

	*value = v;
	return ptr + i + 1;
}

const uint8_t* AMF3ReadDouble(const uint8_t* ptr, const uint8_t* end, double* value)
{
	uint8_t* p = (uint8_t*)value;
	if (!ptr || end - ptr < 8)
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
			memcpy(&value, ptr, 8);
		}
	}
	return ptr + 8;
}

const uint8_t* AMF3ReadString(const uint8_t* ptr, const uint8_t* end, char* string, uint32_t* length)
{
	uint32_t v;
	ptr = AMF3ReadInteger(ptr, end, (int32_t*)&v);

	if (v & 0x01)
	{
		// reference
		return ptr;
	}
	else
	{
		*length = v >> 1;
		memcpy(string, ptr, *length);
		string[*length] = 0;
		return ptr + *length;
	}
}
