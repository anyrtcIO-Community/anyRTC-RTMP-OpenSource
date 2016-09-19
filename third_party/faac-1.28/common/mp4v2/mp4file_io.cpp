/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 */

#include "mp4common.h"

// MP4File low level IO support

u_int64_t MP4File::GetPosition(FILE* pFile)
{
	if (m_memoryBuffer == NULL) {
		if (pFile == NULL) {
			ASSERT(m_pFile);
			u_int64_t fpos;
			if (m_virtual_IO->GetPosition(m_pFile, &fpos) != 0) {
				throw new MP4Error("getting position via Virtual I/O", "MP4GetPosition");
			}
			return fpos;
		} else {
			fpos_t fpos;
			if (fgetpos(pFile, &fpos) < 0) { 
				throw new MP4Error(errno, "MP4GetPosition");
			}
			uint64_t ret;
			FPOS_TO_VAR(fpos, uint64_t, ret);
			return ret;
		}
	} else {
		return m_memoryBufferPosition;
	}
}

void MP4File::SetPosition(u_int64_t pos, FILE* pFile)
{
	if (m_memoryBuffer == NULL) {
		if (pFile == NULL) {
			ASSERT(m_pFile);
			if (m_virtual_IO->SetPosition(m_pFile, pos) != 0) {
				throw new MP4Error("setting position via Virtual I/O", "MP4SetPosition");
			}
		}	else {
			fpos_t fpos;
			VAR_TO_FPOS(fpos, pos);
			if (fsetpos(pFile, &fpos) < 0) { 
				throw new MP4Error(errno, "MP4SetPosition");
			}
		}
	} else {
		if (pos >= m_memoryBufferSize) {
		  //		  abort();
			throw new MP4Error("position out of range", "MP4SetPosition");
		}
		m_memoryBufferPosition = pos;
	}
}

u_int64_t MP4File::GetSize()
{
	if (m_mode == 'w') {
		// we're always positioned at the end of file in write mode
		// except for short intervals in ReadSample and FinishWrite routines
		// so we rely on the faster approach of GetPosition()
		// instead of flushing to disk, and then stat'ing the file
		m_fileSize = GetPosition();
	} // else read mode, fileSize was determined at Open()

	return m_fileSize;
}

void MP4File::ReadBytes(u_int8_t* pBytes, u_int32_t numBytes, FILE* pFile)
{
	// handle degenerate cases
	if (numBytes == 0) {
		return;
	}

	ASSERT(pBytes);
	WARNING(m_numReadBits > 0);

	if (m_memoryBuffer == NULL) {
		if (pFile == NULL) {
			ASSERT(m_pFile);
			if (m_virtual_IO->Read(m_pFile, pBytes, numBytes) != numBytes) {
				throw new MP4Error("not enough bytes, reached end-of-file",		"MP4ReadBytes");
			}
		}	else {
			if (fread(pBytes, 1, numBytes, pFile) != numBytes) { 
				if (feof(pFile)) { 
					throw new MP4Error(
						"not enough bytes, reached end-of-file",
						"MP4ReadBytes");
				} else {
					throw new MP4Error(errno, "MP4ReadBytes");
				}
			}
		}
	} else {
		if (m_memoryBufferPosition + numBytes > m_memoryBufferSize) {
			throw new MP4Error(
				"not enough bytes, reached end-of-memory",
				"MP4ReadBytes");
		}
		memcpy(pBytes, &m_memoryBuffer[m_memoryBufferPosition], numBytes);
		m_memoryBufferPosition += numBytes;
	}
	return;
}

void MP4File::PeekBytes(u_int8_t* pBytes, u_int32_t numBytes, FILE* pFile)
{
	u_int64_t pos = GetPosition(pFile);
	ReadBytes(pBytes, numBytes, pFile);
	SetPosition(pos, pFile);
}

