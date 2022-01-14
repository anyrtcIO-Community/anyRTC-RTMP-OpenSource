#include "rtmp-handshake.h"
#include "rtmp-util.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#if defined(_OPENSSL_)
#include <openssl/sha.h>
#include <openssl/hmac.h>
#define _FLASH_HANDSHAKE_
#elif defined(_IETF_HMAC_)
#include "sha.h"
#define SHA256_DIGEST_LENGTH SHA256HashSize
#define _FLASH_HANDSHAKE_
#endif

#if defined(_FLASH_HANDSHAKE_)
// nginx-rtmp-module / ngx_rtmp_handshake.c

static const uint8_t rtmp_server_key[] = {
	'G', 'e', 'n', 'u', 'i', 'n', 'e', ' ', 'A', 'd', 'o', 'b', 'e', ' ',
	'F', 'l', 'a', 's', 'h', ' ', 'M', 'e', 'd', 'i', 'a', ' ',
	'S', 'e', 'r', 'v', 'e', 'r', ' ',
	'0', '0', '1', /* Genuine Adobe Flash Media Server 001 */

	0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8, 0x2E, 0x00, 0xD0, 0xD1,
	0x02, 0x9E, 0x7E, 0x57, 0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80, 0x6F, 0xAB,
	0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE
};

static const uint8_t rtmp_client_key[] = {
	'G', 'e', 'n', 'u', 'i', 'n', 'e', ' ', 'A', 'd', 'o', 'b', 'e', ' ',
	'F', 'l', 'a', 's', 'h', ' ', 'P', 'l', 'a', 'y', 'e', 'r', ' ',
	'0', '0', '1', /* Genuine Adobe Flash Player 001 */

	0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8, 0x2E, 0x00, 0xD0, 0xD1,
	0x02, 0x9E, 0x7E, 0x57, 0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80, 0x6F, 0xAB,
	0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE
};

static const uint8_t rtmp_server_version[] = {
	0x0D, 0x0E, 0x0A, 0x0D
};

static const uint8_t rtmp_client_version[] = {
	0x0C, 0x00, 0x0D, 0x0E
};

#if defined(_OPENSSL_)
static int rtmp_handshake_make_digest(const uint8_t* key, size_t len, const uint8_t* ptr, size_t ptrlen, const uint8_t* digest, uint8_t* dst)
{
	static HMAC_CTX        *hmac;
	unsigned int            digestlen;

	if (hmac == NULL) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		static HMAC_CTX  shmac;
		hmac = &shmac;
		HMAC_CTX_init(hmac);
#else
		hmac = HMAC_CTX_new();
#endif
	}

	HMAC_Init_ex(hmac, key, len, EVP_sha256(), NULL);
	if (digest)
	{
		assert(digest + SHA256_DIGEST_LENGTH <= ptr + ptrlen);
		HMAC_Update(hmac, ptr, digest - ptr);
		if (digest + SHA256_DIGEST_LENGTH < ptr + ptrlen)
			HMAC_Update(hmac, digest + SHA256_DIGEST_LENGTH, ptrlen - (digest - ptr) - SHA256_DIGEST_LENGTH);
	}
	else
	{
		HMAC_Update(hmac, ptr, ptrlen);
	}
	HMAC_Final(hmac, dst, &digestlen);
	assert(digestlen == SHA256_DIGEST_LENGTH);
	return 0;
}
#else
static int rtmp_handshake_make_digest(const uint8_t* key, size_t len, const uint8_t* ptr, size_t ptrlen, const uint8_t* digest, uint8_t* dst)
{
	HMACContext hmac;
	hmacReset(&hmac, SHA256, key, len);
	if (digest)
	{
		assert(digest + SHA256_DIGEST_LENGTH <= ptr + ptrlen);
		hmacInput(&hmac, ptr, digest - ptr);
		if (digest + SHA256_DIGEST_LENGTH < ptr + ptrlen)
			hmacInput(&hmac, digest + SHA256_DIGEST_LENGTH, ptrlen - (digest - ptr) - SHA256_DIGEST_LENGTH);
	}
	else
	{
		hmacInput(&hmac, ptr, ptrlen);
	}
	hmacResult(&hmac, dst);
	return 0;
}
#endif

/*
// http://blog.csdn.net/win_lin/article/details/13006803
// 764-bytes key
random-data: (offset)bytes
key-data: 128bytes
random-data: (764-offset-128-4)bytes
offset: 4bytes

// 764-bytes digest
offset: 4bytes
random-data: (offset)bytes
digest-data: 32bytes
random-data: (764-4-offset-32)bytes
*/
static const uint8_t* rtmp_handshake_find_digest(const uint8_t* handshake, size_t offset, const uint8_t* key, size_t len)
{
	uint32_t bytes;
	uint8_t digest[SHA256_DIGEST_LENGTH];

	bytes = handshake[offset + 0];
	bytes += handshake[offset + 1];
	bytes += handshake[offset + 2];
	bytes += handshake[offset + 3];
	bytes %= 728;/*764 - 4bytesoffset - 32bytesdigest*/

	rtmp_handshake_make_digest(key, len, handshake, RTMP_HANDSHAKE_SIZE, handshake + offset + 4 + bytes, digest);
	if (0 == memcmp(digest, handshake + offset + 4 + bytes, SHA256_DIGEST_LENGTH))
		return handshake + offset + 4 + bytes;
	return NULL;
}

