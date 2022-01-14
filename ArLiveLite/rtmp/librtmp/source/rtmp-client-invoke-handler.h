#include "rtmp-internal.h"
#include "amf0.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
#define strcasecmp _stricmp
#endif

struct rtmp_result_t
{
	char code[64]; // NetStream.Play.Start
	char level[8]; // warning/status/error
	char description[256];
};

//static const char* s_rtmp_command_code[] = {
//	"NetConnection.Connect.Success",
//	"NetConnection.Connect.Closed",
//	"NetConnection.Connect.Failed",
//	"NetConnection.Connect.AppShutdown",
//	"NetConnection.Connect.InvalidApp",
//	"NetConnection.Connect.Rejected",
//
//	"NetStream.Failed",
//	"NetStream.Play.Failed",
//	"NetStream.Play.StreamNotFound",
//	"NetStream.Play.Start",
//	"NetStream.Play.Stop",
//	"NetStream.Play.Complete",
//	"NetStream.Play.PublishNotify",
//	"NetStream.Play.UnpublishNotify",
//	"NetStream.Seek.Notify",
//	"NetStream.Seek.Failed",
//	"NetStream.Pause.Notify",
//	"NetStream.Unpause.Notify",
//	"NetStream.Publish.Start",
//	"NetStream.Publish.BadName",
//	"NetStream.Unpublish.Success",
//	"NetStream.Record.Failed",
//	"NetStream.Record.NoAccess",
//	"NetStream.Record.Start",
//	"NetStream.Record.Stop",
//};

#define AMF_OBJECT_ITEM_VALUE(v, amf_type, amf_name, amf_value, amf_size) { v.type=amf_type; v.name=amf_name; v.value=amf_value; v.size=amf_size; }

// s -> c
static int rtmp_command_onconnect_reply(struct rtmp_result_t* result, const uint8_t* data, uint32_t bytes)
{
	char fmsver[64] = { 0 };
	double capabilities = 0;
	struct amf_object_item_t prop[2];
	struct amf_object_item_t info[3]; 
	struct amf_object_item_t items[2];

	AMF_OBJECT_ITEM_VALUE(prop[0], AMF_STRING, "fmsVer", fmsver, sizeof(fmsver));
	AMF_OBJECT_ITEM_VALUE(prop[1], AMF_NUMBER, "capabilities", &capabilities, sizeof(capabilities));

	AMF_OBJECT_ITEM_VALUE(info[0], AMF_STRING, "code", result->code, sizeof(result->code));
	AMF_OBJECT_ITEM_VALUE(info[1], AMF_STRING, "level", result->level, sizeof(result->level));
	AMF_OBJECT_ITEM_VALUE(info[2], AMF_STRING, "description", result->description, sizeof(result->description));

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "Properties", prop, sizeof(prop) / sizeof(prop[0]));
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_OBJECT, "Information", info, sizeof(info) / sizeof(info[0]));

	//rtmp->onstatus();
	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : EINVAL;
}

// s -> c
static int rtmp_command_oncreate_stream_reply(const uint8_t* data, uint32_t bytes, double *stream_id)
{
	struct amf_object_item_t items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "streamId", stream_id, 8);

	//rtmp->onstatus();
	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : EINVAL;
}

// s -> c
static int rtmp_command_onresult(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	int r;
	double stream_id = 0;
	double duration = 0;
	struct rtmp_result_t result;

	switch ((uint32_t)transaction)
	{
	case RTMP_TRANSACTION_CONNECT:
		// next:
		// 1. releaseStream/FCPublish or serverBW/user control message event buffer time
		// 2. createStream
		// 3. FCSubscribe
		r = rtmp_command_onconnect_reply(&result, data, bytes);
		return 0 == r ? rtmp->u.client.onconnect(rtmp->param) : r;

	case RTMP_TRANSACTION_CREATE_STREAM:
		// next: 
		// publish 
		// or play/user control message event buffer time
		r = rtmp_command_oncreate_stream_reply(data, bytes, &stream_id);
		return 0 == r ? rtmp->u.client.oncreate_stream(rtmp->param, stream_id) : r;

	case RTMP_TRANSACTION_GET_STREAM_LENGTH:
		return rtmp_command_oncreate_stream_reply(data, bytes, &duration);

	default:
		return 0;
	}
}