void MP4File::EnableMemoryBuffer(u_int8_t* pBytes, u_int64_t numBytes) 
{
	ASSERT(m_memoryBuffer == NULL);

	if (pBytes) {
		m_memoryBuffer = pBytes;
		m_memoryBufferSize = numBytes;
	} else {
		if (numBytes) {	
			m_memoryBufferSize = numBytes;
		} else {
			m_memoryBufferSize = 4096;
		}
		m_memoryBuffer = (u_int8_t*)MP4Malloc(m_memoryBufferSize);
	}
	m_memoryBufferPosition = 0;
}

void MP4File::DisableMemoryBuffer(u_int8_t** ppBytes, u_int64_t* pNumBytes) 
{
	ASSERT(m_memoryBuffer != NULL);

	if (ppBytes) {
		*ppBytes = m_memoryBuffer;
	}
	if (pNumBytes) {
		*pNumBytes = m_memoryBufferPosition;
	}

	m_memoryBuffer = NULL;
	m_memoryBufferSize = 0;
	m_memoryBufferPosition = 0;
}

void MP4File::WriteBytes(u_int8_t* pBytes, u_int32_t numBytes, FILE* pFile)
{
	ASSERT(m_numWriteBits == 0 || m_numWriteBits >= 8);

	if (pBytes == NULL || numBytes == 0) {
		return;
	}

	if (m_memoryBuffer == NULL) {
		if (pFile == NULL) {
			ASSERT(m_pFile);
			if (m_virtual_IO->Write(m_pFile, pBytes, numBytes) != numBytes) {
				throw new MP4Error("error writing bytes via virtual I/O", "MP4WriteBytes");
			}
		} else {
			u_int32_t rc = fwrite(pBytes, 1, numBytes, pFile); 
			if (rc != numBytes) {
				throw new MP4Error(errno, "MP4WriteBytes");
			}
		}
	} else {
		if (m_memoryBufferPosition + numBytes > m_memoryBufferSize) {
			m_memoryBufferSize = 2 * (m_memoryBufferSize + numBytes);
			m_memoryBuffer = (u_int8_t*)
				MP4Realloc(m_memoryBuffer, m_memoryBufferSize);
		}
		memcpy(&m_memoryBuffer[m_memoryBufferPosition], pBytes, numBytes);
		m_memoryBufferPosition += numBytes;
	}
}

u_int64_t MP4File::ReadUInt(u_int8_t size)
{
	switch (size) {
	case 1:
		return ReadUInt8();
	case 2:
		return ReadUInt16();
	case 3:
		return ReadUInt24();
	case 4:
		return ReadUInt32();
	case 8:
		return ReadUInt64();
	default:
		ASSERT(false);
		return 0;
	}
}

#if 0
void MP4File::WriteUInt(u_int64_t value, u_int8_t size)
{
	switch (size) {
	case 1:
		WriteUInt8(value);
	case 2:
		WriteUInt16(value);
	case 3:
		WriteUInt24(value);
	case 4:
		WriteUInt32(value);
	case 8:
		WriteUInt64(value);
	default:
		ASSERT(false);
	}
}
#endif

u_int8_t MP4File::ReadUInt8()
{
	u_int8_t data;
	ReadBytes(&data, 1);
	return data;
}

void MP4File::WriteUInt8(u_int8_t value)
{
	WriteBytes(&value, 1);
}

u_int16_t MP4File::ReadUInt16()
{
	u_int8_t data[2];
	ReadBytes(&data[0], 2);
	return ((data[0] << 8) | data[1]);
}

void MP4File::WriteUInt16(u_int16_t value)
{
	u_int8_t data[2];
	data[0] = (value >> 8) & 0xFF;
	data[1] = value & 0xFF;
	WriteBytes(data, 2);
}

u_int32_t MP4File::ReadUInt24()
{
	u_int8_t data[3];
	ReadBytes(&data[0], 3);
	return ((data[0] << 16) | (data[1] << 8) | data[2]);
}

