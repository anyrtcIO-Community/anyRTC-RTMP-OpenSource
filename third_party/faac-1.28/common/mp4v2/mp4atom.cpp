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
 * Copyright (C) Cisco Systems Inc. 2001 - 2004.  All Rights Reserved.
 * 
 * 3GPP features implementation is based on 3GPP's TS26.234-v5.60,
 * and was contributed by Ximpo Group Ltd.
 *
 * Portions created by Ximpo Group Ltd. are
 * Copyright (C) Ximpo Group Ltd. 2003, 2004.  All Rights Reserved.
 * 
 *  Portions created by Adnecto d.o.o. are
 *  Copyright (C) Adnecto d.o.o. 2005.  All Rights Reserved
 *
 * Contributor(s): 
 *		Dave Mackie			dmackie@cisco.com
 *		Alix Marchandise-Franquet	alix@cisco.com
 *              Ximpo Group Ltd.                mp4v2@ximpo.com
 *              Danijel Kopcinovic              danijel.kopcinovic@adnecto.net
 */

#include "mp4common.h"
#include "atoms.h"

MP4AtomInfo::MP4AtomInfo(const char* name, bool mandatory, bool onlyOne) 
{
	m_name = name;
	m_mandatory = mandatory;
	m_onlyOne = onlyOne;
	m_count = 0;
}

MP4Atom::MP4Atom(const char* type) 
{
	SetType(type);
	m_unknownType = FALSE;
	m_pFile = NULL;
	m_start = 0;
	m_end = 0;
	m_size = 0;
	m_pParentAtom = NULL;
	m_depth = 0xFF;
}

MP4Atom::~MP4Atom()
{
	u_int32_t i;

	for (i = 0; i < m_pProperties.Size(); i++) {
		delete m_pProperties[i];
	}
	for (i = 0; i < m_pChildAtomInfos.Size(); i++) {
		delete m_pChildAtomInfos[i];
	}
	for (i = 0; i < m_pChildAtoms.Size(); i++) {
		delete m_pChildAtoms[i];
	}
}

