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
 *      M. Bakker     mbakker at nero.com
 *
 * Apple iTunes Metadata handling
 */

/**

   The iTunes tagging seems to support any tag field name
   but there are some predefined fields, also known from the QuickTime format

   predefined fields (the ones I know of until now):
   - ©nam : Name of the song/movie (string)
   - ©ART : Name of the artist/performer (string)
   - aART : Album artist
   - ©wrt : Name of the writer (string)
   - ©alb : Name of the album (string)
   - ©day : Year (4 bytes, e.g. "2003") (string)
   - ©too : Tool(s) used to create the file (string)
   - ©cmt : Comment (string)
   - ©gen : Custom genre (string)
   - ©grp : Grouping (string)
   - trkn : Tracknumber (8 byte string)
   16 bit: empty
   16 bit: tracknumber
   16 bit: total tracks on album
   16 bit: empty
   - disk : Disknumber (8 byte string)
   16 bit: empty
   16 bit: disknumber
   16 bit: total number of disks
   16 bit: empty
   - gnre : Genre (16 bit genre) (ID3v1 index + 1)
   - cpil : Part of a compilation (1 byte, 1 or 0)
   - tmpo : Tempo in BPM (16 bit)
   - covr : Cover art (xx bytes binary data)
   - ---- : Free form metadata, can have any name and any data
   - pgap : gapless - 8 bit boolean

   - apID : purchaser name.
   - cprt : copyright
   - purd : purchase date.

**/

#include "mp4common.h"

bool MP4File::GetMetadataByIndex(u_int32_t index,
                                 char** ppName,
                                 u_int8_t** ppValue, u_int32_t* pValueSize)
{
  char s[256];

  snprintf(s, 256, "moov.udta.meta.ilst.*[%u].data.metadata", index);
  GetBytesProperty(s, ppValue, pValueSize);

  snprintf(s, 256, "moov.udta.meta.ilst.*[%u]", index);
  MP4Atom* pParent = m_pRootAtom->FindAtom(s);
  if (pParent == NULL) return false;

  /* check for free form tagfield */
  if (memcmp(*ppName, "----", 4) == 0)
    {
      u_int8_t* pV;
      u_int32_t VSize = 0;
      char *pN;

      snprintf(s, 256, "moov.udta.meta.ilst.*[%u].name.metadata", index);
      GetBytesProperty(s, &pV, &VSize);

      pN = (char*)malloc((VSize+1)*sizeof(char));
      if (pN != NULL) {
	memset(pN, 0, (VSize+1)*sizeof(char));
	memcpy(pN, pV, VSize*sizeof(char));
      }
      free(pV);
      *ppName = pN;
    } else {
    *ppName = strdup(pParent->GetType());
  }

  return true;
}

bool MP4File::CreateMetadataAtom(const char* name)
{
  char s[256];
  char t[256];

  snprintf(t, 256, "udta.meta.ilst.%s.data", name);
  snprintf(s, 256, "moov.udta.meta.ilst.%s.data", name);
  (void)AddDescendantAtoms("moov", t);
  MP4Atom *pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    return false;

  /* some fields need special flags set */
  if ((uint8_t)name[0] == 0251 || ATOMID(name) == ATOMID("aART"))
    {
      pMetaAtom->SetFlags(0x1);
    } else if ((memcmp(name, "cpil", 4) == 0) || (memcmp(name, "tmpo", 4) == 0)) {
    pMetaAtom->SetFlags(0x15);
  }

  MP4Atom *pHdlrAtom = m_pRootAtom->FindAtom("moov.udta.meta.hdlr");
  MP4StringProperty *pStringProperty = NULL;
  MP4BytesProperty *pBytesProperty = NULL;
  ASSERT(pHdlrAtom);

  ASSERT(pHdlrAtom->FindProperty("hdlr.handlerType", 
				 (MP4Property**)&pStringProperty));
  ASSERT(pStringProperty);
  pStringProperty->SetValue("mdir");

  u_int8_t val[12];
  memset(val, 0, 12*sizeof(u_int8_t));
  val[0] = 0x61;
  val[1] = 0x70;
  val[2] = 0x70;
  val[3] = 0x6c;
  ASSERT(pHdlrAtom->FindProperty("hdlr.reserved2", 
				 (MP4Property**)&pBytesProperty));
  ASSERT(pBytesProperty);
  pBytesProperty->SetReadOnly(false);
  pBytesProperty->SetValue(val, 12);
  pBytesProperty->SetReadOnly(true);

  return true;
}