static int rtmp_handshake_parse_challenge(const uint8_t* handshake, const uint8_t* key, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH])
{
	uint32_t epoch;
	uint32_t version;
	const uint8_t* p;

	be_read_uint32(handshake, &epoch);
	be_read_uint32(handshake + 4, &version);

	if (0 == version)
	{
		// simple handshake
		return 0;
	}

	p = rtmp_handshake_find_digest(handshake, 764 + 8, key, len);
	if(NULL == p)
		p = rtmp_handshake_find_digest(handshake, 8, key, len);

	if (p)
		memcpy(digest, p, SHA256_DIGEST_LENGTH);
	return p ? 1 : 0;
}

static int rtmp_handshake_create_challenge(uint8_t* handshake, const uint8_t* key, size_t len)
{
	uint32_t offset;
	uint8_t* digest;
	
	offset = handshake[8];
	offset += handshake[9];
	offset += handshake[10];
	offset += handshake[11];
	digest = handshake + 12 + (offset % 728/*764 - 4bytesoffset - 32bytesdigest*/);

	return rtmp_handshake_make_digest(key, len, handshake, RTMP_HANDSHAKE_SIZE, digest, digest);
}

static int rtmp_handshake_create_response(uint8_t* handshake, const uint8_t* key, size_t len)
{
	uint8_t digest[SHA256_DIGEST_LENGTH];
	rtmp_handshake_make_digest(key, len, handshake, RTMP_HANDSHAKE_SIZE - SHA256_DIGEST_LENGTH, NULL, digest);
	memcpy(handshake + RTMP_HANDSHAKE_SIZE - SHA256_DIGEST_LENGTH, digest, SHA256_DIGEST_LENGTH);
	return 0;
}

#endif

static void rtmp_handshake_random(uint8_t* p, uint32_t timestamp)
{
	int i;
	
	srand(timestamp);
	for (i = 0; i * 4 < RTMP_HANDSHAKE_SIZE - 8; i++)
	{
		*((int*)p + i) = rand();
	}
}

int rtmp_handshake_c0(uint8_t* c0, int version)
{
	assert(RTMP_VERSION == version);
	*c0 = (uint8_t)version;
	return 1;
}

int rtmp_handshake_c1(uint8_t* c1, uint32_t timestamp)
{
	be_write_uint32(c1, timestamp);
#if defined(_FLASH_HANDSHAKE_)
	memcpy(c1 + 4, rtmp_client_version, 4);
	rtmp_handshake_random(c1 + 8, timestamp);
	rtmp_handshake_create_challenge(c1, rtmp_client_key, 30);
#else
	be_write_uint32(c1 + 4, 0);
	rtmp_handshake_random(c1 + 8, timestamp);
#endif
	return RTMP_HANDSHAKE_SIZE;
}

int rtmp_handshake_c2(uint8_t* c2, uint32_t timestamp, const uint8_t* s1, size_t bytes)
{
#if defined(_FLASH_HANDSHAKE_)
	uint8_t digest[SHA256_DIGEST_LENGTH];
	assert(RTMP_HANDSHAKE_SIZE == bytes);
	if (1 == rtmp_handshake_parse_challenge(s1, rtmp_server_key, 36, digest))
	{
		rtmp_handshake_make_digest(rtmp_client_key, sizeof(rtmp_client_key), digest, SHA256_DIGEST_LENGTH, NULL, digest);
		rtmp_handshake_random(c2, timestamp);
		rtmp_handshake_create_response(c2, digest, SHA256_DIGEST_LENGTH);
	}
	else
	{
		memmove(c2, s1, bytes);
		//be_write_uint32(c2 + 4, timestamp);
	}
#else
	assert(RTMP_HANDSHAKE_SIZE == bytes);
	memmove(c2, s1, bytes);
	be_write_uint32(c2 + 4, timestamp);
#endif
	return (int)bytes;
}

int rtmp_handshake_s0(uint8_t* s0, int version)
{
	assert(RTMP_VERSION == version);
	*s0 = (uint8_t)version;
	return 1;
}

int rtmp_handshake_s1(uint8_t* s1, uint32_t timestamp)
{
	be_write_uint32(s1, timestamp);
#if defined(_FLASH_HANDSHAKE_)
	memcpy(s1 + 4, rtmp_server_version, 4);
	rtmp_handshake_random(s1 + 8, timestamp);
	rtmp_handshake_create_challenge(s1, rtmp_server_key, 36);
#else
	be_write_uint32(s1 + 4, 0);
	rtmp_handshake_random(s1 + 8, timestamp);
#endif
	return RTMP_HANDSHAKE_SIZE;
}

int rtmp_handshake_s2(uint8_t* s2, uint32_t timestamp, const uint8_t* c1, size_t bytes)
{
#if defined(_FLASH_HANDSHAKE_)
	uint8_t digest[SHA256_DIGEST_LENGTH];
	assert(RTMP_HANDSHAKE_SIZE == bytes);
	if (1 == rtmp_handshake_parse_challenge(c1, rtmp_client_key, 30, digest))
	{
		rtmp_handshake_make_digest(rtmp_server_key, sizeof(rtmp_server_key), digest, SHA256_DIGEST_LENGTH, NULL, digest);
		rtmp_handshake_random(s2, timestamp);
		rtmp_handshake_create_response(s2, digest, SHA256_DIGEST_LENGTH);
	}
	else
	{
		memmove(s2, c1, bytes);
		//be_write_uint32(s2 + 4, timestamp);
	}
#else
	assert(RTMP_HANDSHAKE_SIZE == bytes);
	memmove(s2, c1, bytes);
	be_write_uint32(s2 + 4, timestamp);
#endif
	return (int)bytes;
}