MP4Atom* MP4Atom::CreateAtom(const char* type)
{
  MP4Atom* pAtom = NULL;

  if (type == NULL) {
    pAtom = new MP4RootAtom();
  } else {
    switch((uint8_t)type[0]) {
    case 'a':
      if (ATOMID(type) == ATOMID("avc1")) {
	pAtom = new MP4Avc1Atom();
      } else if (ATOMID(type) == ATOMID("avcC")) {
	pAtom = new MP4AvcCAtom();
      } else if (ATOMID(type) == ATOMID("alis")) {
	pAtom = new MP4UrlAtom("alis");
      } else if (ATOMID(type) == ATOMID("alaw")) {
	pAtom = new MP4SoundAtom(type);
      } else if (ATOMID(type) == ATOMID("alac")) {
	pAtom = new MP4SoundAtom(type);
      }
      break;
    case 'c':
      if (ATOMID(type) == ATOMID("chap")) {
	pAtom = new MP4TrefTypeAtom(type);
	  }
	  break;
    case 'd':
      if (ATOMID(type) == ATOMID("d263")) {
	pAtom = new MP4D263Atom();
      } else if (ATOMID(type) == ATOMID("damr")) {
	pAtom = new MP4DamrAtom();
      } else if (ATOMID(type) == ATOMID("dref")) {
	pAtom = new MP4DrefAtom();
      } else if (ATOMID(type) == ATOMID("dpnd")) {
	pAtom = new MP4TrefTypeAtom(type);
      } else if (ATOMID(type) == ATOMID("data")) { /* Apple iTunes */
	pAtom = new MP4DataAtom();
      }
      break;
    case 'e':
      if (ATOMID(type) == ATOMID("elst")) {
	pAtom = new MP4ElstAtom();
      } else if (ATOMID(type) == ATOMID("enca")) {
	pAtom = new MP4EncaAtom();
      } else if (ATOMID(type) == ATOMID("encv")) {
	pAtom = new MP4EncvAtom();
      }
      break;
    case 'f':
      if (ATOMID(type) == ATOMID("free")) {
	pAtom = new MP4FreeAtom();
      } else if (ATOMID(type) == ATOMID("ftyp")) {
	pAtom = new MP4FtypAtom();
      }
      break;
    case 'g':
      if (ATOMID(type) == ATOMID("gmin")) {
	pAtom = new MP4GminAtom();
      }
      break;
    case 'h':
      if (ATOMID(type) == ATOMID("hdlr")) {
	pAtom = new MP4HdlrAtom();
      } else if (ATOMID(type) == ATOMID("hint")) {
	pAtom = new MP4TrefTypeAtom(type);
      } else if (ATOMID(type) == ATOMID("hnti")) {
	pAtom = new MP4HntiAtom();
      } else if (ATOMID(type) == ATOMID("hinf")) {
	pAtom = new MP4HinfAtom();
      } else if (ATOMID(type) == ATOMID("h263")) {
	pAtom = new MP4VideoAtom("h263");
      } else if (ATOMID(type) == ATOMID("href")) {
	pAtom = new MP4HrefAtom();
      }
      break;
    case 'i':
      if (ATOMID(type) == ATOMID("ipir")) {
	pAtom = new MP4TrefTypeAtom(type);
      } else if (ATOMID(type) == ATOMID("ima4")) {
	pAtom = new MP4SoundAtom("ima4");
      }
      break;
    case 'j':
      if (ATOMID(type) == ATOMID("jpeg")) {
	pAtom = new MP4VideoAtom("jpeg");
      }
      break;
    case 'm':
      if (ATOMID(type) == ATOMID("mdhd")) {
	pAtom = new MP4MdhdAtom();
      } else if (ATOMID(type) == ATOMID("mvhd")) {
	pAtom = new MP4MvhdAtom();
      } else if (ATOMID(type) == ATOMID("mdat")) {
	pAtom = new MP4MdatAtom();
      } else if (ATOMID(type) == ATOMID("mpod")) {
	pAtom = new MP4TrefTypeAtom(type);
      } else if (ATOMID(type) == ATOMID("mp4a")) {
	pAtom = new MP4SoundAtom("mp4a");
      } else if (ATOMID(type) == ATOMID("mp4s")) {
	pAtom = new MP4Mp4sAtom();
      } else if (ATOMID(type) == ATOMID("mp4v")) {
	pAtom = new MP4Mp4vAtom();
      } else if (ATOMID(type) == ATOMID("mean")) { // iTunes
	pAtom = new MP4Meta1Atom(type);
      }
      break;
    case 'n':
      if (ATOMID(type) == ATOMID("name")) { // iTunes
	pAtom = new MP4Meta1Atom(type);
      }
      break;
    case 'o':
      if (ATOMID(type) == ATOMID("ohdr")) {
	pAtom = new MP4OhdrAtom();
      }
      break;
    case 'r':
      if (ATOMID(type) == ATOMID("rtp ")) {
	pAtom = new MP4RtpAtom();
      } else if (ATOMID(type) == ATOMID("raw ")) {
	pAtom = new MP4VideoAtom("raw ");
      }
      break;
    case 's':
      if (ATOMID(type) == ATOMID("s263"))  {
        pAtom = new MP4S263Atom();
      } else if (ATOMID(type) == ATOMID("samr")) {
	pAtom = new MP4AmrAtom("samr");
      } else if (ATOMID(type) == ATOMID("sawb")) {
	pAtom = new MP4AmrAtom("sawb");
      } else if (ATOMID(type) == ATOMID("stbl")) {
	pAtom = new MP4StblAtom();
      } else if (ATOMID(type) == ATOMID("stsd")) {
	pAtom = new MP4StsdAtom();
      } else if (ATOMID(type) == ATOMID("stsz")) {
	pAtom = new MP4StszAtom();
      } else if (ATOMID(type) == ATOMID("stsc")) {
	pAtom = new MP4StscAtom();
      } else if (ATOMID(type) == ATOMID("stz2")) {
	pAtom = new MP4Stz2Atom();
      } else if (ATOMID(type) == ATOMID("stdp")) {
	pAtom = new MP4StdpAtom();
      } else if (ATOMID(type) == ATOMID("sdp ")) {
	pAtom = new MP4SdpAtom();
      } else if (ATOMID(type) == ATOMID("sync")) {
	pAtom = new MP4TrefTypeAtom(type);
      } else if (ATOMID(type) == ATOMID("skip")) {
	pAtom = new MP4FreeAtom();
	pAtom->SetType("skip");
      } else if (ATOMID(type) == ATOMID("sowt")) {
	pAtom = new MP4SoundAtom("sowt");
      }
      break;
    case 't':
      if (ATOMID(type) == ATOMID("text")) {
	pAtom = new MP4TextAtom();
      } else if (ATOMID(type) == ATOMID("tkhd")) {
	pAtom = new MP4TkhdAtom();
      } else if (ATOMID(type) == ATOMID("tfhd")) {
	pAtom = new MP4TfhdAtom();
      } else if (ATOMID(type) == ATOMID("trun")) {
	pAtom = new MP4TrunAtom();
      } else if (ATOMID(type) == ATOMID("twos")) {
	pAtom = new MP4SoundAtom("twos");
      }
      break;
    case 'u':
      if (ATOMID(type) == ATOMID("udta")) {
	pAtom = new MP4UdtaAtom();
      } else if (ATOMID(type) == ATOMID("url ")) {
	pAtom = new MP4UrlAtom();
      } else if (ATOMID(type) == ATOMID("urn ")) {
	pAtom = new MP4UrnAtom();
      } else if (ATOMID(type) == ATOMID("ulaw")) {
	pAtom = new MP4SoundAtom("ulaw");
      }
      break;
    case 'v':
      if (ATOMID(type) == ATOMID("vmhd")) {
	pAtom = new MP4VmhdAtom();
      }
      break;
    case 'y':
      if (ATOMID(type) == ATOMID("yuv2")) {
	pAtom = new MP4VideoAtom("yuv2");
      }
      break;
    case 'S':
      if (ATOMID(type) == ATOMID("SVQ3")) {
	pAtom = new MP4VideoAtom("SVQ3");
      } else if (ATOMID(type) == ATOMID("SMI ")) {
	pAtom = new MP4SmiAtom();
      }
      break;
    case 0251:
      static const char name[5]={0251,'n', 'a', 'm', '\0'};
      static const char cmt[5]={0251,'c', 'm', 't', '\0'};
      static const char cpy[5]={0251,'c', 'p', 'y', '\0'};
      static const char des[5]={0251,'d', 'e', 's','\0'};
      static const char prd[5]={0251, 'p', 'r', 'd', '\0'};
      if (ATOMID(type) == ATOMID(name) ||
	  ATOMID(type) == ATOMID(cmt) ||
	  ATOMID(type) == ATOMID(cpy) ||
	  ATOMID(type) == ATOMID(prd) ||
	  ATOMID(type) == ATOMID(des)) {
	pAtom = new MP4Meta2Atom(type);
      }
      break;
    }
  }

  if (pAtom == NULL) {
    pAtom = new MP4StandardAtom(type);
    // unknown type is set by StandardAtom type
  }

  ASSERT(pAtom);
  return pAtom;
}

