#ifndef _mpeg4_bits_h_
#define _mpeg4_bits_h_

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mpeg4_bits_t
{
	uint8_t* data;
	size_t size;
	size_t bits; // offset bit
	int error;
};

#define mpeg4_bits_read_uint8(bits, n)		(uint8_t)mpeg4_bits_read_n(bits, n)
#define mpeg4_bits_read_uint16(bits, n)		(uint16_t)mpeg4_bits_read_n(bits, n)
#define mpeg4_bits_read_uint32(bits, n)		(uint32_t)mpeg4_bits_read_n(bits, n)
#define mpeg4_bits_read_uint64(bits, n)		(uint64_t)mpeg4_bits_read_n(bits, n)
#define mpeg4_bits_write_uint8(bits, v, n)	mpeg4_bits_write_n(bits, (uint64_t)v, n)
#define mpeg4_bits_write_uint16(bits, v, n) mpeg4_bits_write_n(bits, (uint64_t)v, n)
#define mpeg4_bits_write_uint32(bits, v, n) mpeg4_bits_write_n(bits, (uint64_t)v, n)
#define mpeg4_bits_write_uint64(bits, v, n) mpeg4_bits_write_n(bits, (uint64_t)v, n)

static inline void mpeg4_bits_init(struct mpeg4_bits_t* bits, void* data, size_t size)
{
	bits->data = (uint8_t*)data;
	bits->size = size;
	bits->bits = 0;
	bits->error = 0;
}

/// @return 1-error, 0-no error
static inline int mpeg4_bits_error(struct mpeg4_bits_t* bits)
{
	//return bits->bits >= bits->size * 8 ? 1 : 0;
	return bits->error;
}

static inline void mpeg4_bits_aligment(struct mpeg4_bits_t* bits, int n)
{
	bits->bits = (bits->bits + n - 1) / n * n;
}

static inline size_t mpeg4_bits_remain(struct mpeg4_bits_t* bits)
{
	return bits->error ? 0 : (bits->size * 8 - bits->bits);
}

static inline void mpeg4_bits_skip(struct mpeg4_bits_t* bits, size_t n)
{
	bits->bits += n;
}

/// read 1-bit from bit stream(offset position)
/// @param[in] bits bit stream
/// @return -1-error, 1-value, 0-value
static inline int mpeg4_bits_read(struct mpeg4_bits_t* bits)
{
	uint8_t bit;
	assert(bits && bits->data && bits->size > 0);
	if (bits->bits >= bits->size * 8)
	{
		bits->error = -1;
		return 0; // throw exception
	}

	bit = bits->data[bits->bits/8] & (0x80U >> (bits->bits%8));
	bits->bits += 1; // update offset
	return bit ? 1 : 0;
}

/// read n-bit(n <= 64) from bit stream(offset position)
/// @param[in] bits bit stream
/// @return -1-error, other-value
static inline uint64_t mpeg4_bits_read_n(struct mpeg4_bits_t* bits, int n)
{
	int m;
	size_t i;
	uint64_t v;

	assert(n > 0 && n <= 64);
	assert(bits && bits->data && bits->size > 0);
	if (bits->bits + n > bits->size * 8 || n > 64 || n < 0)
	{
		bits->error = -1;
		return 0; // throw exception
	}

	m = n;
	v = bits->data[bits->bits / 8] & (0xFFU >> (bits->bits%8)); // remain valid value
	if (n <= 8 - (int)(bits->bits % 8))
	{
		v = v >> (8 - (bits->bits % 8) - n); // shift right value
		bits->bits += n;
		return v;
	}

	n -= 8 - (int)(bits->bits % 8);
	for (i = 1; n >= 8; i++)
	{
		assert(bits->bits / 8 + i < bits->size);
		v <<= 8;
		v += bits->data[bits->bits / 8 + i];
		n -= 8;
	}

	if (n > 0)
	{
		v <<= n;
		v += bits->data[bits->bits / 8 + i] >> (8 - n);
	}

	bits->bits += m;
	return v;
}

// http://aomedia.org/av1/specification/conventions/#descriptors
static inline uint64_t mpeg4_bits_read_uvlc(struct mpeg4_bits_t* bits)
{
	uint64_t value;
	int leadingZeros;
	for (leadingZeros = 0; !mpeg4_bits_read(bits); ++leadingZeros)
	{
	}

	if (leadingZeros >= 32)
		return (1ULL << 32) - 1;
	
	value = mpeg4_bits_read_n(bits, leadingZeros);
	return (1ULL << leadingZeros) - 1 + value;
}

static inline uint64_t mpeg4_bits_read_latm(struct mpeg4_bits_t* bits)
{
	int len;
	len = (int)mpeg4_bits_read_n(bits, 2);
	return mpeg4_bits_read_n(bits, (len + 1) * 8);
}

/// write 1-bit
/// @param[in] v write 0 if v value 0, other, write 1
static inline int mpeg4_bits_write(struct mpeg4_bits_t* bits, int v)
{
	assert(bits && bits->data && bits->size > 0);
	if (bits->bits >= bits->size * 8)
	{
		bits->error = -1;
		return -1; // throw exception
	}

	if(v)
		bits->data[bits->bits / 8] |= (0x80U >> (bits->bits % 8));
	bits->bits += 1; // update offset
	return 0;
}

static inline int mpeg4_bits_write_n(struct mpeg4_bits_t* bits, uint64_t v, int n)
{
	int m;
	size_t i;

	assert(n > 0 && n <= 64);
	assert(bits && bits->data && bits->size > 0);
	if (bits->bits + n > bits->size * 8 || n > 64 || n < 0)
	{
		bits->error = -1;
		return -1; // throw exception
	}

	m = n;
	v = v << (64 - n); // left shift to first bit

	bits->data[bits->bits / 8] |= v >> (56 + (bits->bits % 8)); // remain valid value
	v <<= 8 - (bits->bits % 8);
	n -= 8 - (int)(bits->bits % 8);

	for (i = 1; n > 0; i++)
	{
		assert(bits->bits / 8 + i < bits->size);
		bits->data[bits->bits / 8 + i] = (uint8_t)(v >> 56);
		v <<= 8;
		n -= 8;
	}

	bits->bits += m;
	return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* !_mpeg4_bits_h_ */