void MP4File::WriteUInt24(u_int32_t value)
{
	u_int8_t data[3];
	data[0] = (value >> 16) & 0xFF;
	data[1] = (value >> 8) & 0xFF;
	data[2] = value & 0xFF;
	WriteBytes(data, 3);
}

u_int32_t MP4File::ReadUInt32()
{
	u_int8_t data[4];
	ReadBytes(&data[0], 4);
	return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
}

void MP4File::WriteUInt32(u_int32_t value)
{
	u_int8_t data[4];
	data[0] = (value >> 24) & 0xFF;
	data[1] = (value >> 16) & 0xFF;
	data[2] = (value >> 8) & 0xFF;
	data[3] = value & 0xFF;
	WriteBytes(data, 4);
}

u_int64_t MP4File::ReadUInt64()
{
	u_int8_t data[8];
	u_int64_t result = 0;
	u_int64_t temp;

	ReadBytes(&data[0], 8);
	
	for (int i = 0; i < 8; i++) {
		temp = data[i];
		result |= temp << ((7 - i) * 8);
	}
	return result;
}

void MP4File::WriteUInt64(u_int64_t value)
{
	u_int8_t data[8];

	for (int i = 7; i >= 0; i--) {
		data[i] = value & 0xFF;
		value >>= 8;
	}
	WriteBytes(data, 8);
}

float MP4File::ReadFixed16()
{
	u_int8_t iPart = ReadUInt8();
	u_int8_t fPart = ReadUInt8();

	return iPart + (((float)fPart) / 0x100);
}

void MP4File::WriteFixed16(float value)
{
	if (value >= 0x100) {
		throw new MP4Error(ERANGE, "MP4WriteFixed16");
	}

	u_int8_t iPart = (u_int8_t)value;
	u_int8_t fPart = (u_int8_t)((value - iPart) * 0x100);

	WriteUInt8(iPart);
	WriteUInt8(fPart);
}

float MP4File::ReadFixed32()
{
	u_int16_t iPart = ReadUInt16();
	u_int16_t fPart = ReadUInt16();

	return iPart + (((float)fPart) / 0x10000);
}

void MP4File::WriteFixed32(float value)
{
	if (value >= 0x10000) {
		throw new MP4Error(ERANGE, "MP4WriteFixed32");
	}

	u_int16_t iPart = (u_int16_t)value;
	u_int16_t fPart = (u_int16_t)((value - iPart) * 0x10000);

	WriteUInt16(iPart);
	WriteUInt16(fPart);
}

float MP4File::ReadFloat()
{
	union {
		float f;
		u_int32_t i;
	} u;

	u.i = ReadUInt32();
	return u.f;
}

void MP4File::WriteFloat(float value)
{
	union {
		float f;
		u_int32_t i;
	} u;

	u.f = value;
	WriteUInt32(u.i);
}

char* MP4File::ReadString()
{
	u_int32_t length = 0;
	u_int32_t alloced = 64;
	char* data = (char*)MP4Malloc(alloced);

	do {
		if (length == alloced) {
			data = (char*)MP4Realloc(data, alloced * 2);
			if (data == NULL) return NULL;
			alloced *= 2;
		}
		ReadBytes((u_int8_t*)&data[length], 1);
		length++;
	} while (data[length - 1] != 0);

	data = (char*)MP4Realloc(data, length);
	return data;
}

void MP4File::WriteString(char* string)
{
	if (string == NULL) {
		u_int8_t zero = 0;
		WriteBytes(&zero, 1);
	} else {
		WriteBytes((u_int8_t*)string, strlen(string) + 1);
	}
}

