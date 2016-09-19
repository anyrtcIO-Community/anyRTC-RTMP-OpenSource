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
 *		Dave Mackie		dmackie@cisco.com
 */

#ifndef __DESCRIPTORS_INCLUDED__
#define __DESCRIPTORS_INCLUDED__

const u_int8_t MP4ODescrTag					= 0x01; 
const u_int8_t MP4IODescrTag				= 0x02; 
const u_int8_t MP4ESDescrTag				= 0x03; 
const u_int8_t MP4DecConfigDescrTag			= 0x04; 
const u_int8_t MP4DecSpecificDescrTag		= 0x05; 
const u_int8_t MP4SLConfigDescrTag		 	= 0x06; 
const u_int8_t MP4ContentIdDescrTag		 	= 0x07; 
const u_int8_t MP4SupplContentIdDescrTag 	= 0x08; 
const u_int8_t MP4IPIPtrDescrTag		 	= 0x09; 
const u_int8_t MP4IPMPPtrDescrTag		 	= 0x0A; 
const u_int8_t MP4IPMPDescrTag			 	= 0x0B; 
const u_int8_t MP4RegistrationDescrTag	 	= 0x0D; 
const u_int8_t MP4ESIDIncDescrTag			= 0x0E; 
const u_int8_t MP4ESIDRefDescrTag			= 0x0F; 
const u_int8_t MP4FileIODescrTag			= 0x10; 
const u_int8_t MP4FileODescrTag				= 0x11; 
const u_int8_t MP4ExtProfileLevelDescrTag 	= 0x13; 
const u_int8_t MP4ExtDescrTagsStart			= 0x80; 
const u_int8_t MP4ExtDescrTagsEnd			= 0xFE; 

class MP4BaseDescriptor : public MP4Descriptor {
 public:
  MP4BaseDescriptor(u_int8_t tag);
};

class MP4BytesDescriptor : public MP4Descriptor {
 public:
  MP4BytesDescriptor(u_int8_t tag);
  void Read(MP4File* pFile);
 protected:
  uint m_size_offset; // size to adjust the size for the bytes property
  uint m_bytes_index; // index into properties for bytes property
};

class MP4IODescriptor : public MP4Descriptor {
public:
	MP4IODescriptor();
	void Generate();
protected:
	void Mutate();
};

class MP4ODescriptor : public MP4Descriptor {
public:
	MP4ODescriptor();
	void Generate();
protected:
	void Mutate();
};


class MP4ESDescriptor : public MP4Descriptor {
public:
	MP4ESDescriptor();
protected:
	void Mutate();
};

class MP4DecConfigDescriptor : public MP4Descriptor {
public:
	MP4DecConfigDescriptor();
	void Generate();
};


class MP4SLConfigDescriptor : public MP4Descriptor {
public:
	MP4SLConfigDescriptor();
	void Generate();
	void Read(MP4File* pFile);
 protected:
	void Mutate();
};

class MP4IPIPtrDescriptor : public MP4Descriptor {
public:
	MP4IPIPtrDescriptor();
};

class MP4ContentIdDescriptor : public MP4Descriptor {
public:
	MP4ContentIdDescriptor();
	void Read(MP4File* pFile);
protected:
	void Mutate();
};

// associated values in descriptors

// ES objectTypeId
const u_int8_t MP4SystemsV1ObjectType			= 0x01; 
const u_int8_t MP4SystemsV2ObjectType			= 0x02; 

// ES streamType
const u_int8_t MP4ObjectDescriptionStreamType	= 0x01; 
const u_int8_t MP4ClockReferenceStreamType		= 0x02; 
const u_int8_t MP4SceneDescriptionStreamType	= 0x03; 
const u_int8_t MP4VisualStreamType				= 0x04; 
const u_int8_t MP4AudioStreamType				= 0x05; 
const u_int8_t MP4Mpeg7StreamType				= 0x06; 
const u_int8_t MP4IPMPStreamType				= 0x07; 
const u_int8_t MP4OCIStreamType					= 0x08; 
const u_int8_t MP4MPEGJStreamType				= 0x09; 
const u_int8_t MP4UserPrivateStreamType			= 0x20; 

#endif /* __DESCRIPTORS_INCLUDED__ */