// generate a skeletal self

void MP4Atom::Generate()
{
	u_int32_t i;

	// for all properties
	for (i = 0; i < m_pProperties.Size(); i++) {
		// ask it to self generate
		m_pProperties[i]->Generate();
	}

	// for all mandatory, single child atom types
	for (i = 0; i < m_pChildAtomInfos.Size(); i++) {
		if (m_pChildAtomInfos[i]->m_mandatory
		  && m_pChildAtomInfos[i]->m_onlyOne) {

			// create the mandatory, single child atom
			MP4Atom* pChildAtom = 
				CreateAtom(m_pChildAtomInfos[i]->m_name);

			AddChildAtom(pChildAtom);

			// and ask it to self generate
			pChildAtom->Generate();
		}
	}
}

MP4Atom* MP4Atom::ReadAtom(MP4File* pFile, MP4Atom* pParentAtom)
{
	u_int8_t hdrSize = 8;
	u_int8_t extendedType[16];

	u_int64_t pos = pFile->GetPosition();

	VERBOSE_READ(pFile->GetVerbosity(), 
		printf("ReadAtom: pos = 0x"X64"\n", pos));

	u_int64_t dataSize = pFile->ReadUInt32();

	char type[5];
	pFile->ReadBytes((u_int8_t*)&type[0], 4);
	type[4] = '\0';
	
	// extended size
	if (dataSize == 1) {
		dataSize = pFile->ReadUInt64(); 
		hdrSize += 8;
		pFile->Check64BitStatus(type);
	}

	// extended type
	if (ATOMID(type) == ATOMID("uuid")) {
		pFile->ReadBytes(extendedType, sizeof(extendedType));
		hdrSize += sizeof(extendedType);
	}

	if (dataSize == 0) {
		// extends to EOF
		dataSize = pFile->GetSize() - pos;
	}

	dataSize -= hdrSize;

	VERBOSE_READ(pFile->GetVerbosity(), 
		printf("ReadAtom: type = \"%s\" data-size = "U64" (0x"X64") hdr %u\n", 
		       type, dataSize, dataSize, hdrSize));

	if (pos + hdrSize + dataSize > pParentAtom->GetEnd()) {
		VERBOSE_ERROR(pFile->GetVerbosity(), 
			printf("ReadAtom: invalid atom size, extends outside parent atom - skipping to end of \"%s\" \"%s\" "U64" vs "U64"\n", 
			       pParentAtom->GetType(), type,
			       pos + hdrSize + dataSize, 
			       pParentAtom->GetEnd()));
		VERBOSE_READ(pFile->GetVerbosity(),
			     printf("parent %s ("U64") pos "U64" hdr %d data "U64" sum "U64"\n",
				    pParentAtom->GetType(),
				    pParentAtom->GetEnd(),
				    pos, 
				    hdrSize, 
				    dataSize,
				    pos + hdrSize + dataSize));
#if 0
		throw new MP4Error("invalid atom size", "ReadAtom");
#else
		// skip to end of atom
		dataSize = pParentAtom->GetEnd() - pos - hdrSize;
#endif
	}


	MP4Atom* pAtom = CreateAtom(type);
	pAtom->SetFile(pFile);
	pAtom->SetStart(pos);
	pAtom->SetEnd(pos + hdrSize + dataSize);
	pAtom->SetSize(dataSize);
	if (ATOMID(type) == ATOMID("uuid")) {
		pAtom->SetExtendedType(extendedType);
	}
	if (pAtom->IsUnknownType()) {
		if (!IsReasonableType(pAtom->GetType())) {
			VERBOSE_READ(pFile->GetVerbosity(),
				printf("Warning: atom type %s is suspect\n", pAtom->GetType()));
		} else {
			VERBOSE_READ(pFile->GetVerbosity(),
				printf("Info: atom type %s is unknown\n", pAtom->GetType()));
		}

		if (dataSize > 0) {
			pAtom->AddProperty(
				new MP4BytesProperty("data", dataSize));
		}
	}

	pAtom->SetParentAtom(pParentAtom);

	pAtom->Read();

	return pAtom;
}

