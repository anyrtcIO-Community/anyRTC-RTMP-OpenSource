#include "rtmp-internal.h"
#include "amf0.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rtmp-client-invoke-handler.h"

// connect request parser
static int rtmp_command_onconnect(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	struct rtmp_connect_t connect;
	struct amf_object_item_t items[1];
	struct amf_object_item_t commands[8];

	memset(&connect, 0, sizeof(connect));
	connect.encoding = (double)RTMP_ENCODING_AMF_0;
	AMF_OBJECT_ITEM_VALUE(commands[0], AMF_STRING, "app", connect.app, sizeof(connect.app));
	AMF_OBJECT_ITEM_VALUE(commands[1], AMF_STRING, "flashver", connect.flashver, sizeof(connect.flashver));
	AMF_OBJECT_ITEM_VALUE(commands[2], AMF_STRING, "tcUrl", connect.tcUrl, sizeof(connect.tcUrl));
	AMF_OBJECT_ITEM_VALUE(commands[3], AMF_BOOLEAN, "fpad", &connect.fpad, 1);
	AMF_OBJECT_ITEM_VALUE(commands[4], AMF_NUMBER, "audioCodecs", &connect.audioCodecs, 8);
	AMF_OBJECT_ITEM_VALUE(commands[5], AMF_NUMBER, "videoCodecs", &connect.videoCodecs, 8);
	AMF_OBJECT_ITEM_VALUE(commands[6], AMF_NUMBER, "videoFunction", &connect.videoFunction, 8);
	AMF_OBJECT_ITEM_VALUE(commands[7], AMF_NUMBER, "objectEncoding", &connect.encoding, 8);

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", commands, sizeof(commands) / sizeof(commands[0]));

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onconnect(rtmp->param, r, transaction, &connect);
}

// createStream request parser
static int rtmp_command_oncreate_stream(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	struct amf_object_item_t items[1];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.oncreate_stream(rtmp->param, r, transaction);
}

// 7.2.2.1. play (p38)
static int rtmp_command_onplay(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	uint8_t reset = 0;
	double start = -2; // the start time in seconds, [default] -2-live/vod, -1-live only, >=0-seek position
	double duration = -1; // duration of playback in seconds, [default] -1-live/record ends, 0-single frame, >0-play duration
	char stream_name[N_STREAM_NAME] = { 0 };

	struct amf_object_item_t items[5];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "stream", stream_name, sizeof(stream_name));
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_NUMBER, "start", &start, 8);
	AMF_OBJECT_ITEM_VALUE(items[3], AMF_NUMBER, "duration", &duration, 8);
	AMF_OBJECT_ITEM_VALUE(items[4], AMF_BOOLEAN, "reset", &reset, 1);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onplay(rtmp->param, r, transaction, stream_name, start, duration, reset);
}

// 7.2.2.3. deleteStream (p43)
static int rtmp_command_ondelete_stream(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	double stream_id = 0;
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "streamId", &stream_id, 8);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.ondelete_stream(rtmp->param, r, transaction, stream_id);
}

// 7.2.2.4. receiveAudio (p44)
static int rtmp_command_onreceive_audio(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	uint8_t receiveAudio = 1; // 1-receive audio, 0-no audio
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "receiveAudio", &receiveAudio, 1);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onreceive_audio(rtmp->param, r, transaction, receiveAudio);
}

// 7.2.2.5. receiveVideo (p45)
static int rtmp_command_onreceive_video(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	uint8_t receiveVideo = 1; // 1-receive video, 0-no video
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "receiveVideo", &receiveVideo, 1);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onreceive_video(rtmp->param, r, transaction, receiveVideo);
}

// 7.2.2.6. publish (p45)
static int rtmp_command_onpublish(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	char stream_name[N_STREAM_NAME] = { 0 };
	char stream_type[18] = { 0 }; // Publishing type: live/record/append

	struct amf_object_item_t items[3];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "name", stream_name, sizeof(stream_name));
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_STRING, "type", stream_type, sizeof(stream_type));

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onpublish(rtmp->param, r, transaction, stream_name, stream_type);
}

// 7.2.2.7. seek (p46)
static int rtmp_command_onseek(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	double milliSeconds = 0;
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "milliSeconds", &milliSeconds, 8);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onseek(rtmp->param, r, transaction, milliSeconds);
}

