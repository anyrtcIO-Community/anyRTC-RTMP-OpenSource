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
 * Copyright (C) Cisco Systems Inc. 2004.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Bill May (from others work).
 */

#include "mp4common.h"
#include "atoms.h"

static const char name[5]={0251,'n', 'a', 'm', '\0'};
static const char art[5]={0251,'A', 'R', 'T', '\0'};
static const char wrt[5]={0251,'w', 'r', 't', '\0'};
static const char alb[5]={0251,'a', 'l', 'b', '\0'};
static const char day[5]={0251,'d', 'a', 'y', '\0'};
static const char too[5]={0251,'t', 'o', 'o', '\0'};
static const char cmt[5]={0251,'c', 'm', 't', '\0'};
static const char gen[5]={0251,'g', 'e', 'n', '\0'};
static const char grp[5]={0251,'g', 'r', 'p', '\0'};

MP4StandardAtom::MP4StandardAtom (const char *type) : MP4Atom(type)
{
  /*
   * This is a big if else loop.  Make sure that you don't break it 
   * when adding new atoms, or you will set the unknown type flag
   *
   * Try to keep it in alphabetical order - it should only be called
   * 1 time per atom, so it's not that urgent.
   */
  if (ATOMID(type) == ATOMID("aART") ||
      ATOMID(type) == ATOMID("akID") ||
      ATOMID(type) == ATOMID("apID") ||
      ATOMID(type) == ATOMID("atID")) {
      ExpectChildAtom("data", Required, OnlyOne);
  /*
   * b???
   */
  } else if (ATOMID(type) == ATOMID("bitr")) {
    AddProperty( /* 0 */
		new MP4Integer32Property("avgBitrate"));
    
    AddProperty( /* 1 */
		new MP4Integer32Property("maxBitrate"));

  } else if (ATOMID(type) == ATOMID("btrt")) {
    AddProperty( new MP4Integer32Property("bufferSizeDB")); /* 0 */
    AddProperty( new MP4Integer32Property("avgBitrate"));   /* 1 */
    AddProperty( new MP4Integer32Property("maxBitrate"));   /* 2 */
  } else if (ATOMID(type) == ATOMID("burl")) {
    AddProperty( new MP4StringProperty("base_url"));
  /*
   * c???
   */
  } else if (ATOMID(type) == ATOMID("co64")) {
    AddVersionAndFlags();
    
    MP4Integer32Property* pCount = 
      new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);
    
    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);
    
    pTable->AddProperty(
			new MP4Integer64Property("chunkOffset"));

  } else if (ATOMID(type) == ATOMID("cpil")) {
    ExpectChildAtom("data", Required, OnlyOne);
  } else if (ATOMID(type) == ATOMID("covr")) { /* Apple iTunes */
    ExpectChildAtom("data", Required, Many);
  } else if (ATOMID(type) == ATOMID("cprt") ||
	     ATOMID(type) == ATOMID("cnID")) {
#if 0
    AddVersionAndFlags();
    AddProperty(
		new MP4Integer16Property("language"));
    AddProperty(
		new MP4StringProperty("notice"));
#else
    ExpectChildAtom("data", Required, OnlyOne);
#endif

  } else if (ATOMID(type) == ATOMID("ctts")) {
    AddVersionAndFlags();
    
    MP4Integer32Property* pCount = 
      new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);
    
    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);
    
    pTable->AddProperty(new MP4Integer32Property("sampleCount"));
    pTable->AddProperty(new MP4Integer32Property("sampleOffset"));
  /*
   * d???
   */
  } else if (ATOMID(type) == ATOMID("dinf")) {
    ExpectChildAtom("dref", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("dimm")) {
    AddProperty( // bytes of immediate data
		new MP4Integer64Property("bytes"));

  } else if (ATOMID(type) == ATOMID("disk")) { /* Apple iTunes */
    ExpectChildAtom("data", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("dmax")) {
    AddProperty( // max packet duration 
		new MP4Integer32Property("milliSecs"));

  } else if (ATOMID(type) == ATOMID("dmed")) {
    AddProperty( // bytes sent from media data
		new MP4Integer64Property("bytes"));

  } else if (ATOMID(type) == ATOMID("drep")) {
    AddProperty( // bytes of repeated data
		new MP4Integer64Property("bytes"));
  /*
   * e???
   */
  } else if (ATOMID(type) == ATOMID("edts")) {
    ExpectChildAtom("elst", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("esds")) {
    AddVersionAndFlags();
    AddProperty(
		new MP4DescriptorProperty(NULL, MP4ESDescrTag, 0, 
					  Required, OnlyOne));
  /*
   * f???
   */
  } else if (ATOMID(type) == ATOMID("frma")) {
    AddProperty( /* 0 */
                new MP4Integer32Property("data-format"));			
  /*
   * g???
   */
  } else if (ATOMID(type) == ATOMID("gmhd")) { 
    ExpectChildAtom("gmin", Required, OnlyOne);
    ExpectChildAtom("tmcd", Optional, OnlyOne);
    ExpectChildAtom("text", Optional, OnlyOne);
  } else if (ATOMID(type) == ATOMID("gnre") ||
	     ATOMID(type) == ATOMID("geID")) { // Apple iTunes 
    ExpectChildAtom("data", Optional, OnlyOne);

  /*
   * h???
   */
  } else if (ATOMID(type) == ATOMID("hmhd")) {
    AddVersionAndFlags();
    
    AddProperty(new MP4Integer16Property("maxPduSize")); 
    AddProperty(new MP4Integer16Property("avgPduSize")); 
    AddProperty(new MP4Integer32Property("maxBitRate")); 
    AddProperty(new MP4Integer32Property("avgBitRate")); 
    AddProperty(new MP4Integer32Property("slidingAvgBitRate")); 
  /*
   * i???
   */
  } else if (ATOMID(type) == ATOMID("iKMS")) {
    AddVersionAndFlags(); /* 0, 1 */
    MP4StringProperty* pProp = new MP4StringProperty("kms_URI");
    AddProperty(pProp); /* 2 */

  } else if (ATOMID(type) == ATOMID("iSFM")) {
    AddVersionAndFlags(); /* 0, 1 */
    AddProperty( /* 2 */
		new MP4BitfieldProperty("selective-encryption", 1));
    AddProperty( /* 3 */
		new MP4BitfieldProperty("reserved", 7));
    AddProperty( /* 4 */
		new MP4Integer8Property("key-indicator-length"));	
    AddProperty( /* 5 */
		new MP4Integer8Property("IV-length"));	

  } else if (ATOMID(type) == ATOMID("ilst")) {
    ExpectChildAtom("\251nam", Optional, OnlyOne); /* name */
    ExpectChildAtom("\251ART", Optional, OnlyOne); /* artist */
    ExpectChildAtom("\251wrt", Optional, OnlyOne); /* writer */
    ExpectChildAtom("\251alb", Optional, OnlyOne); /* album */
    ExpectChildAtom("\251day", Optional, OnlyOne); /* date */
    ExpectChildAtom("\251too", Optional, OnlyOne); /* tool */
    ExpectChildAtom("\251cmt", Optional, OnlyOne); /* comment */
    ExpectChildAtom("\251gen", Optional, OnlyOne); /* custom genre */
    ExpectChildAtom("trkn", Optional, OnlyOne); /* tracknumber */
    ExpectChildAtom("disk", Optional, OnlyOne); /* disknumber */
    ExpectChildAtom("gnre", Optional, OnlyOne); /* genre (ID3v1 index + 1) */
    ExpectChildAtom("cpil", Optional, OnlyOne); /* compilation */
    ExpectChildAtom("tmpo", Optional, OnlyOne); /* BPM */
    ExpectChildAtom("covr", Optional, OnlyOne); /* cover art */
    ExpectChildAtom("aART", Optional, OnlyOne); /* album artist */
    ExpectChildAtom("----", Optional, Many); /* ---- free form */
    ExpectChildAtom("pgap", Optional, OnlyOne); /* part of gapless album */

  }  else if (ATOMID(type) == ATOMID("imif")) {
    AddVersionAndFlags();
    AddProperty(new MP4DescriptorProperty("ipmp_desc", MP4IPMPDescrTag,
					  MP4IPMPDescrTag, Required, Many));
  } else if (ATOMID(type) == ATOMID("iods")) {
    AddVersionAndFlags();
    AddProperty(
		new MP4DescriptorProperty(NULL, MP4FileIODescrTag, 
					  MP4FileODescrTag, 
					  Required, OnlyOne));
  /*
   * m???
   */
  } else if (ATOMID(type) == ATOMID("maxr")) {
    AddProperty(new MP4Integer32Property("granularity"));
    AddProperty(new MP4Integer32Property("bytes"));

  } else if (ATOMID(type) == ATOMID("mdia")) {
    ExpectChildAtom("mdhd", Required, OnlyOne);
    ExpectChildAtom("hdlr", Required, OnlyOne);
    ExpectChildAtom("minf", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("meta")) { // iTunes
    AddVersionAndFlags(); /* 0, 1 */
    ExpectChildAtom("hdlr", Required, OnlyOne);
    ExpectChildAtom("ilst", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("mfhd")) {
    AddVersionAndFlags();	/* 0, 1 */
    AddProperty( /* 2 */
		new MP4Integer32Property("sequenceNumber"));

  } else if (ATOMID(type) == ATOMID("minf")) {
    ExpectChildAtom("vmhd", Optional, OnlyOne);
    ExpectChildAtom("smhd", Optional, OnlyOne);
    ExpectChildAtom("hmhd", Optional, OnlyOne);
    ExpectChildAtom("nmhd", Optional, OnlyOne);
    ExpectChildAtom("gmhd", Optional, OnlyOne);
    ExpectChildAtom("dinf", Required, OnlyOne);
    ExpectChildAtom("stbl", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("moof")) {
    ExpectChildAtom("mfhd", Required, OnlyOne);
    ExpectChildAtom("traf", Optional, Many);

  } else if (ATOMID(type) == ATOMID("moov")) {
    ExpectChildAtom("mvhd", Required, OnlyOne);
    ExpectChildAtom("iods", Optional, OnlyOne);
    ExpectChildAtom("trak", Required, Many);
    ExpectChildAtom("udta", Optional, Many);
    ExpectChildAtom("mvex", Optional, OnlyOne);

  } else if (ATOMID(type) == ATOMID("mvex")) {
    ExpectChildAtom("trex", Required, Many);

  /*
   * n???
   */
  } else if (ATOMID(type) == ATOMID("nmhd")) {
    AddVersionAndFlags();

  } else if (ATOMID(type) == ATOMID("nump")) {
    AddProperty( // packets sent
		new MP4Integer64Property("packets"));
  /*
    * o???
    */
  } else if (ATOMID(type) == ATOMID("odkm")) {
    AddVersionAndFlags();
    ExpectChildAtom("ohdr", Required, OnlyOne);
  /*
   * p???
   */
  } else if (ATOMID(type) == ATOMID("payt")) {
    AddProperty(new MP4Integer32Property("payloadNumber"));
    AddProperty(new MP4StringProperty("rtpMap", Counted));

  } else if (ATOMID(type) == ATOMID("pinf")) {
    ExpectChildAtom("frma", Required, OnlyOne);
  } else if (ATOMID(type) == ATOMID("pmax")) {
    AddProperty( // max packet size 
		new MP4Integer32Property("bytes"));
  } else if (ATOMID(type) == ATOMID("pgap") ||
	     ATOMID(type) == ATOMID("plID") ||
	     ATOMID(type) == ATOMID("purd") ||
	     ATOMID(type) == ATOMID("rtng")) {
    ExpectChildAtom("data", Required, OnlyOne);
  /*
   * s???
   */
  } else if (ATOMID(type) == ATOMID("schi")) {
    // not sure if this is child atoms or table of boxes
    // get clarification on spec 9.1.2.5
    ExpectChildAtom("odkm", Optional, OnlyOne);
    ExpectChildAtom("iKMS", Optional, OnlyOne);
    ExpectChildAtom("iSFM", Optional, OnlyOne);

  } else if (ATOMID(type) == ATOMID("schm")) {
    AddVersionAndFlags(); /* 0, 1 */
    AddProperty( /* 2 */
                new MP4Integer32Property("scheme_type"));
    AddProperty( /* 3 */
	        new MP4Integer32Property("scheme_version"));
    // browser URI if flags set, TODO

  } else if (ATOMID(type) == ATOMID("sinf")) {
    ExpectChildAtom("frma", Required, OnlyOne);
    ExpectChildAtom("imif", Optional, OnlyOne);
    ExpectChildAtom("schm", Optional, OnlyOne);
    ExpectChildAtom("schi", Optional, OnlyOne);

  } else if (ATOMID(type) == ATOMID("smhd")) {
    AddVersionAndFlags();
    AddReserved("reserved", 4);

  } else if (ATOMID(type) == ATOMID("snro")) {
    AddProperty(new MP4Integer32Property("offset"));

  } else if (ATOMID(type) == ATOMID("stco")) {
    AddVersionAndFlags();

    MP4Integer32Property* pCount = new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);

    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);

    pTable->AddProperty(new MP4Integer32Property("chunkOffset"));

  } else if (ATOMID(type) == ATOMID("stsh")) {
    AddVersionAndFlags();

    MP4Integer32Property* pCount = new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);

    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);

    pTable->AddProperty(new MP4Integer32Property("shadowedSampleNumber"));
    pTable->AddProperty(new MP4Integer32Property("syncSampleNumber"));

  } else if (ATOMID(type) == ATOMID("stss")) {
    AddVersionAndFlags();

    MP4Integer32Property* pCount = new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);

    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);

    pTable->AddProperty(new MP4Integer32Property("sampleNumber"));

  } else if (ATOMID(type) == ATOMID("stts")) {
    AddVersionAndFlags();
    MP4Integer32Property* pCount = new MP4Integer32Property("entryCount"); 
    AddProperty(pCount);

    MP4TableProperty* pTable = new MP4TableProperty("entries", pCount);
    AddProperty(pTable);

    pTable->AddProperty(new MP4Integer32Property("sampleCount"));
    pTable->AddProperty(new MP4Integer32Property("sampleDelta"));
  } else if (ATOMID(type) == ATOMID("sfID") ||
	     ATOMID(type) == ATOMID("stik")) {
    ExpectChildAtom("data", Required, OnlyOne);
  /*
   * t???
   */
  } else if (ATOMID(type) == ATOMID("tims")) {
    AddProperty( 
		new MP4Integer32Property("timeScale"));

  } else if (ATOMID(type) == ATOMID("tmin")) {
    AddProperty( // min relative xmit time
		new MP4Integer32Property("milliSecs"));

  } else if (ATOMID(type) == ATOMID("tmax")) {
    AddProperty( // max relative xmit time
		new MP4Integer32Property("milliSecs"));

  } else if (ATOMID(type) == ATOMID("tmpo")) { // iTunes
    ExpectChildAtom("data", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("traf")) {
    ExpectChildAtom("tfhd", Required, OnlyOne);
    ExpectChildAtom("trun", Optional, Many);

  } else if (ATOMID(type) == ATOMID("trak")) {
    ExpectChildAtom("tkhd", Required, OnlyOne);
    ExpectChildAtom("tref", Optional, OnlyOne);
    ExpectChildAtom("edts", Optional, OnlyOne);
    ExpectChildAtom("mdia", Required, OnlyOne);
    ExpectChildAtom("udta", Optional, Many);

  } else if (ATOMID(type) == ATOMID("tref")) {
    ExpectChildAtom("chap", Optional, OnlyOne);
    ExpectChildAtom("dpnd", Optional, OnlyOne);
    ExpectChildAtom("hint", Optional, OnlyOne);
    ExpectChildAtom("ipir", Optional, OnlyOne);
    ExpectChildAtom("mpod", Optional, OnlyOne);
    ExpectChildAtom("sync", Optional, OnlyOne);

  } else if (ATOMID(type) == ATOMID("trex")) {
    AddVersionAndFlags();	/* 0, 1 */
    AddProperty( /* 2 */
		new MP4Integer32Property("trackId"));
    AddProperty( /* 3 */
		new MP4Integer32Property("defaultSampleDesriptionIndex"));
    AddProperty( /* 4 */
		new MP4Integer32Property("defaultSampleDuration"));
    AddProperty( /* 5 */
		new MP4Integer32Property("defaultSampleSize"));
    AddProperty( /* 6 */
		new MP4Integer32Property("defaultSampleFlags"));

  } else if (ATOMID(type) == ATOMID("trkn")) { // iTunes
    ExpectChildAtom("data", Required, OnlyOne);

  } else if (ATOMID(type) == ATOMID("trpy") ||
	     ATOMID(type) == ATOMID("tpyl")) {
    AddProperty( // bytes sent including RTP headers
		new MP4Integer64Property("bytes"));

  } else if (ATOMID(type) == ATOMID("tsro")) {
    AddProperty( 
		new MP4Integer32Property("offset"));
  } else if (ATOMID(type) == ATOMID("wave")) {
    ExpectChildAtom("esds", Required, OnlyOne);
  /*
   * copyright???
   */
  } else if (ATOMID(type) == ATOMID(art) ||
	     ATOMID(type) == ATOMID(wrt) ||
	     ATOMID(type) == ATOMID(alb) ||
	     ATOMID(type) == ATOMID(day) ||
	     ATOMID(type) == ATOMID(too) ||
	     ATOMID(type) == ATOMID(gen) ||
	     ATOMID(type) == ATOMID(grp)) { /* Apple iTunes */
    ExpectChildAtom("data", Required, OnlyOne);
  /*
   * ---- 
   */
  } else if (ATOMID(type) == ATOMID("----")) { /* Apple iTunes */
    ExpectChildAtom("mean", Required, OnlyOne);
    ExpectChildAtom("name", Required, OnlyOne);
    ExpectChildAtom("data", Required, OnlyOne);
  } else {
  /*
   * default - unknown type
   */
    SetUnknownType(true);
  }
  
}
