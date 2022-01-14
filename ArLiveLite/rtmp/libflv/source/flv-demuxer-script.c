#include "flv-demuxer.h"
#include "amf0.h"
#include <errno.h>
#include <assert.h>
#include <string.h>

#define N_ONMETADATA 12 // 2-LEN + 10-onMetaData

/// http://www.cnblogs.com/musicfans/archive/2012/11/07/2819291.html
/// metadata keyframes/filepositions
/// @return >0-OK, 0-don't metadata, <0-error 
int flv_demuxer_script(struct flv_demuxer_t* flv, const uint8_t* data, size_t bytes)
{
	const uint8_t* end;
	char buffer[64] = { 0 };
	double audiocodecid = 0;
	double audiodatarate = 0; // bitrate / 1024
	double audiodelay = 0;
	double audiosamplerate = 0;
	double audiosamplesize = 0;
	double videocodecid = 0;
	double videodatarate = 0; // bitrate / 1024
	double framerate = 0;
	double height = 0;
	double width = 0;
	double duration = 0;
	double filesize = 0;
	int canSeekToEnd = 0;
	int stereo = 0;
	struct amf_object_item_t keyframes[2];
	struct amf_object_item_t prop[16];
	struct amf_object_item_t items[1];

#define AMF_OBJECT_ITEM_VALUE(v, amf_type, amf_name, amf_value, amf_size) { v.type=amf_type; v.name=amf_name; v.value=amf_value; v.size=amf_size; }
	AMF_OBJECT_ITEM_VALUE(keyframes[0], AMF_STRICT_ARRAY, "filepositions", NULL, 0); // ignore keyframes
	AMF_OBJECT_ITEM_VALUE(keyframes[1], AMF_STRICT_ARRAY, "times", NULL, 0);

	AMF_OBJECT_ITEM_VALUE(prop[0], AMF_NUMBER, "audiocodecid", &audiocodecid, sizeof(audiocodecid));
	AMF_OBJECT_ITEM_VALUE(prop[1], AMF_NUMBER, "audiodatarate", &audiodatarate, sizeof(audiodatarate));
	AMF_OBJECT_ITEM_VALUE(prop[2], AMF_NUMBER, "audiodelay", &audiodelay, sizeof(audiodelay));
	AMF_OBJECT_ITEM_VALUE(prop[3], AMF_NUMBER, "audiosamplerate", &audiosamplerate, sizeof(audiosamplerate));
	AMF_OBJECT_ITEM_VALUE(prop[4], AMF_NUMBER, "audiosamplesize", &audiosamplesize, sizeof(audiosamplesize));
	AMF_OBJECT_ITEM_VALUE(prop[5], AMF_BOOLEAN, "stereo", &stereo, sizeof(stereo));

	AMF_OBJECT_ITEM_VALUE(prop[6], AMF_BOOLEAN, "canSeekToEnd", &canSeekToEnd, sizeof(canSeekToEnd));
	AMF_OBJECT_ITEM_VALUE(prop[7], AMF_STRING, "creationdate", buffer, sizeof(buffer));
	AMF_OBJECT_ITEM_VALUE(prop[8], AMF_NUMBER, "duration", &duration, sizeof(duration));
	AMF_OBJECT_ITEM_VALUE(prop[9], AMF_NUMBER, "filesize", &filesize, sizeof(filesize));

	AMF_OBJECT_ITEM_VALUE(prop[10], AMF_NUMBER, "videocodecid", &videocodecid, sizeof(videocodecid));
	AMF_OBJECT_ITEM_VALUE(prop[11], AMF_NUMBER, "videodatarate", &videodatarate, sizeof(videodatarate));
	AMF_OBJECT_ITEM_VALUE(prop[12], AMF_NUMBER, "framerate", &framerate, sizeof(framerate));
	AMF_OBJECT_ITEM_VALUE(prop[13], AMF_NUMBER, "height", &height, sizeof(height));
	AMF_OBJECT_ITEM_VALUE(prop[14], AMF_NUMBER, "width", &width, sizeof(width));

	AMF_OBJECT_ITEM_VALUE(prop[15], AMF_OBJECT, "keyframes", keyframes, 2); // FLV I-index

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "onMetaData", prop, sizeof(prop) / sizeof(prop[0]));
#undef AMF_OBJECT_ITEM_VALUE

	end = data + bytes;
	if (AMF_STRING != data[0] || NULL == (data = AMFReadString(data + 1, end, 0, buffer, sizeof(buffer) - 1)))
	{
		assert(0);
		return -1;
	}
	
	// filter @setDataFrame
	if (0 == strcmp(buffer, "@setDataFrame"))
	{
		if (AMF_STRING != data[0] || NULL == (data = AMFReadString(data + 1, end, 0, buffer, sizeof(buffer) - 1)))
		{
			assert(0);
			return -1;
		}
	}

	// onTextData/onCaption/onCaptionInfo/onCuePoint/|RtmpSampleAccess
	if (0 != strcmp(buffer, "onMetaData"))
		return 0; // skip

	(void)flv;
	return amf_read_items(data, end, items, sizeof(items) / sizeof(items[0])) ? N_ONMETADATA : -1;
}