bool MP4Atom::IsReasonableType(const char* type)
{
	for (u_int8_t i = 0; i < 4; i++) {
		if (isalnum(type[i])) {
			continue;
		}
		if (i == 3 && type[i] == ' ') {
			continue;
		}
		return false;
	}
	return true;
}

// generic read
void MP4Atom::Read()
{
	ASSERT(m_pFile);

	if (ATOMID(m_type) != 0 && m_size > 1000000) {
		VERBOSE_READ(GetVerbosity(), 
			printf("Warning: %s atom size "U64" is suspect\n",
				m_type, m_size));
	}

	ReadProperties();

	// read child atoms, if we expect there to be some
	if (m_pChildAtomInfos.Size() > 0) {
		ReadChildAtoms();
	}

	Skip();	// to end of atom
}

void MP4Atom::Skip()
{
	if (m_pFile->GetPosition() != m_end) {
		VERBOSE_READ(m_pFile->GetVerbosity(),
			printf("Skip: "U64" bytes\n", m_end - m_pFile->GetPosition()));
	}
	m_pFile->SetPosition(m_end);
}

MP4Atom* MP4Atom::FindAtom(const char* name)
{
	if (!IsMe(name)) {
		return NULL;
	}

	if (!IsRootAtom()) {
		VERBOSE_FIND(m_pFile->GetVerbosity(),
			printf("FindAtom: matched %s\n", name));

		name = MP4NameAfterFirst(name);

		// I'm the sought after atom 
		if (name == NULL) {
			return this;
		}
	}

	// else it's one of my children
	return FindChildAtom(name);
}

bool MP4Atom::FindProperty(const char *name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (!IsMe(name)) {
		return false;
	}

	if (!IsRootAtom()) {
		VERBOSE_FIND(m_pFile->GetVerbosity(),
			printf("FindProperty: matched %s\n", name));

		name = MP4NameAfterFirst(name);

		// no property name given
		if (name == NULL) {
			return false;
		}
	}

	return FindContainedProperty(name, ppProperty, pIndex);
}