bool MP4File::DeleteMetadataAtom(const char* name, bool try_udta)
{
  MP4Atom *pMetaAtom = NULL;
  char s[256];

  snprintf(s, 256, "moov.udta.meta.ilst.%s", name);
  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (pMetaAtom == NULL && try_udta) {
    snprintf(s, 256, "moov.udta.%s", name);
    pMetaAtom = m_pRootAtom->FindAtom(s);
  }
  /* if it exists, delete it */
  if (pMetaAtom)
    {
      MP4Atom *pParent = pMetaAtom->GetParentAtom();

      pParent->DeleteChildAtom(pMetaAtom);

      delete pMetaAtom;

      return true;
    }

  return false;
}

bool MP4File::SetMetadataString (const char *atom, const char *value)
{
  char atomstring[40];
  MP4Atom *pMetaAtom;
  MP4BytesProperty *pMetadataProperty = NULL;
  snprintf(atomstring, 40, "moov.udta.meta.ilst.%s.data", atom);

  pMetaAtom = m_pRootAtom->FindAtom(atomstring);
  
  if (!pMetaAtom)
    {
      if (!CreateMetadataAtom(atom))
	return false;
      
      pMetaAtom = m_pRootAtom->FindAtom(atomstring);
      if (pMetaAtom == NULL) return false;
    }

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);
  
  pMetadataProperty->SetValue((u_int8_t*)value, strlen(value));
  
  return true;
}

bool MP4File::GetMetadataString (const char *atom, char **value, bool try_udta)
{
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  char atomstring[60];
  snprintf(atomstring, 60, "moov.udta.meta.ilst.%s.data.metadata", atom);

  *value = NULL;
  if (try_udta == false) {
    GetBytesProperty(atomstring, (u_int8_t**)&val, &valSize);
  } else {
    bool got_it = false;
    try {
      GetBytesProperty(atomstring, (u_int8_t**)&val, &valSize);
      got_it = true;
    }
    catch (MP4Error* e) {
      delete e;
    }
    if (got_it == false) {
      snprintf(atomstring, 60, "moov.udta.%s.metadata", atom);
      GetBytesProperty(atomstring, (u_int8_t**)&val, &valSize);
    }
  }
  if (valSize > 0)
    {
      *value = (char*)malloc((valSize+1)*sizeof(char));
      if (*value == NULL) {
	free(val);
	return false;
      }
      memcpy(*value, val, valSize*sizeof(unsigned char));
      free(val);
      (*value)[valSize] = '\0';
      return true;
    } 
  return false;
}

bool MP4File::SetMetadataTrack(u_int16_t track, u_int16_t totalTracks)
{
  unsigned char t[9];
  const char *s = "moov.udta.meta.ilst.trkn.data";
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;
    
  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    {
      if (!CreateMetadataAtom("trkn"))
	return false;

      pMetaAtom = m_pRootAtom->FindAtom(s);
      if (pMetaAtom == NULL) return false;
    }

  memset(t, 0, 9*sizeof(unsigned char));
  t[2] = (unsigned char)(track>>8)&0xFF;
  t[3] = (unsigned char)(track)&0xFF;
  t[4] = (unsigned char)(totalTracks>>8)&0xFF;
  t[5] = (unsigned char)(totalTracks)&0xFF;

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);

  pMetadataProperty->SetValue((u_int8_t*)t, 8);

  return true;
}

bool MP4File::GetMetadataTrack(u_int16_t* track, u_int16_t* totalTracks)
{
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  const char *s = "moov.udta.meta.ilst.trkn.data.metadata";

  *track = 0;
  *totalTracks = 0;

  GetBytesProperty(s, (u_int8_t**)&val, &valSize);

  if (valSize == 8) {
    *track = (u_int16_t)(val[3]);
    *track += (u_int16_t)(val[2]<<8);
    *totalTracks = (u_int16_t)(val[5]);
    *totalTracks += (u_int16_t)(val[4]<<8);
    CHECK_AND_FREE(val);
    return true;
  } 
  CHECK_AND_FREE(val);
  return false;
}