// s -> c
static int rtmp_command_onerror(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	struct rtmp_result_t result;
	struct amf_object_item_t info[3];
	struct amf_object_item_t items[2];

	AMF_OBJECT_ITEM_VALUE(info[0], AMF_STRING, "code", result.code, sizeof(result.code));
	AMF_OBJECT_ITEM_VALUE(info[1], AMF_STRING, "level", result.level, sizeof(result.level));
	AMF_OBJECT_ITEM_VALUE(info[2], AMF_STRING, "description", result.description, sizeof(result.description));

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_OBJECT, "Information", info, sizeof(info) / sizeof(info[0]));

	if (NULL == amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])))
	{
		return EINVAL; // format error
	}

	//rtmp->onerror(rtmp->param, -1, result.code);
	(void)transaction;
	(void)rtmp;
	return -1;
}

// s -> c
static int rtmp_command_onstatus(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	struct rtmp_result_t result;
	struct amf_object_item_t info[3];
	struct amf_object_item_t items[2];

	AMF_OBJECT_ITEM_VALUE(info[0], AMF_STRING, "code", result.code, sizeof(result.code));
	AMF_OBJECT_ITEM_VALUE(info[1], AMF_STRING, "level", result.level, sizeof(result.level));
	AMF_OBJECT_ITEM_VALUE(info[2], AMF_STRING, "description", result.description, sizeof(result.description));

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0); // Command object
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_OBJECT, "information", info, sizeof(info) / sizeof(info[0])); // Information object

	if (NULL == amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])))
	{
		return EINVAL;
	}

	assert(0 == strcmp(RTMP_LEVEL_ERROR, result.level)
		|| 0 == strcmp(RTMP_LEVEL_STATUS, result.level)
		|| 0 == strcmp(RTMP_LEVEL_WARNING, result.level)
		|| 0 == strcmp(RTMP_LEVEL_FINISH, result.level));

	if (0 == strcmp(RTMP_LEVEL_ERROR, result.level))
	{
		//rtmp->onerror(rtmp->param, -1, result.code);
		return -1;
	}
	else
	{
		if (0 == strcasecmp(result.code, "NetStream.Play.Start") 
			|| 0 == strcasecmp(result.code, "NetStream.Record.Start")
			|| 0 == strcasecmp(result.code, "NetStream.Publish.Start"))
		{
			rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_START);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Seek.Notify"))
		{
			rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_SEEK);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Pause.Notify"))
		{
			rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_PAUSE);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Unpause.Notify"))
		{
			rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_START);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Play.Reset"))
		{
			//rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_RESET);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Play.Stop") 
				|| 0 == strcasecmp(result.code, "NetStream.Record.Stop")
				|| 0 == strcasecmp(result.code, "NetStream.Play.Complete"))
		{
			rtmp->u.client.onnotify(rtmp->param, RTMP_NOTIFY_STOP);
		}
		else if (0 == strcasecmp(result.code, "NetStream.Play.PublishNotify") 
			|| 0 == strcasecmp(result.code, "NetStream.Play.UnpublishNotify"))
		{
		}
		else if (0 == strcasecmp(result.code, "NetConnection.Connect.InvalidApp")
			|| 0 == strcasecmp(result.code, "NetConnection.Connect.Rejected")
			|| 0 == strcasecmp(result.code, "NetStream.Connect.IllegalApplication") // ksyun cdn: level finish, auth failed
			|| 0 == strcasecmp(result.code, "NetStream.Publish.AlreadyExistStream") // ksyun cdn: level finish, description Already exist stream!
			|| 0 == strcasecmp(result.code, "NetStream.Failed")
			|| 0 == strcasecmp(result.code, "NetStream.Play.Failed")
			|| 0 == strcasecmp(result.code, "NetStream.Play.StreamNotFound"))
		{
			//rtmp->onerror(rtmp->param, -1, result.code);
			return -1;
		}
		else
		{
			assert(0);
			printf("%s: level: %s, code: %s, description: %s\n", __FUNCTION__, result.level, result.code, result.description);
			return -1;
		}
	}

	(void)transaction;
	return 0;
}

/*
static int rtmp_command_onbwdone(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	struct amf_object_item_t items[1];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}

static int rtmp_command_onbwcheck(struct rtmp_t* rtmp, double transaction, const uint8_t* data, uint32_t bytes)
{
	struct amf_object_item_t items[1];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);

	return amf_read_items(data, data + bytes, items, sizeof(items) / sizeof(items[0])) ? 0 : -1;
}
*/