bool MP4Atom::IsMe(const char* name)
{
	if (name == NULL) {
		return false;
	}

	// root atom always matches
	if (!strcmp(m_type, "")) {
		return true;
	}

	// check if our atom name is specified as the first component
	if (!MP4NameFirstMatches(m_type, name)) {
		return false;
	}

	return true;
}

MP4Atom* MP4Atom::FindChildAtom(const char* name)
{
	u_int32_t atomIndex = 0;

	// get the index if we have one, e.g. moov.trak[2].mdia...
	(void)MP4NameFirstIndex(name, &atomIndex);

	// need to get to the index'th child atom of the right type
	for (u_int32_t i = 0; i < m_pChildAtoms.Size(); i++) {
		if (MP4NameFirstMatches(m_pChildAtoms[i]->GetType(), name)) {
			if (atomIndex == 0) {
				// this is the one, ask it to match
				return m_pChildAtoms[i]->FindAtom(name);
			}
			atomIndex--;
		}
	}

	return NULL;
}

bool MP4Atom::FindContainedProperty(const char *name,
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	u_int32_t numProperties = m_pProperties.Size();
	u_int32_t i;
	// check all of our properties
	for (i = 0; i < numProperties; i++) {
		if (m_pProperties[i]->FindProperty(name, ppProperty, pIndex)) {
			return true;
		}
	}

	// not one of our properties, 
	// presumably one of our children's properties
	// check child atoms...

	// check if we have an index, e.g. trak[2].mdia...
	u_int32_t atomIndex = 0;
	(void)MP4NameFirstIndex(name, &atomIndex);

	// need to get to the index'th child atom of the right type
	for (i = 0; i < m_pChildAtoms.Size(); i++) {
		if (MP4NameFirstMatches(m_pChildAtoms[i]->GetType(), name)) {
			if (atomIndex == 0) {
				// this is the one, ask it to match
				return m_pChildAtoms[i]->FindProperty(name, ppProperty, pIndex);
			}
			atomIndex--;
		}
	}

	VERBOSE_FIND(m_pFile->GetVerbosity(),
		printf("FindProperty: no match for %s\n", name));
	return false;
}

void MP4Atom::ReadProperties(u_int32_t startIndex, u_int32_t count)
{
	u_int32_t numProperties = MIN(count, m_pProperties.Size() - startIndex);

	// read any properties of the atom
	for (u_int32_t i = startIndex; i < startIndex + numProperties; i++) {

		m_pProperties[i]->Read(m_pFile);

		if (m_pFile->GetPosition() > m_end) {
			VERBOSE_READ(GetVerbosity(), 
				printf("ReadProperties: insufficient data for property: %s pos 0x"X64" atom end 0x"X64"\n",
					m_pProperties[i]->GetName(), 
					m_pFile->GetPosition(), m_end)); 

			throw new MP4Error("atom is too small", "Atom ReadProperties");
		}

		if (m_pProperties[i]->GetType() == TableProperty) {
			VERBOSE_READ_TABLE(GetVerbosity(), 
				printf("Read: "); m_pProperties[i]->Dump(stdout, 0, true));
		} else if (m_pProperties[i]->GetType() != DescriptorProperty) {
			VERBOSE_READ(GetVerbosity(), 
				printf("Read: "); m_pProperties[i]->Dump(stdout, 0, true));
		}
	}
}