bool MP4File::SetMetadataDisk(u_int16_t disk, u_int16_t totalDisks)
{
  unsigned char t[7];
  const char *s = "moov.udta.meta.ilst.disk.data";
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;
    
  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    {
      if (!CreateMetadataAtom("disk"))
	return false;

      pMetaAtom = m_pRootAtom->FindAtom(s);
      if (pMetaAtom == NULL) return false;
    }

  memset(t, 0, 7*sizeof(unsigned char));
  t[2] = (unsigned char)(disk>>8)&0xFF;
  t[3] = (unsigned char)(disk)&0xFF;
  t[4] = (unsigned char)(totalDisks>>8)&0xFF;
  t[5] = (unsigned char)(totalDisks)&0xFF;

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);

  pMetadataProperty->SetValue((u_int8_t*)t, 6);

  return true;
}

bool MP4File::GetMetadataDisk(u_int16_t* disk, u_int16_t* totalDisks)
{
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  const char *s = "moov.udta.meta.ilst.disk.data.metadata";

  *disk = 0;
  *totalDisks = 0;

  GetBytesProperty(s, (u_int8_t**)&val, &valSize);

  if (valSize == 6 || valSize == 8) {
    *disk = (u_int16_t)(val[3]);
    *disk += (u_int16_t)(val[2]<<8);
    *totalDisks = (u_int16_t)(val[5]);
    *totalDisks += (u_int16_t)(val[4]<<8);
    free(val);
    return true;
  }
  CHECK_AND_FREE(val);
  return true;
}

static const char* ID3v1GenreList[] = {
  "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
  "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies",
  "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
  "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
  "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
  "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
  "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
  "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
  "Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
  "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
  "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
  "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
  "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical",
  "Rock & Roll", "Hard Rock", "Folk", "Folk/Rock", "National Folk", "Swing",
  "Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
  "Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
  "Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
  "Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
  "Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
  "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet",
  "Punk Rock", "Drum Solo", "A capella", "Euro-House", "Dance Hall",
  "Goa", "Drum & Bass", "Club House", "Hardcore", "Terror",
  "Indie", "BritPop", "NegerPunk", "Polsk Punk", "Beat",
  "Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
  "Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop",
  "SynthPop",
};

void GenreToString(char** GenreStr, const int genre)
{
  if (genre > 0 && 
      genre <= (int)(sizeof(ID3v1GenreList)/sizeof(*ID3v1GenreList)))
    {
      uint len = strlen(ID3v1GenreList[genre-1])+1;
      *GenreStr = (char*)malloc(len);
      if (*GenreStr == NULL) return;
      // no need for strncpy; enough was malloced
      strcpy(*GenreStr, ID3v1GenreList[genre-1]);
      return;
    } 
  *GenreStr = (char*)malloc(2*sizeof(char));
  if (*GenreStr == NULL) return;
  memset(*GenreStr, 0, 2*sizeof(char));
  return;
}

int StringToGenre(const char* GenreStr)
{
  unsigned int i;

  for (i = 0; i < sizeof(ID3v1GenreList)/sizeof(*ID3v1GenreList); i++)
    {
      if (strcasecmp(GenreStr, ID3v1GenreList[i]) == 0)
	return i+1;
    }
  return 0;
}

