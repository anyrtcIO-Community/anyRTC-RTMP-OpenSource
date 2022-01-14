#ifndef _H264_SEI_PACK_H_INCLUDE
#define _H264_SEI_PACK_H_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

enum sei_payload_type_e
{
    /* libx264 raw enum */
    SEI_BUFFERING_PERIOD = 0,
    SEI_PIC_TIMING = 1,
    SEI_PAN_SCAN_RECT = 2,
    SEI_FILLER = 3,
    SEI_USER_DATA_REGISTERED = 4,
    SEI_USER_DATA_UNREGISTERED = 5,
    SEI_RECOVERY_POINT = 6,
    SEI_DEC_REF_PIC_MARKING = 7,
    SEI_FRAME_PACKING = 45,

    /* self define */
    SEI_SELF_DEFINE_LAYOUT = 100,
    SEI_SELF_DEFINE_242 = 242,
};


int h264_insert_sei(char* pMemBuffer, char* p264Data, int n264Len, char* payload, int payload_size, int payload_type);



#endif /* _H264_SEI_PACK_H_INCLUDE */