void MP4Atom::ReadChildAtoms()
{
  bool this_is_udta = ATOMID(m_type) == ATOMID("udta");

	VERBOSE_READ(GetVerbosity(), 
		printf("ReadChildAtoms: of %s\n", m_type[0] ? m_type : "root"));
	for (u_int64_t position = m_pFile->GetPosition();
	     position < m_end;
	     position = m_pFile->GetPosition()) {
	  // make sure that we have enough to read at least 8 bytes
	  // size and type.
	  if (m_end - position < 2 * sizeof(uint32_t)) {
	    // if we're reading udta, it's okay to have 4 bytes of 0
	    if (this_is_udta &&
		m_end - position == sizeof(uint32_t)) {
	      u_int32_t mbz = m_pFile->ReadUInt32();
	      if (mbz != 0) {
		VERBOSE_WARNING(GetVerbosity(),
				printf("Error: In udta atom, end value is not zero %x\n", 
				       mbz));
	      }
	      continue;
	    }
	    // otherwise, output a warning, but don't care
	    VERBOSE_WARNING(GetVerbosity(),
			    printf("Error: In %s atom, extra "D64" bytes at end of atom\n", 
				   m_type, (m_end - position)));
	    for (uint64_t ix = 0; ix < m_end - position; ix++) {
	      (void)m_pFile->ReadUInt8();
	    }
	    continue;
	  }
		MP4Atom* pChildAtom = MP4Atom::ReadAtom(m_pFile, this);

		AddChildAtom(pChildAtom);

		MP4AtomInfo* pChildAtomInfo = FindAtomInfo(pChildAtom->GetType());

		// if child atom is of known type
		// but not expected here print warning
		if (pChildAtomInfo == NULL && !pChildAtom->IsUnknownType()) {
			VERBOSE_READ(GetVerbosity(),
				printf("Warning: In atom %s unexpected child atom %s\n",
					GetType(), pChildAtom->GetType()));
		}

		// if child atoms should have just one instance
		// and this is more than one, print warning
		if (pChildAtomInfo) {
			pChildAtomInfo->m_count++;

			if (pChildAtomInfo->m_onlyOne && pChildAtomInfo->m_count > 1) {
				VERBOSE_READ(GetVerbosity(),
					printf("Warning: In atom %s multiple child atoms %s\n",
						GetType(), pChildAtom->GetType()));
			}
		}

	}

	// if mandatory child atom doesn't exist, print warning
	u_int32_t numAtomInfo = m_pChildAtomInfos.Size();
	for (u_int32_t i = 0; i < numAtomInfo; i++) {
		if (m_pChildAtomInfos[i]->m_mandatory
		  && m_pChildAtomInfos[i]->m_count == 0) {
				VERBOSE_READ(GetVerbosity(),
					printf("Warning: In atom %s missing child atom %s\n",
						GetType(), m_pChildAtomInfos[i]->m_name));
		}
	}

	VERBOSE_READ(GetVerbosity(), 
		printf("ReadChildAtoms: finished %s\n", m_type));
}

MP4AtomInfo* MP4Atom::FindAtomInfo(const char* name)
{
	u_int32_t numAtomInfo = m_pChildAtomInfos.Size();
	for (u_int32_t i = 0; i < numAtomInfo; i++) {
		if (ATOMID(m_pChildAtomInfos[i]->m_name) == ATOMID(name)) {
			return m_pChildAtomInfos[i];
		}
	}
	return NULL;
}

// generic write
void MP4Atom::Write()
{
	ASSERT(m_pFile);

	BeginWrite();

	WriteProperties();

	WriteChildAtoms();

	FinishWrite();
}

void MP4Atom::Rewrite()
{
       ASSERT(m_pFile);
       
       if (!m_end) {
	       // This atom hasn't been written yet...
	       return;
       }
       
       u_int64_t fPos = m_pFile->GetPosition();
       m_pFile->SetPosition(GetStart());
       Write();
       m_pFile->SetPosition(fPos);
}

void MP4Atom::BeginWrite(bool use64)
{
	m_start = m_pFile->GetPosition();
	//use64 = m_pFile->Use64Bits();
	if (use64) {
		m_pFile->WriteUInt32(1);
	} else {
		m_pFile->WriteUInt32(0);
	}
	m_pFile->WriteBytes((u_int8_t*)&m_type[0], 4);
	if (use64) {
		m_pFile->WriteUInt64(0);
	}
	if (ATOMID(m_type) == ATOMID("uuid")) {
		m_pFile->WriteBytes(m_extendedType, sizeof(m_extendedType));
	}
}

void MP4Atom::FinishWrite(bool use64)
{
	m_end = m_pFile->GetPosition();
	m_size = (m_end - m_start);
  VERBOSE_WRITE(GetVerbosity(), 
		printf("end: type %s "U64" "U64" size "U64"\n", m_type, 
		       m_start, m_end,
		       m_size));
	//use64 = m_pFile->Use64Bits();
	if (use64) {
		m_pFile->SetPosition(m_start + 8);
		m_pFile->WriteUInt64(m_size);
	} else {
		ASSERT(m_size <= (u_int64_t)0xFFFFFFFF);
		m_pFile->SetPosition(m_start);
		m_pFile->WriteUInt32(m_size);
	}
	m_pFile->SetPosition(m_end);

	// adjust size to just reflect data portion of atom
	m_size -= (use64 ? 16 : 8);
	if (ATOMID(m_type) == ATOMID("uuid")) {
		m_size -= sizeof(m_extendedType);
	}
}