bool MP4File::SetMetadataGenre(const char* value)
{
  u_int16_t genreIndex = 0;
  unsigned char t[3];
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;

  genreIndex = StringToGenre(value);

  const char *s = "moov.udta.meta.ilst.gnre.data";
  const char *sroot = "moov.udta.meta.ilst.gnre";
  const char *s2 = "moov.udta.meta.ilst.\251gen.data";
  const char *s2root = "moov.udta.meta.ilst.\251gen";
  if (genreIndex != 0)
    {
      pMetaAtom = m_pRootAtom->FindAtom(s);
      if (!pMetaAtom)
        {
	  if (!CreateMetadataAtom("gnre"))
	    return false;

	  pMetaAtom = m_pRootAtom->FindAtom(s);
	  if (pMetaAtom == NULL) return false;
        }

      memset(t, 0, 3*sizeof(unsigned char));
      t[0] = (unsigned char)(genreIndex>>8)&0xFF;
      t[1] = (unsigned char)(genreIndex)&0xFF;

      ASSERT(pMetaAtom->FindProperty("data.metadata", 
				     (MP4Property**)&pMetadataProperty));
      ASSERT(pMetadataProperty);

      pMetadataProperty->SetValue((u_int8_t*)t, 2);
	
      // remove other style of genre atom, if this one is added
      pMetaAtom = m_pRootAtom->FindAtom(s2root);
      if (pMetaAtom != NULL) {
	MP4Atom *pParent = pMetaAtom->GetParentAtom();
	if (pParent != NULL) {
	  pParent->DeleteChildAtom(pMetaAtom);
	  delete pMetaAtom;
	}
      }
	  

      (void)DeleteMetadataAtom( "\251gen" );

      return true;
    } else {
    pMetaAtom = m_pRootAtom->FindAtom(s2);

    if (!pMetaAtom)
      {
	if (!CreateMetadataAtom("\251gen"))
	  return false;

	pMetaAtom = m_pRootAtom->FindAtom(s2);
      }

    ASSERT(pMetaAtom->FindProperty("data.metadata", 
				   (MP4Property**)&pMetadataProperty));
    ASSERT(pMetadataProperty);

    pMetadataProperty->SetValue((u_int8_t*)value, strlen(value));

    // remove other gnre atom if this one is entered
    pMetaAtom = m_pRootAtom->FindAtom(sroot);
    if (pMetaAtom != NULL) {
      MP4Atom *pParent = pMetaAtom->GetParentAtom();
      pParent->DeleteChildAtom(pMetaAtom);
      delete pMetaAtom;
    }
    return true;
  }

  return false;
}

bool MP4File::GetMetadataGenre(char** value)
{
  u_int16_t genreIndex = 0;
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  const char *t = "moov.udta.meta.ilst.gnre";
  const char *s = "moov.udta.meta.ilst.gnre.data.metadata";

  *value = NULL;

  MP4Atom *gnre = FindAtom(t);

  if (gnre)
    {
      GetBytesProperty(s, (u_int8_t**)&val, &valSize);

      if (valSize != 2) {
	CHECK_AND_FREE(val);
	return false;
      }

      genreIndex = (u_int16_t)(val[1]);
      genreIndex += (u_int16_t)(val[0]<<8);

      GenreToString(value, genreIndex);

      (void)DeleteMetadataAtom( "gnre" );
      free(val);
      return true;
    } else {
    const char *s2 = "moov.udta.meta.ilst.\251gen.data.metadata";

    val = NULL;
    valSize = 0;

    GetBytesProperty(s2, (u_int8_t**)&val, &valSize);

    if (valSize > 0)
      {
	*value = (char*)malloc((valSize+1)*sizeof(unsigned char));
	if (*value != NULL) {
	  memset(*value, 0, (valSize+1)*sizeof(unsigned char));
	  memcpy(*value, val, valSize*sizeof(unsigned char));
	}
	free(val);
	return true;
      } else {
      CHECK_AND_FREE(val);
    }
  }

  return false;
}

bool MP4File::DeleteMetadataGenre()
{
  bool val1 = DeleteMetadataAtom("\251gen");
  bool val2 = DeleteMetadataAtom("gnre");
  return val1 || val2;
}

bool MP4File::SetMetadataTempo(u_int16_t tempo)
{
  unsigned char t[3];
  const char *s = "moov.udta.meta.ilst.tmpo.data";
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;
    
  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    {
      if (!CreateMetadataAtom("tmpo"))
	return false;

      pMetaAtom = m_pRootAtom->FindAtom(s);
      if (pMetaAtom == NULL) return false;
    }

  memset(t, 0, 3*sizeof(unsigned char));
  t[0] = (unsigned char)(tempo>>8)&0xFF;
  t[1] = (unsigned char)(tempo)&0xFF;

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);

  pMetadataProperty->SetValue((u_int8_t*)t, 2);

  return true;
}

