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
 * Copyright (C) Cisco Systems Inc. 2001-2002.  All Rights Reserved.
 * 
 * Portions created by Ximpo Group Ltd. are
 * Copyright (C) Ximpo Group Ltd. 2003, 2004.  All Rights Reserved.
 *
 * Contributor(s):
 *		Dave Mackie               dmackie@cisco.com
 *              Bill May                  wmay@cisco.com
 *		Alix Marchandise-Franquet alix@cisco.com
 *		Ximpo Group Ltd.          mp4v2@ximpo.com
 */

#include "mp4common.h"

static char* PrintAudioInfo(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{
	static const char* mpeg4AudioNames[] = {
		"MPEG-4 AAC main",
		"MPEG-4 AAC LC",
		"MPEG-4 AAC SSR",
		"MPEG-4 AAC LTP",
		"MPEG-4 AAC HE",
		"MPEG-4 AAC Scalable",
		"MPEG-4 TwinVQ",
		"MPEG-4 CELP",
		"MPEG-4 HVXC",
		NULL, NULL,
		"MPEG-4 TTSI",
		"MPEG-4 Main Synthetic",
		"MPEG-4 Wavetable Syn",
		"MPEG-4 General MIDI",
		"MPEG-4 Algo Syn and Audio FX",
		"MPEG-4 ER AAC LC",
		NULL,
		"MPEG-4 ER AAC LTP",
		"MPEG-4 ER AAC Scalable",
		"MPEG-4 ER TwinVQ",
		"MPEG-4 ER BSAC",
		"MPEG-4 ER ACC LD",
		"MPEG-4 ER CELP",
		"MPEG-4 ER HVXC",
		"MPEG-4 ER HILN",
		"MPEG-4 ER Parametric",
		"MPEG-4 SSC",
		"MPEG-4 PS",
		"MPEG-4 MPEG Surround",
		NULL,
		"MPEG-4 Layer-1",
		"MPEG-4 Layer-2",
		"MPEG-4 Layer-3",
		"MPEG-4 DST",
		"MPEG-4 Audio Lossless",
		"MPEG-4 SLS",
		"MPEG-4 SLS non-core", 
	};

	static const u_int8_t mpegAudioTypes[] = {
		MP4_MPEG2_AAC_MAIN_AUDIO_TYPE,	// 0x66
		MP4_MPEG2_AAC_LC_AUDIO_TYPE,	// 0x67
		MP4_MPEG2_AAC_SSR_AUDIO_TYPE,	// 0x68
		MP4_MPEG2_AUDIO_TYPE,			// 0x69
		MP4_MPEG1_AUDIO_TYPE,			// 0x6B
		// private types
		MP4_PCM16_LITTLE_ENDIAN_AUDIO_TYPE,
		MP4_VORBIS_AUDIO_TYPE,
		MP4_ALAW_AUDIO_TYPE,
		MP4_ULAW_AUDIO_TYPE,
		MP4_G723_AUDIO_TYPE,
		MP4_PCM16_BIG_ENDIAN_AUDIO_TYPE,
	};
	static const char* mpegAudioNames[] = {
		"MPEG-2 AAC Main",
		"MPEG-2 AAC LC",
		"MPEG-2 AAC SSR",
		"MPEG-2 Audio (13818-3)",
		"MPEG-1 Audio (11172-3)",
		// private types
		"PCM16 (little endian)",
		"Vorbis",
		"G.711 aLaw",
		"G.711 uLaw",
		"G.723.1",
		"PCM16 (big endian)",
	};
	u_int8_t numMpegAudioTypes =
		sizeof(mpegAudioTypes) / sizeof(u_int8_t);

	const char* typeName = "Unknown";
	bool foundType = false;
	u_int8_t type = 0;
	const char *media_data_name;

	media_data_name = MP4GetTrackMediaDataName(mp4File, trackId);

	if (media_data_name == NULL) {
	  typeName = "Unknown - no media data name";
	} else if (strcasecmp(media_data_name, "samr") == 0) {
	    typeName = "AMR";
	    foundType = true;
	} else if (strcasecmp(media_data_name, "sawb") == 0) {
	    typeName = "AMR-WB";
	    foundType = true;
	} else if (strcasecmp(media_data_name, "mp4a") == 0) {
	    
	  type = MP4GetTrackEsdsObjectTypeId(mp4File, trackId);
	  switch (type) {
	  case MP4_INVALID_AUDIO_TYPE:
	    typeName = "AAC from .mov";
	    foundType = true;
	    break;
	  case MP4_MPEG4_AUDIO_TYPE:  {
	
	    type = MP4GetTrackAudioMpeg4Type(mp4File, trackId);
	    if (type == MP4_MPEG4_INVALID_AUDIO_TYPE ||
		type > NUM_ELEMENTS_IN_ARRAY(mpeg4AudioNames) || 
		mpeg4AudioNames[type - 1] == NULL) {
	      typeName = "MPEG-4 Unknown Profile";
	    } else {
	      typeName = mpeg4AudioNames[type - 1];
	      foundType = true;
	    }
	    break;
	  }
	    // fall through
	  default:
	    for (u_int8_t i = 0; i < numMpegAudioTypes; i++) {
	      if (type == mpegAudioTypes[i]) {
		typeName = mpegAudioNames[i];
		foundType = true;
		break;
	      }
	    }
	  }
	} else {
	  typeName = media_data_name;
	  foundType = true;
	}

	u_int32_t timeScale =
		MP4GetTrackTimeScale(mp4File, trackId);

	MP4Duration trackDuration =
		MP4GetTrackDuration(mp4File, trackId);

	double msDuration =
		UINT64_TO_DOUBLE(MP4ConvertFromTrackDuration(mp4File, trackId,
			trackDuration, MP4_MSECS_TIME_SCALE));

	u_int32_t avgBitRate =
		MP4GetTrackBitRate(mp4File, trackId);

	char *sInfo = (char*)MP4Malloc(256);

	// type duration avgBitrate samplingFrequency
	if (foundType)
	  snprintf(sInfo, 256, 
		  "%u\taudio\t%s%s, %.3f secs, %u kbps, %u Hz\n",
		  trackId,
		  MP4IsIsmaCrypMediaTrack(mp4File, trackId) ? "enca - " : "",
		  typeName,
		  msDuration / 1000.0,
		  (avgBitRate + 500) / 1000,
		  timeScale);
	else
	  snprintf(sInfo, 256,
		  "%u\taudio\t%s%s(%u), %.3f secs, %u kbps, %u Hz\n",
		  trackId,
		  MP4IsIsmaCrypMediaTrack(mp4File, trackId) ? "enca - " : "",
		  typeName,
		  type,
		  msDuration / 1000.0,
		  (avgBitRate + 500) / 1000,
		  timeScale);

	return sInfo;
}
static const struct {
  uint8_t profile;
  const char *name;
} VisualProfileToName[] = {
  { MPEG4_SP_L1, "MPEG-4 Simple @ L1"},
  { MPEG4_SP_L2, "MPEG-4 Simple @ L2" },
  { MPEG4_SP_L3, "MPEG-4 Simple @ L3" },
  { MPEG4_SP_L0, "MPEG-4 Simple @ L0" },
  { MPEG4_SSP_L1, "MPEG-4 Simple Scalable @ L1"},
  { MPEG4_SSP_L2, "MPEG-4 Simple Scalable @ L2" },
  { MPEG4_CP_L1, "MPEG-4 Core @ L1"},
  { MPEG4_CP_L2, "MPEG-4 Core @ L2"},
  { MPEG4_MP_L2, "MPEG-4 Main @ L2"},
  { MPEG4_MP_L3, "MPEG-4 Main @ L3"},
  { MPEG4_MP_L4, "MPEG-4 Main @ L4"},
  { MPEG4_NBP_L2, "MPEG-4 N-bit @ L2"},
  { MPEG4_STP_L1, "MPEG-4  Scalable Texture @ L1"},
  { MPEG4_SFAP_L1, "MPEG-4 Simple Face Anim @ L1"},
  { MPEG4_SFAP_L2, "MPEG-4  Simple Face Anim @ L2"},
  { MPEG4_SFBAP_L1, "MPEG-4  Simple FBA @ L1"},
  { MPEG4_SFBAP_L2, "MPEG-4 Simple FBA @ L2"},
  { MPEG4_BATP_L1, "MPEG-4 Basic Anim Text @ L1"},
  { MPEG4_BATP_L2, "MPEG-4 Basic Anim Text @ L2"},
  { MPEG4_HP_L1, "MPEG-4 Hybrid @ L1"},
  { MPEG4_HP_L2, "MPEG-4 Hybrid @ L2"},
  { MPEG4_ARTSP_L1, "MPEG-4 Adv RT Simple @ L1"},
  { MPEG4_ARTSP_L2, "MPEG-4 Adv RT Simple @ L2"},
  { MPEG4_ARTSP_L3, "MPEG-4 Adv RT Simple @ L3"},
  { MPEG4_ARTSP_L4, "MPEG-4 Adv RT Simple @ L4"},
  { MPEG4_CSP_L1, "MPEG-4 Core Scalable @ L1"},
  { MPEG4_CSP_L2, "MPEG-4 Core Scalable @ L2"},
  { MPEG4_CSP_L3, "MPEG-4 Core Scalable @ L3"},
  { MPEG4_ACEP_L1, "MPEG-4 Adv Coding Efficieny @ L1"},
  { MPEG4_ACEP_L2, "MPEG-4 Adv Coding Efficieny @ L2"},
  { MPEG4_ACEP_L3, "MPEG-4 Adv Coding Efficieny @ L3"},
  { MPEG4_ACEP_L4, "MPEG-4 Adv Coding Efficieny @ L4"},
  { MPEG4_ACP_L1, "MPEG-4 Adv Core Profile @ L1"},
  { MPEG4_ACP_L2, "MPEG-4 Adv Core Profile @ L2"},
  { MPEG4_AST_L1, "MPEG-4 Adv Scalable Texture @ L1"},
  { MPEG4_AST_L2, "MPEG-4 Adv Scalable Texture @ L2"},
  { MPEG4_AST_L3, "MPEG-4 Adv Scalable Texture @ L3"},
  { MPEG4_S_STUDIO_P_L1, "MPEG-4 Simple Studio @ L1"},
  { MPEG4_S_STUDIO_P_L2, "MPEG-4 Simple Studio @ L2"},
  { MPEG4_S_STUDIO_P_L3, "MPEG-4 Simple Studio @ L3"},
  { MPEG4_S_STUDIO_P_L4, "MPEG-4 Simple Studio @ L4"},
  { MPEG4_C_STUDIO_P_L1, "MPEG-4 Core Studio @ L1"},
  { MPEG4_C_STUDIO_P_L2, "MPEG-4 Core Studio @ L2"},
  { MPEG4_C_STUDIO_P_L3, "MPEG-4 Core Studio @ L3"},
  { MPEG4_C_STUDIO_P_L4, "MPEG-4 Core Studio @ L4"},
  { MPEG4_ASP_L0, "MPEG-4 Adv Simple@L0"},
  { MPEG4_ASP_L1, "MPEG-4 Adv Simple@L1"},
  { MPEG4_ASP_L2, "MPEG-4 Adv Simple@L2"},
  { MPEG4_ASP_L3, "MPEG-4 Adv Simple@L3"},
  { MPEG4_ASP_L4, "MPEG-4 Adv Simple@L4"},
  { MPEG4_ASP_L5, "MPEG-4 Adv Simple@L5"},
  { MPEG4_ASP_L3B, "MPEG-4 Adv Simple@L3b"},
  { MPEG4_FGSP_L0, "MPEG-4 FGS @ L0" },
  { MPEG4_FGSP_L1, "MPEG-4 FGS @ L1" },
  { MPEG4_FGSP_L2, "MPEG-4 FGS @ L2" },
  { MPEG4_FGSP_L3, "MPEG-4 FGS @ L3" },
  { MPEG4_FGSP_L4, "MPEG-4 FGS @ L4" },
  { MPEG4_FGSP_L5, "MPEG-4 FGS @ L5" }
};

static const char *Mpeg4VisualProfileName (uint8_t visual_profile)
{
  size_t size = sizeof(VisualProfileToName) / sizeof(*VisualProfileToName);

  for (size_t ix = 0; ix < size; ix++) {
    if (visual_profile == VisualProfileToName[ix].profile) {
      return (VisualProfileToName[ix].name);
    }
  }
  return (NULL);
}
static char* PrintVideoInfo(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{

	static const u_int8_t mpegVideoTypes[] = {
		MP4_MPEG2_SIMPLE_VIDEO_TYPE,	// 0x60
		MP4_MPEG2_MAIN_VIDEO_TYPE,		// 0x61
		MP4_MPEG2_SNR_VIDEO_TYPE,		// 0x62
		MP4_MPEG2_SPATIAL_VIDEO_TYPE,	// 0x63
		MP4_MPEG2_HIGH_VIDEO_TYPE,		// 0x64
		MP4_MPEG2_442_VIDEO_TYPE,		// 0x65
		MP4_MPEG1_VIDEO_TYPE,			// 0x6A
		MP4_JPEG_VIDEO_TYPE,			// 0x6C
		MP4_YUV12_VIDEO_TYPE,
		MP4_H263_VIDEO_TYPE,
		MP4_H261_VIDEO_TYPE,
	};
	static const char* mpegVideoNames[] = {
		"MPEG-2 Simple",
		"MPEG-2 Main",
		"MPEG-2 SNR",
		"MPEG-2 Spatial",
		"MPEG-2 High",
		"MPEG-2 4:2:2",
		"MPEG-1",
		"JPEG",
		"YUV12",
		"H.263",
		"H.261",
	};
	u_int8_t numMpegVideoTypes =
		sizeof(mpegVideoTypes) / sizeof(u_int8_t);
	bool foundTypeName = false;
	const char* typeName = "Unknown";

	const char *media_data_name;
	char originalFormat[8];
	char  oformatbuffer[32];
	originalFormat[0] = 0;
	*oformatbuffer = 0;
	uint8_t type = 0;
	
	media_data_name = MP4GetTrackMediaDataName(mp4File, trackId);
	// encv 264b
	if (strcasecmp(media_data_name, "encv") == 0) {
	  if (MP4GetTrackMediaDataOriginalFormat(mp4File, 
						 trackId, 
						 originalFormat, 
						 sizeof(originalFormat)) == false)
	      media_data_name = NULL;
	      
	}
  
	char  typebuffer[80];
	if (media_data_name == NULL) {
	  typeName = "Unknown - no media data name";
	  foundTypeName = true;
	} else if ((strcasecmp(media_data_name, "avc1") == 0) ||
	  	(strcasecmp(originalFormat, "264b") == 0)) {
	  // avc
	  uint8_t profile, level;
	  char profileb[20], levelb[20];
	  if (MP4GetTrackH264ProfileLevel(mp4File, trackId, 
					  &profile, &level)) {
	    if (profile == 66) {
	      strcpy(profileb, "Baseline");
	    } else if (profile == 77) {
	      strcpy(profileb, "Main");
	    } else if (profile == 88) {
	      strcpy(profileb, "Extended");
	    } else if (profile == 100) {
	      strcpy(profileb, "High");
	    } else if (profile == 110) {
	      strcpy(profileb, "High 10");
	    } else if (profile == 122) {
	      strcpy(profileb, "High 4:2:2");
	    } else if (profile == 144) {
	      strcpy(profileb, "High 4:4:4");
	    } else {
	      snprintf(profileb, 20, "Unknown Profile %x", profile);
	    } 
	    switch (level) {
	    case 10: case 20: case 30: case 40: case 50:
	      snprintf(levelb, 20, "%u", level / 10);
	      break;
	    case 11: case 12: case 13:
	    case 21: case 22:
	    case 31: case 32:
	    case 41: case 42:
	    case 51:
	      snprintf(levelb, 20, "%u.%u", level / 10, level % 10);
	      break;
	    default:
	      snprintf(levelb, 20, "unknown level %x", level);
	      break;
	    }
	    if (originalFormat != NULL && originalFormat[0] != '\0') 
	      snprintf(oformatbuffer, 32, "(%s) ", originalFormat);
	    snprintf(typebuffer, sizeof(typebuffer), "H264 %s%s@%s", 
		    oformatbuffer, profileb, levelb);
	    typeName = typebuffer;
	  } else {
	    typeName = "H.264 - profile/level error";
	  }
	  foundTypeName = true;
	} else if (strcasecmp(media_data_name, "s263") == 0) {
	  // 3gp h.263
	  typeName = "H.263";
	  foundTypeName = true;
	} else if ((strcasecmp(media_data_name, "mp4v") == 0) ||
		   (strcasecmp(media_data_name, "encv") == 0)) {
	  // note encv might needs it's own field eventually.
	  type = MP4GetTrackEsdsObjectTypeId(mp4File, trackId);
	  if (type == MP4_MPEG4_VIDEO_TYPE) {
	    type = MP4GetVideoProfileLevel(mp4File, trackId);
	    typeName = Mpeg4VisualProfileName(type);
	    if (typeName == NULL) {
	      typeName = "MPEG-4 Unknown Profile";
	    } else {
	      foundTypeName = true;
	    }
	  } else {
	    for (u_int8_t i = 0; i < numMpegVideoTypes; i++) {
	      if (type == mpegVideoTypes[i]) {
		typeName = mpegVideoNames[i];
		foundTypeName = true;
		break;
	      }
	    }
	  }
	} else {
	  typeName = media_data_name;
	  foundTypeName = true; // we don't have a type value to display
	}

	MP4Duration trackDuration =
		MP4GetTrackDuration(mp4File, trackId);

	double msDuration =
		UINT64_TO_DOUBLE(MP4ConvertFromTrackDuration(mp4File, trackId,
			trackDuration, MP4_MSECS_TIME_SCALE));

	u_int32_t avgBitRate =
		MP4GetTrackBitRate(mp4File, trackId);

	// Note not all mp4 implementations set width and height correctly
	// The real answer can be buried inside the ES configuration info
	u_int16_t width = MP4GetTrackVideoWidth(mp4File, trackId);

	u_int16_t height = MP4GetTrackVideoHeight(mp4File, trackId);

	double fps = MP4GetTrackVideoFrameRate(mp4File, trackId);

	char *sInfo = (char*)MP4Malloc(256);

	// type duration avgBitrate frameSize frameRate
	if (foundTypeName) {
	  sprintf(sInfo,
		  "%u\tvideo\t%s%s, %.3f secs, %u kbps, %ux%u @ %f fps\n",
		  trackId,
		  MP4IsIsmaCrypMediaTrack(mp4File, trackId) ? "encv - " : "",
		  typeName,
		  msDuration / 1000.0,
		  (avgBitRate + 500) / 1000,
		  width,
		  height,
		  fps
		  );
	} else {
	  sprintf(sInfo,
		  "%u\tvideo\t%s(%u), %.3f secs, %u kbps, %ux%u @ %f fps\n",
		  trackId,
		  typeName,
		  type, 
		  msDuration / 1000.0,
		  (avgBitRate + 500) / 1000,
		  width,
		  height,
		  fps
		  );
	}

	return sInfo;
}
static char* PrintCntlInfo(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{
  const char *media_data_name = MP4GetTrackMediaDataName(mp4File, trackId);
  const char *typeName = "Unknown";

  if (media_data_name == NULL) {
    typeName = "Unknown - no media data name";
  } else if (strcasecmp(media_data_name, "href") == 0) {
    typeName = "ISMA Href";
  } else {
    typeName = media_data_name;
  }

  MP4Duration trackDuration = 
    MP4GetTrackDuration(mp4File, trackId);
 
  double msDuration = 
    UINT64_TO_DOUBLE(MP4ConvertFromTrackDuration(mp4File, trackId, 
						 trackDuration, MP4_MSECS_TIME_SCALE));
  char *sInfo = (char *)MP4Malloc(256);

  snprintf(sInfo, 256,
	   "%u\tcontrol\t%s, %.3f secs\n",
	   trackId, 
	   typeName,
	   msDuration / 1000.0);
  return sInfo;
}

	  
static char* PrintHintInfo(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{
	MP4TrackId referenceTrackId =
		MP4GetHintTrackReferenceTrackId(mp4File, trackId);

	char* payloadName = NULL;
	if (!MP4GetHintTrackRtpPayload(mp4File, trackId, &payloadName))
	  return NULL;

	char *sInfo = (char*)MP4Malloc(256);

	snprintf(sInfo, 256, 
		"%u\thint\tPayload %s for track %u\n",
		trackId,
		payloadName,
		referenceTrackId);

	free(payloadName);

	return sInfo;
}

static char* PrintTrackInfo(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{
	char* trackInfo = NULL;

	const char* trackType =
		MP4GetTrackType(mp4File, trackId);
	if (trackType == NULL) return NULL;

	if (!strcmp(trackType, MP4_AUDIO_TRACK_TYPE)) {
		trackInfo = PrintAudioInfo(mp4File, trackId);
	} else if (!strcmp(trackType, MP4_VIDEO_TRACK_TYPE)) {
		trackInfo = PrintVideoInfo(mp4File, trackId);
	} else if (!strcmp(trackType, MP4_HINT_TRACK_TYPE)) {
		trackInfo = PrintHintInfo(mp4File, trackId);
	} else if (strcmp(trackType, MP4_CNTL_TRACK_TYPE) == 0) {
	  trackInfo = PrintCntlInfo(mp4File, trackId);
	} else {
		trackInfo = (char*)MP4Malloc(256);
		if (!strcmp(trackType, MP4_OD_TRACK_TYPE)) {
		  snprintf(trackInfo, 256,
				"%u\tod\tObject Descriptors\n",
				trackId);
		} else if (!strcmp(trackType, MP4_SCENE_TRACK_TYPE)) {
		  snprintf(trackInfo, 256,
				"%u\tscene\tBIFS\n",
				trackId);
		} else {
		  snprintf(trackInfo, 256,
					"%u\t%s\n",
					trackId, trackType);
		}
	}

	return trackInfo;
}

extern "C" char* MP4Info(
	MP4FileHandle mp4File,
	MP4TrackId trackId)
{
	char* info = NULL;

	if (MP4_IS_VALID_FILE_HANDLE(mp4File)) {
		try {
			if (trackId == MP4_INVALID_TRACK_ID) {
			  uint buflen = 4 * 1024;
				info = (char*)MP4Calloc(buflen);

				buflen -= snprintf(info, buflen,
						"Track\tType\tInfo\n");

				u_int32_t numTracks = MP4GetNumberOfTracks(mp4File);

				for (u_int32_t i = 0; i < numTracks; i++) {
					trackId = MP4FindTrackId(mp4File, i);
					char* trackInfo = PrintTrackInfo(mp4File, trackId);
					strncat(info, trackInfo, buflen);
					uint newlen = strlen(trackInfo);
					if (newlen > buflen) buflen = 0;
					else buflen -= newlen;
					MP4Free(trackInfo);
				}
			} else {
				info = PrintTrackInfo(mp4File, trackId);
			}
		}
		catch (MP4Error* e) {
			delete e;
		}
	}

	return info;
}

extern "C" char* MP4FileInfo(
	const char* fileName,
	MP4TrackId trackId)
{
	MP4FileHandle mp4File =
		MP4Read(fileName);

	if (!mp4File) {
		return NULL;
	}

	char* info = MP4Info(mp4File, trackId);

	MP4Close(mp4File);

	return info;	// caller should free this
}

