#include "rtmp-internal.h"
#include "rtmp-msgtypeid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int rtmp_audio(struct rtmp_t* rtmp, struct rtmp_chunk_header_t* header, const uint8_t* payload)
{
	return rtmp->onaudio(rtmp->param, payload, header->length, header->timestamp);
}

static int rtmp_video(struct rtmp_t* rtmp, struct rtmp_chunk_header_t* header, const uint8_t* payload)
{
	return rtmp->onvideo(rtmp->param, payload, header->length, header->timestamp);
}

static int rtmp_script(struct rtmp_t* rtmp, struct rtmp_chunk_header_t* header, const uint8_t* payload)
{
	// filter @setDataFrame
	const static uint8_t s_setFrameData[] = { 0x02, 0x00, 0x0d, 0x40, 0x73, 0x65, 0x74, 0x44, 0x61, 0x74, 0x61, 0x46, 0x72, 0x61, 0x6d, 0x65 };
	if (header->length > sizeof(s_setFrameData) && 0 == memcmp(s_setFrameData, payload, sizeof(s_setFrameData)))
		return rtmp->onscript(rtmp->param, payload + sizeof(s_setFrameData), header->length - sizeof(s_setFrameData), header->timestamp);

	return rtmp->onscript(rtmp->param, payload, header->length, header->timestamp);
}

int rtmp_handler(struct rtmp_t* rtmp, struct rtmp_chunk_header_t* header, const uint8_t* payload)
{
	switch (header->type)
	{
	case RTMP_TYPE_FLEX_MESSAGE:
		// filter AMF3 0x00
		payload += (header->length > 0) ? 1 : 0; // fix header->length = 0
		header->length -= (header->length > 0) ? 1 : 0;
		return rtmp_invoke_handler(rtmp, header, payload);

	case RTMP_TYPE_INVOKE:
		return rtmp_invoke_handler(rtmp, header, payload);

	case RTMP_TYPE_VIDEO:
		return rtmp_video(rtmp, header, payload);

	case RTMP_TYPE_AUDIO:
		return rtmp_audio(rtmp, header, payload);

	case RTMP_TYPE_EVENT:
		// User Control Message Events
		return 0 == rtmp_event_handler(rtmp, header, payload) ? -1 : 0;

		// Protocol Control Messages
	case RTMP_TYPE_SET_CHUNK_SIZE:
	case RTMP_TYPE_ABORT:
	case RTMP_TYPE_ACKNOWLEDGEMENT:
	case RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE:
	case RTMP_TYPE_SET_PEER_BANDWIDTH:
		return 0 == rtmp_control_handler(rtmp, header, payload) ? -1 : 0;

	case RTMP_TYPE_DATA:
	case RTMP_TYPE_FLEX_STREAM:
		// play -> RtmpSampleAccess
		// finish -> onPlayStatus("NetStream.Play.Complete")
		return rtmp_script(rtmp, header, payload);

	case RTMP_TYPE_SHARED_OBJECT:
	case RTMP_TYPE_FLEX_OBJECT:
		break;

	case RTMP_TYPE_METADATA:
		break;

	default:
		assert(0);
		printf("%s: unknown rtmp header type: %d\n", __FUNCTION__, (int)header->type);
		break;
	}

	return 0;
}
