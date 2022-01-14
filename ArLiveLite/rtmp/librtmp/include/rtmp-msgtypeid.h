#ifndef _rtmp_msgtypeid_h_
#define _rtmp_msgtypeid_h_

enum RTMPMessageTypeId
{
	/* Protocol Control Messages */
	RTMP_TYPE_SET_CHUNK_SIZE = 1,
	RTMP_TYPE_ABORT = 2,
	RTMP_TYPE_ACKNOWLEDGEMENT = 3, // bytes read report
	RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE = 5, // server bandwidth
	RTMP_TYPE_SET_PEER_BANDWIDTH = 6, // client bandwidth

	/* User Control Messages Event (4) */
	RTMP_TYPE_EVENT = 4,

	RTMP_TYPE_AUDIO = 8,
	RTMP_TYPE_VIDEO = 9,
	
	/* Data Message */
	RTMP_TYPE_FLEX_STREAM = 15, // AMF3
	RTMP_TYPE_DATA = 18, // AMF0

	/* Shared Object Message */
	RTMP_TYPE_FLEX_OBJECT = 16, // AMF3
	RTMP_TYPE_SHARED_OBJECT = 19, // AMF0

	/* Command Message */
	RTMP_TYPE_FLEX_MESSAGE = 17, // AMF3
	RTMP_TYPE_INVOKE = 20, // AMF0

	/* Aggregate Message */
	RTMP_TYPE_METADATA = 22,
};

#endif /* !_rtmp_msgtypeid_h_ */
