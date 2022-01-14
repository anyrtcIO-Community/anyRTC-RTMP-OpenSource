#include "H264SeiPack.h"

//¿ªÊ¼Âë
static unsigned char start_code3[] = { 0x00,0x00,0x01 };
static unsigned char start_code[] = { 0x00,0x00,0x00,0x01 };

void h264_sei_pack_internal(uint8_t* sei, int* len, uint8_t* payload, int payload_size, int payload_type)
{
	static unsigned char start_code[] = { 0x00, 0x00, 0x00, 0x01 };

	int i, index = 0;

	/* start code */
	memcpy(sei + index, start_code, 4);
	index += 4;

	/* nalu type */
	sei[index++] = 6;

	/* sei payload type */
	for (i = 0; i <= payload_type - 255; i += 255) {
		sei[index++] = 255;
	}
	sei[index++] = payload_type - i;

	/* sei payload size */
	for (i = 0; i <= payload_size - 255; i += 255) {
		sei[index++] = 255;
	}
	sei[index++] = payload_size - i;

	/* sei content */
	for (i = 0; i < payload_size; i++) {
		sei[index++] = payload[i];
	}

	/* sei rbsp_trailing */
	sei[index++] = 0x80;
	*len = index;
}

int h264_insert_sei(char* pMemBuffer, char* p264Data, int n264Len, char* payload, int payload_size, int payload_type)
{
	int nRet = 0;
	char* ptr = p264Data;
	int limLen = 0;
	int ii = 0; 
	char* ptrFind7 = NULL;
	char* ptrFind8 = NULL;
	char* ptrFind6 = NULL;
	char* ptrFind5 = NULL;
	while (ii <= n264Len - 4) {
		limLen = 0;
		if (ptr[ii] == 0 && ptr[ii + 1] == 0 && ptr[ii + 2] == 0 && ptr[ii + 3] == 1) {
			limLen = 4;
		}
		else if (ptr[ii] == 0 && ptr[ii + 1] == 0 &&  ptr[ii + 2] == 1) {
			limLen = 3;
		}
		if (limLen > 0) {
			ii += limLen;
			int nType = ptr[ii] & 0x1f;
			if (nType == 7) {
				ptrFind7 = ptr + ii - limLen;
			}
			else if (nType == 8) {
				ptrFind8 = ptr + ii - limLen;
			}
			else if (nType == 6) {
				ptrFind6 = ptr + ii - limLen;
			}
			else if (nType == 5) {
				ptrFind5 = ptr + ii - limLen;
				if (ptrFind7 != NULL && ptrFind8 != NULL) {
					memcpy(pMemBuffer, ptrFind7, (ptrFind8 - ptrFind7));
					nRet += (ptrFind8 - ptrFind7);

					if (ptrFind6 != NULL) {
						memcpy(pMemBuffer + nRet, ptrFind8, (ptrFind6 - ptrFind8));
						nRet += (ptrFind6 - ptrFind8);
					}
					else {
						memcpy(pMemBuffer + nRet, ptrFind8, (ptrFind5 - ptrFind8));
						nRet += (ptrFind5 - ptrFind8);
					}
				}
				int wrSei = 0;
				h264_sei_pack_internal((uint8_t*)pMemBuffer + nRet, &wrSei, (uint8_t*)payload, payload_size, payload_type);
				nRet += wrSei;
				memcpy(pMemBuffer + nRet, ptrFind5, (n264Len - (ptrFind5 - ptr)));
				nRet += (n264Len - (ptrFind5 - ptr));
				break;
			}
		}
		else {
			ii++;
		}
	}
	
	return nRet;
}