bool MP4File::GetMetadataTempo(u_int16_t* tempo)
{
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  const char *s = "moov.udta.meta.ilst.tmpo.data.metadata";

  *tempo = 0;

  GetBytesProperty(s, (u_int8_t**)&val, &valSize);

  if (valSize != 2) {
    CHECK_AND_FREE(val);
    return false;
  }

  *tempo = (u_int16_t)(val[1]);
  *tempo += (u_int16_t)(val[0]<<8);
  free(val);
  return true;
}
bool MP4File::SetMetadataUint8 (const char *atom, uint8_t value)
{
  char atompath[36];
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;

  snprintf(atompath, 36, "moov.udta.meta.ilst.%s.data", atom);

  pMetaAtom = m_pRootAtom->FindAtom(atompath);

  if (pMetaAtom == NULL) {
    if (!CreateMetadataAtom(atom))
      return false;

    pMetaAtom = m_pRootAtom->FindAtom(atompath);
    if (pMetaAtom == NULL) return false;
  }

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);
  
  pMetadataProperty->SetValue(&value, 1);

  return true;
}


bool MP4File::GetMetadataUint8(const char *atom, u_int8_t* retvalue)
{
  unsigned char *val = NULL;
  u_int32_t valSize = 0;
  char atompath[80];
  snprintf(atompath, 80, "moov.udta.meta.ilst.%s.data.metadata", atom);

  *retvalue = 0;

  GetBytesProperty(atompath, (u_int8_t**)&val, &valSize);

  if (valSize != 1) {
    CHECK_AND_FREE(val);
    return false;
  }

  *retvalue = val[0];
  free(val);
  return true;
}

bool MP4File::SetMetadataCoverArt(u_int8_t *coverArt, u_int32_t size)
{
  const char *s = "moov.udta.meta.ilst.covr.data";
  MP4BytesProperty *pMetadataProperty = NULL;
  MP4Atom *pMetaAtom = NULL;
    
  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    {
      if (!CreateMetadataAtom("covr"))
	return false;

      pMetaAtom = m_pRootAtom->FindAtom(s);
      if (pMetaAtom == NULL) return false;
    }

  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);

  pMetadataProperty->SetValue(coverArt, size);

  return true;
}

bool MP4File::GetMetadataCoverArt(u_int8_t **coverArt, u_int32_t *size,
				  uint32_t index)
{
  char buffer[256];
  if (size == NULL || coverArt == NULL) return false;

  if (index > 0 && index > GetMetadataCoverArtCount()) return false;

  snprintf(buffer, 256, "moov.udta.meta.ilst.covr.data[%d].metadata", index);

  *coverArt = NULL;
  *size = 0;

  GetBytesProperty(buffer, coverArt, size);

  if (size == 0)
    return false;

  return true;
}

u_int32_t MP4File::GetMetadataCoverArtCount (void)
{
  MP4Atom *pMetaAtom = m_pRootAtom->FindAtom("moov.udta.meta.ilst.covr");
  if (!pMetaAtom)
    return 0;

  return pMetaAtom->GetNumberOfChildAtoms();
}