// pause request parser
static int rtmp_command_onpause(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	uint8_t pause = 0; // 1-pause, 0-resuming play
	double milliSeconds = 0;
	struct amf_object_item_t items[3];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "pause", &pause, 1);
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_NUMBER, "milliSeconds", &milliSeconds, 8);

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onpause(rtmp->param, r, transaction, pause, milliSeconds);
}

static int rtmp_command_onget_stream_length(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	char stream_name[N_STREAM_NAME] = { 0 };
	struct amf_object_item_t items[3];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "playpath", stream_name, sizeof(stream_name));

	r = amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
	return rtmp->u.server.onget_stream_length(rtmp->param, r, transaction, stream_name);
}

/*
// FCPublish request parser
static int rtmp_command_onfcpublish(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	char stream_name[N_STREAM_NAME] = { 0 };
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "playpath", stream_name, sizeof(stream_name));

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}

// FCUnpublish request parser
static int rtmp_command_onfcunpublish(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	char stream_name[N_STREAM_NAME] = { 0 };
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "playpath", stream_name, sizeof(stream_name));

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}

// FCSubscribe request parser
static int rtmp_command_onfcsubscribe(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	char subscribe[N_STREAM_NAME] = { 0 };
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "subscribepath", subscribe, sizeof(subscribe));

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}

// FCUnsubscribe request parser
static int rtmp_command_onfcunsubscribe(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	char subscribe[N_STREAM_NAME] = { 0 };
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "subscribepath", subscribe, sizeof(subscribe));

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}
*/

struct rtmp_command_handler_t
{
	const char* name;
	int (*handler)(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes);
};

const static struct rtmp_command_handler_t s_command_handler[] = {
	// client side
	{ "_result",		rtmp_command_onresult },
	{ "_error",			rtmp_command_onerror },
	{ "onStatus",		rtmp_command_onstatus },

//	{ "close",			rtmp_command_onclose },

//	{ "onBWDone",		rtmp_command_onsendcheckbw },

//	{ "_onbwcheck",		rtmp_command_onbwcheck },
//	{ "_onbwdone",		rtmp_command_onbwdone },

//	{ "ping",			rtmp_command_onping },

//	{ "playlist_ready",	rtmp_command_onplaylist },

//	{ "onFCSubscribe",	rtmp_command_onfcsubscribe },
//	{ "onFCUnsubscribe",rtmp_command_onfcunsubscribe },

	// server side
	{ "connect",		rtmp_command_onconnect },
	{ "createStream",	rtmp_command_oncreate_stream },

	{ "play",			rtmp_command_onplay },
	{ "deleteStream",	rtmp_command_ondelete_stream },
	{ "receiveAudio",	rtmp_command_onreceive_audio },
	{ "receiveVideo",	rtmp_command_onreceive_video },
	{ "publish",		rtmp_command_onpublish },
	{ "seek",			rtmp_command_onseek },
	{ "pause",			rtmp_command_onpause },
	{ "getStreamLength",rtmp_command_onget_stream_length },

//	{ "FCPublish",		rtmp_command_onfcpublish },
//	{ "FCUnpublish",	rtmp_command_onfcunpublish },
//	{ "FCSubscribe",	rtmp_command_onfcsubscribe },
//	{ "FCUnsubscribe",	rtmp_command_onfcunsubscribe },
//	{ "_checkbw",		rtmp_command_onsendcheckbw },
};

int rtmp_invoke_handler(struct rtmp_t* rtmp, const struct rtmp_chunk_header_t* header, const uint8_t* data)
{
	int i;
	char command[64] = { 0 };
	double transaction = -1;
	const uint8_t *end = data + header->length;

	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_STRING, "command", command, sizeof(command));
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "transactionId", &transaction, sizeof(double));

	data = amf_read_items(data, end, items, sizeof(items) / sizeof(items[0]));
	if (!data || -1.0 == transaction)
		return EINVAL; // invalid data

	for (i = 0; i < sizeof(s_command_handler) / sizeof(s_command_handler[0]); i++)
	{
		if (0 == strcmp(command, s_command_handler[i].name))
		{
			return s_command_handler[i].handler(rtmp, transaction, data, (int)(end - data));
		}
	}

	//printf("unknown command: %s\n", command);
	return 0; // not found
}