void MP4Atom::WriteProperties(u_int32_t startIndex, u_int32_t count)
{
	u_int32_t numProperties = MIN(count, m_pProperties.Size() - startIndex);

	VERBOSE_WRITE(GetVerbosity(), 
		printf("Write: type %s\n", m_type));

	for (u_int32_t i = startIndex; i < startIndex + numProperties; i++) {
		m_pProperties[i]->Write(m_pFile);

		if (m_pProperties[i]->GetType() == TableProperty) {
			VERBOSE_WRITE_TABLE(GetVerbosity(), 
				printf("Write: "); m_pProperties[i]->Dump(stdout, 0, false));
		} else {
			VERBOSE_WRITE(GetVerbosity(), 
				printf("Write: "); m_pProperties[i]->Dump(stdout, 0, false));
		}
	}
}

void MP4Atom::WriteChildAtoms()
{
	u_int32_t size = m_pChildAtoms.Size();
	for (u_int32_t i = 0; i < size; i++) {
		m_pChildAtoms[i]->Write();
	}

	VERBOSE_WRITE(GetVerbosity(), 
		printf("Write: finished %s\n", m_type));
}

void MP4Atom::AddProperty(MP4Property* pProperty) 
{
	ASSERT(pProperty);
	m_pProperties.Add(pProperty);
	pProperty->SetParentAtom(this);
}

void MP4Atom::AddVersionAndFlags()
{
	AddProperty(new MP4Integer8Property("version"));
	AddProperty(new MP4Integer24Property("flags"));
}

void MP4Atom::AddReserved(char* name, u_int32_t size) 
{
	MP4BytesProperty* pReserved = new MP4BytesProperty(name, size); 
	pReserved->SetReadOnly();
	AddProperty(pReserved);
}

void MP4Atom::ExpectChildAtom(const char* name, bool mandatory, bool onlyOne)
{
	m_pChildAtomInfos.Add(new MP4AtomInfo(name, mandatory, onlyOne));
}

u_int8_t MP4Atom::GetVersion()
{
	if (strcmp("version", m_pProperties[0]->GetName())) {
		return 0;
	}
	return ((MP4Integer8Property*)m_pProperties[0])->GetValue();
}

void MP4Atom::SetVersion(u_int8_t version) 
{
	if (strcmp("version", m_pProperties[0]->GetName())) {
		return;
	}
	((MP4Integer8Property*)m_pProperties[0])->SetValue(version);
}

u_int32_t MP4Atom::GetFlags()
{
	if (strcmp("flags", m_pProperties[1]->GetName())) {
		return 0;
	}
	return ((MP4Integer24Property*)m_pProperties[1])->GetValue();
}

void MP4Atom::SetFlags(u_int32_t flags) 
{
	if (strcmp("flags", m_pProperties[1]->GetName())) {
		return;
	}
	((MP4Integer24Property*)m_pProperties[1])->SetValue(flags);
}

void MP4Atom::Dump(FILE* pFile, u_int8_t indent, bool dumpImplicits)
{
	if (m_type[0] != '\0') {
		Indent(pFile, indent);
		fprintf(pFile, "type %s\n", m_type);
		fflush(pFile);
	}

	u_int32_t i;
	u_int32_t size;

	// dump our properties
	size = m_pProperties.Size();
	for (i = 0; i < size; i++) {

		/* skip details of tables unless we're told to be verbose */
		if (m_pProperties[i]->GetType() == TableProperty
		  && !(GetVerbosity() & MP4_DETAILS_TABLE)) {
			Indent(pFile, indent + 1);
			fprintf(pFile, "<table entries suppressed>\n");
			continue;
		}

		m_pProperties[i]->Dump(pFile, indent + 1, dumpImplicits);
	}

	// dump our children
	size = m_pChildAtoms.Size();
	for (i = 0; i < size; i++) {
		m_pChildAtoms[i]->Dump(pFile, indent + 1, dumpImplicits);
	}
}

u_int32_t MP4Atom::GetVerbosity() 
{
	ASSERT(m_pFile);
	return m_pFile->GetVerbosity();
}

u_int8_t MP4Atom::GetDepth()
{
	if (m_depth < 0xFF) {
		return m_depth;
	}

	MP4Atom *pAtom = this;
	m_depth = 0;

	while ((pAtom = pAtom->GetParentAtom()) != NULL) {
		m_depth++;
		ASSERT(m_depth < 255);
	}
	return m_depth;
}

