#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__
#include <stdint.h>
#if defined(WEBRTC_POSIX) && !defined(__native_client__)
#include <arpa/inet.h>
#endif

// Processor architecture detection.  For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define WEBRTC_ARCH_X86_FAMILY
#define WEBRTC_ARCH_X86_64
#define WEBRTC_ARCH_64_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(__aarch64__)
#define WEBRTC_ARCH_ARM_FAMILY
#define WEBRTC_ARCH_64_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(_M_IX86) || defined(__i386__)
#define WEBRTC_ARCH_X86_FAMILY
#define WEBRTC_ARCH_X86
#define WEBRTC_ARCH_32_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(__ARMEL__)
#define WEBRTC_ARCH_ARM_FAMILY
#define WEBRTC_ARCH_32_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(__MIPSEL__)
#define WEBRTC_ARCH_MIPS_FAMILY
#if defined(__LP64__)
#define WEBRTC_ARCH_64_BITS
#else
#define WEBRTC_ARCH_32_BITS
#endif
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(__pnacl__)
#define WEBRTC_ARCH_32_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#elif defined(__EMSCRIPTEN__)
#define WEBRTC_ARCH_32_BITS
#define WEBRTC_ARCH_LITTLE_ENDIAN
#else
#error Please add support for your architecture in rtc_base/system/arch.h
#endif


#if defined(WEBRTC_MAC) || defined(WEBRTC_IOS)
#include <libkern/OSByteOrder.h>

#define htobe16(v) OSSwapHostToBigInt16(v)
#define htobe32(v) OSSwapHostToBigInt32(v)
#define htobe64(v) OSSwapHostToBigInt64(v)
#define be16toh(v) OSSwapBigToHostInt16(v)
#define be32toh(v) OSSwapBigToHostInt32(v)
#define be64toh(v) OSSwapBigToHostInt64(v)

#define htole16(v) OSSwapHostToLittleInt16(v)
#define htole32(v) OSSwapHostToLittleInt32(v)
#define htole64(v) OSSwapHostToLittleInt64(v)
#define le16toh(v) OSSwapLittleToHostInt16(v)
#define le32toh(v) OSSwapLittleToHostInt32(v)
#define le64toh(v) OSSwapLittleToHostInt64(v)

#elif defined(WEBRTC_WIN) || defined(__native_client__)

#if defined(WEBRTC_WIN)
#include <stdlib.h>
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif  // defined(WEBRTC_WIN)

#if defined(WEBRTC_ARCH_LITTLE_ENDIAN)
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)
#define htole16(v) (v)
#define htole32(v) (v)
#define htole64(v) (v)
#define le16toh(v) (v)
#define le32toh(v) (v)
#define le64toh(v) (v)
#if defined(WEBRTC_WIN)
#define htobe64(v) _byteswap_uint64(v)
#define be64toh(v) _byteswap_uint64(v)
#endif  // defined(WEBRTC_WIN)
#if defined(__native_client__)
#define htobe64(v) __builtin_bswap64(v)
#define be64toh(v) __builtin_bswap64(v)
#endif  // defined(__native_client__)

#elif defined(WEBRTC_ARCH_BIG_ENDIAN)
#define htobe16(v) (v)
#define htobe32(v) (v)
#define be16toh(v) (v)
#define be32toh(v) (v)
#define htole16(v) __builtin_bswap16(v)
#define htole32(v) __builtin_bswap32(v)
#define htole64(v) __builtin_bswap64(v)
#define le16toh(v) __builtin_bswap16(v)
#define le32toh(v) __builtin_bswap32(v)
#define le64toh(v) __builtin_bswap64(v)
#if defined(WEBRTC_WIN)
#define htobe64(v) (v)
#define be64toh(v) (v)
#endif  // defined(WEBRTC_WIN)
#if defined(__native_client__)
#define htobe64(v) (v)
#define be64toh(v) (v)
#endif  // defined(__native_client__)
#else
#error WEBRTC_ARCH_BIG_ENDIAN or WEBRTC_ARCH_LITTLE_ENDIAN must be defined.
#endif  // defined(WEBRTC_ARCH_LITTLE_ENDIAN)

#elif defined(WEBRTC_POSIX)
#include <endian.h>
#else
#error "Missing byte order functions for this arch."
#endif  // defined(WEBRTC_MAC)


inline void SetBE32(void* memory, uint32_t v) {
	*static_cast<uint32_t*>(memory) = htobe32(v);
}

#endif