char* MP4File::ReadCountedString(u_int8_t charSize, bool allowExpandedCount)
{
	u_int32_t charLength;
	if (allowExpandedCount) {
		u_int8_t b;
		uint ix = 0;
		charLength = 0;
		do {
			b = ReadUInt8();
			charLength += b;
			ix++;
			if (ix > 25) 
			  throw new MP4Error(ERANGE, 
					     "Counted string too long 25 * 255");
		} while (b == 255);
	} else {
		charLength = ReadUInt8();
	}

	u_int32_t byteLength = charLength * charSize;
	char* data = (char*)MP4Malloc(byteLength + 1);
	if (byteLength > 0) {
		ReadBytes((u_int8_t*)data, byteLength);
	}
	data[byteLength] = '\0';
	return data;
}

void MP4File::WriteCountedString(char* string, 
	u_int8_t charSize, bool allowExpandedCount)
{
	u_int32_t byteLength;
	if (string) {
		byteLength = strlen(string);
	} else {
		byteLength = 0;
	}
	u_int32_t charLength = byteLength / charSize;

	if (allowExpandedCount) {
		while (charLength >= 0xFF) {
			WriteUInt8(0xFF);
			charLength -= 0xFF;
		}		
		WriteUInt8(charLength);
	} else {
		if (charLength > 255) {
			throw new MP4Error(ERANGE, "Length is %d", "MP4WriteCountedString", charLength);
		}
		WriteUInt8(charLength);
	}

	if (byteLength > 0) {
		WriteBytes((u_int8_t*)string, byteLength);
	}
}

u_int64_t MP4File::ReadBits(u_int8_t numBits)
{
	ASSERT(numBits > 0);
	ASSERT(numBits <= 64);

	u_int64_t bits = 0;

	for (u_int8_t i = numBits; i > 0; i--) {
		if (m_numReadBits == 0) {
			ReadBytes(&m_bufReadBits, 1);
			m_numReadBits = 8;
		}
		bits = (bits << 1) | ((m_bufReadBits >> (--m_numReadBits)) & 1);
	}

	return bits;
}

void MP4File::FlushReadBits()
{
	// eat any remaining bits in the read buffer
	m_numReadBits = 0;
}

void MP4File::WriteBits(u_int64_t bits, u_int8_t numBits)
{
	ASSERT(numBits <= 64);

	for (u_int8_t i = numBits; i > 0; i--) {
		m_bufWriteBits |= 
			(((bits >> (i - 1)) & 1) << (8 - ++m_numWriteBits));
	
		if (m_numWriteBits == 8) {
			FlushWriteBits();
		}
	}
}

void MP4File::PadWriteBits(u_int8_t pad)
{
	if (m_numWriteBits) {
		WriteBits(pad ? 0xFF : 0x00, 8 - m_numWriteBits);
	}
}

void MP4File::FlushWriteBits()
{
	if (m_numWriteBits > 0) {
		WriteBytes(&m_bufWriteBits, 1);
		m_numWriteBits = 0;
		m_bufWriteBits = 0;
	}
}

u_int32_t MP4File::ReadMpegLength()
{
	u_int32_t length = 0;
	u_int8_t numBytes = 0;
	u_int8_t b;

	do {
		b = ReadUInt8();
		length = (length << 7) | (b & 0x7F);
		numBytes++;
	} while ((b & 0x80) && numBytes < 4);

	return length;
}

void MP4File::WriteMpegLength(u_int32_t value, bool compact)
{
	if (value > 0x0FFFFFFF) {
		throw new MP4Error(ERANGE, "MP4WriteMpegLength");
	}

	int8_t numBytes;

	if (compact) {
		if (value <= 0x7F) {
			numBytes = 1;
		} else if (value <= 0x3FFF) {
			numBytes = 2;
		} else if (value <= 0x1FFFFF) {
			numBytes = 3;
		} else {
			numBytes = 4;
		}
	} else {
		numBytes = 4;
	}

	int8_t i = numBytes;
	do {
		i--;
		u_int8_t b = (value >> (i * 7)) & 0x7F;
		if (i > 0) {
			b |= 0x80;
		}
		WriteUInt8(b);
	} while (i > 0);
}