bool MP4File::SetMetadataFreeForm (const char *name, 
				   const u_int8_t* pValue, 
				   u_int32_t valueSize,
				   const char *owner)
{
  MP4Atom *pMetaAtom = NULL;
  MP4BytesProperty *pMetadataProperty = NULL;
  char s[256];
  int i = 0;
  size_t nameLen = strlen(name);
  size_t ownerLen = owner != NULL ? strlen(owner) : 0;

  while (1)
    {
      snprintf(s, 256, "moov.udta.meta.ilst.----[%u].name", i);

      MP4Atom *pTagAtom = m_pRootAtom->FindAtom(s);

      if (!pTagAtom)
	break;

      snprintf(s, 256, "moov.udta.meta.ilst.----[%u].mean", i);

      MP4Atom *pMeanAtom = m_pRootAtom->FindAtom(s);

      if (pTagAtom->FindProperty("name.metadata", 
				 (MP4Property**)&pMetadataProperty) &&
	  pMetadataProperty) {
	u_int8_t* pV;
	u_int32_t VSize = 0;

	pMetadataProperty->GetValue(&pV, &VSize);

	if (VSize == nameLen && memcmp(pV, name, VSize) == 0) {
	  u_int8_t* pOwner=0;
	  u_int32_t ownerSize = 0;
	  
	  if (pMeanAtom && 
	      pMeanAtom->FindProperty("mean.metadata", 
				      (MP4Property**)&pMetadataProperty) &&
	      pMetadataProperty) {
	    pMetadataProperty->GetValue(&pOwner, &ownerSize);
	  }
	  
	  if (owner == NULL|| 
	      (pOwner && 
	       ownerLen == ownerSize && 
	       memcmp(owner, pOwner, ownerSize))) {
	    snprintf(s, 256, "moov.udta.meta.ilst.----[%u].data.metadata", i);
	    SetBytesProperty(s, pValue, valueSize);
	    CHECK_AND_FREE(pV);
	    CHECK_AND_FREE(pOwner);
	    
	    return true;
	  }
	  CHECK_AND_FREE(pOwner);
	}
	CHECK_AND_FREE(pV);
      }
      
      i++;
    }

  /* doesn't exist yet, create it */
  char t[256];

  snprintf(t, 256, "udta.meta.ilst.----[%u]", i);
  snprintf(s, 256, "moov.udta.meta.ilst.----[%u].data", i);
  (void)AddDescendantAtoms("moov", t);

  pMetaAtom = m_pRootAtom->FindAtom(s);

  if (!pMetaAtom)
    return false;

  pMetaAtom->SetFlags(0x1);

  MP4Atom *pHdlrAtom = m_pRootAtom->FindAtom("moov.udta.meta.hdlr");
  MP4StringProperty *pStringProperty = NULL;
  MP4BytesProperty *pBytesProperty = NULL;
  ASSERT(pHdlrAtom);

  ASSERT(pHdlrAtom->FindProperty("hdlr.handlerType", 
				 (MP4Property**)&pStringProperty));
  ASSERT(pStringProperty);
  pStringProperty->SetValue("mdir");

  u_int8_t val[12];
  memset(val, 0, 12*sizeof(u_int8_t));
  val[0] = 0x61;
  val[1] = 0x70;
  val[2] = 0x70;
  val[3] = 0x6c;
  ASSERT(pHdlrAtom->FindProperty("hdlr.reserved2", 
				 (MP4Property**)&pBytesProperty));
  ASSERT(pBytesProperty);
  pBytesProperty->SetReadOnly(false);
  pBytesProperty->SetValue(val, 12);
  pBytesProperty->SetReadOnly(true);

  pMetaAtom = m_pRootAtom->FindAtom(s);
  ASSERT(pMetaAtom);
  ASSERT(pMetaAtom->FindProperty("data.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);
  pMetadataProperty->SetValue(pValue, valueSize);

  snprintf(s, 256, "moov.udta.meta.ilst.----[%u].name", i);
  pMetaAtom = m_pRootAtom->FindAtom(s);
  ASSERT(pMetaAtom->FindProperty("name.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);
  pMetadataProperty->SetValue((const u_int8_t*)name, strlen(name));

  snprintf(s, 256, "moov.udta.meta.ilst.----[%u].mean", i);
  pMetaAtom = m_pRootAtom->FindAtom(s);
  ASSERT(pMetaAtom->FindProperty("mean.metadata", 
				 (MP4Property**)&pMetadataProperty));
  ASSERT(pMetadataProperty);
  if (!owner || !*owner)
    pMetadataProperty->SetValue((u_int8_t*)"com.apple.iTunes", 16); /* com.apple.iTunes is the default*/
  else
    pMetadataProperty->SetValue((const u_int8_t*)owner, strlen((const char *)owner));

  return true;
}

bool MP4File::GetMetadataFreeForm(const char *name, 
				  u_int8_t** ppValue, 
				  u_int32_t *pValueSize,
				  const char *owner)
{
  char s[256];
  int i = 0;

  *ppValue = NULL;
  *pValueSize = 0;

  size_t nameLen = strlen(name);
  size_t ownerLen = owner?strlen(owner):0;

  while (1)
    {
      MP4BytesProperty *pMetadataProperty;

      snprintf(s, 256,"moov.udta.meta.ilst.----[%u].name", i);
      MP4Atom *pTagAtom = m_pRootAtom->FindAtom(s);

      snprintf(s, 256,"moov.udta.meta.ilst.----[%u].mean", i);
      MP4Atom *pMeanAtom = m_pRootAtom->FindAtom(s);

      if (!pTagAtom)
	return false;

      if (pTagAtom->FindProperty("name.metadata", 
				 (MP4Property**)&pMetadataProperty) &&
	  pMetadataProperty) {
	u_int8_t* pV;
	u_int32_t VSize = 0;

	pMetadataProperty->GetValue(&pV, &VSize);

	if (VSize == nameLen && memcmp(pV, name, VSize) == 0) {
	    u_int8_t* pOwner=0;
	    u_int32_t ownerSize = 0;

	    if (pMeanAtom && pMeanAtom->FindProperty("mean.metadata", 
						     (MP4Property**)&pMetadataProperty) &&
		pMetadataProperty) {
	      pMetadataProperty->GetValue(&pOwner, &ownerSize);
	    }

	    if (!owner || (pOwner && ownerLen == ownerSize && memcmp(owner, pOwner, ownerSize))) {
	      snprintf(s, 256, "moov.udta.meta.ilst.----[%u].data.metadata", i);
	      GetBytesProperty(s, ppValue, pValueSize);
	      CHECK_AND_FREE(pV);
	      CHECK_AND_FREE(pOwner);
	      return true;	
	    }
	    CHECK_AND_FREE(pOwner);
	}
	CHECK_AND_FREE(pV);
      }
      
      i++;
    }
}

bool MP4File::DeleteMetadataFreeForm(const char *name, const char *owner)
{
  char s[256];
  int i = 0;

  size_t nameLen = strlen(name);
  size_t ownerLen = owner?strlen(owner):0;

  while (1)
    {
      MP4BytesProperty *pMetadataProperty;

      snprintf(s, 256, "moov.udta.meta.ilst.----[%u].name", i);
      MP4Atom *pTagAtom = m_pRootAtom->FindAtom(s);

      snprintf(s, 256,"moov.udta.meta.ilst.----[%u].mean", i);
      MP4Atom *pMeanAtom = m_pRootAtom->FindAtom(s);


      if (!pTagAtom)
	return false;

      if (pTagAtom->FindProperty("name.metadata", 
				 (MP4Property**)&pMetadataProperty) &&
	  pMetadataProperty) {
	u_int8_t* pV;
	u_int32_t VSize = 0;

	pMetadataProperty->GetValue(&pV, &VSize);

	if (VSize != 0)
	  {
	    if (VSize == nameLen && memcmp(pV, name, VSize) == 0)
	      {
		u_int8_t* pOwner=0;
		u_int32_t ownerSize = 0;

		if (pMeanAtom && pMeanAtom->FindProperty("mean.metadata", 
							 (MP4Property**)&pMetadataProperty) &&
		    pMetadataProperty) 
		  {
		    pMetadataProperty->GetValue(&pOwner, &ownerSize);
		  }

		if (!owner || (pOwner && ownerLen == ownerSize && memcmp(owner, pOwner, ownerSize)))
		  {
		    snprintf(s, 256, "----[%u]", i);
		    CHECK_AND_FREE(pOwner);
		    CHECK_AND_FREE(pV);
		    return DeleteMetadataAtom(s);
		  }
		CHECK_AND_FREE(pOwner);

	      }
	  }
	CHECK_AND_FREE(pV);
      }

      i++;
    }
}

bool MP4File::MetadataDelete()
{
  MP4Atom *pMetaAtom = NULL;
  char s[256];

  snprintf(s, 256, "moov.udta.meta");
  pMetaAtom = m_pRootAtom->FindAtom(s);

  /* if it exists, delete it */
  if (pMetaAtom)
    {
      MP4Atom *pParent = pMetaAtom->GetParentAtom();

      pParent->DeleteChildAtom(pMetaAtom);

      delete pMetaAtom;

      return true;
    }

  return false;
}
