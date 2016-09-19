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
 * Copyright (C) Cisco Systems Inc. 2001 - 2005.  All Rights Reserved.
 *
 * 3GPP features implementation is based on 3GPP's TS26.234-v5.60,
 * and was contributed by Ximpo Group Ltd.
 *
 * Portions created by Ximpo Group Ltd. are
 * Copyright (C) Ximpo Group Ltd. 2003, 2004.  All Rights Reserved.
 *
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 *              Ximpo Group Ltd.                mp4v2@ximpo.com
 *              Bill May                wmay@cisco.com
 */

#ifndef __MP4_ATOMS_INCLUDED__
#define __MP4_ATOMS_INCLUDED__

// declare all the atom subclasses
// i.e. spare us atom_xxxx.h for all the atoms
//
// The majority of atoms just need their own constructor declared
// Some atoms have a few special needs
// A small minority of atoms need lots of special handling

class MP4RootAtom : public MP4Atom {
public:
	MP4RootAtom();
	void BeginWrite(bool use64 = false);
	void Write();
	void FinishWrite(bool use64 = false);

	void BeginOptimalWrite();
	void FinishOptimalWrite();

protected:
	u_int32_t GetLastMdatIndex();
	void WriteAtomType(const char* type, bool onlyOne);
};

/***********************************************************************
 * Common atom classes - standard for anything that just contains atoms
 * and non-maleable properties, treftype and url
 ***********************************************************************/
class MP4StandardAtom : public MP4Atom {
 public:
  MP4StandardAtom(const char *name);
};

class MP4TrefTypeAtom : public MP4Atom {
public:
	MP4TrefTypeAtom(const char* type);
	void Read();
};

class MP4UrlAtom : public MP4Atom {
public:
	MP4UrlAtom(const char *type="url ");
	void Read();
	void Write();
};

/***********************************************************************
 * Sound and Video atoms - use the generic atoms when possible
 * (MP4SoundAtom and MP4VideoAtom)
 ***********************************************************************/
class MP4SoundAtom : public MP4Atom {
 public:
  MP4SoundAtom(const char *atomid);
  void Generate();
  void Read();
protected:
  void AddProperties(u_int8_t version);
};

class MP4VideoAtom : public MP4Atom {
 public:
  MP4VideoAtom(const char *atomid);
  void Generate();
};

class MP4AmrAtom : public MP4Atom {
 public:
  MP4AmrAtom(const char *type);
  void Generate();
};

// H.264 atoms

class MP4Avc1Atom : public MP4Atom {
 public:
  MP4Avc1Atom();
  void Generate();
};

class MP4AvcCAtom : public MP4Atom {
 public:
  MP4AvcCAtom();
  void Generate();
  void Clone(MP4AvcCAtom *dstAtom);
};


class MP4D263Atom : public MP4Atom {
 public:
  MP4D263Atom();
  void Generate();
  void Write();
};
 
class MP4DamrAtom : public MP4Atom {
 public: 
  MP4DamrAtom();
  void Generate();
};

class MP4EncaAtom : public MP4Atom {
public:
        MP4EncaAtom();
        void Generate();
};

class MP4EncvAtom : public MP4Atom {
public:
        MP4EncvAtom();
        void Generate();
};

class MP4Mp4aAtom : public MP4Atom {
public:
	MP4Mp4aAtom();
	void Generate();
};

class MP4Mp4sAtom : public MP4Atom {
public:
	MP4Mp4sAtom();
	void Generate();
};

class MP4Mp4vAtom : public MP4Atom {
public:
	MP4Mp4vAtom();
	void Generate();
};


class MP4S263Atom : public MP4Atom {
 public:
  MP4S263Atom();
  void Generate();
};
 
 
 
/************************************************************************
 * Specialized Atoms
 ************************************************************************/

class MP4DataAtom : public MP4Atom {
public:
    MP4DataAtom();
    void Read();
};

class MP4DrefAtom : public MP4Atom {
public:
	MP4DrefAtom();
	void Read();
};

class MP4ElstAtom : public MP4Atom {
public:
	MP4ElstAtom();
	void Generate();
	void Read();
protected:
	void AddProperties(u_int8_t version);
};

class MP4FreeAtom : public MP4Atom {
public:
	MP4FreeAtom();
	void Read();
	void Write();
};

class MP4FtypAtom : public MP4Atom {
public:
	MP4FtypAtom();
	void Generate();
	void Read();
};

class MP4GminAtom : public MP4Atom {
public:
	MP4GminAtom();
	void Generate();
};

class MP4HdlrAtom : public MP4Atom {
public:
	MP4HdlrAtom();
	void Read();
};

class MP4HinfAtom : public MP4Atom {
public:
	MP4HinfAtom();
	void Generate();
};

class MP4HntiAtom : public MP4Atom {
public:
	MP4HntiAtom();
	void Read();
};


class MP4MdatAtom : public MP4Atom {
public:
	MP4MdatAtom();
	void Read();
	void Write();
};

class MP4MdhdAtom : public MP4Atom {
public:
	MP4MdhdAtom();
	void Generate();
	void Read();
protected:
	void AddProperties(u_int8_t version);
};

class MP4Meta1Atom : public MP4Atom {
 public:
  MP4Meta1Atom(const char *name);
  void Read();
};

class MP4Meta2Atom : public MP4Atom {
 public:
  MP4Meta2Atom(const char *name);
  void Read();
};
	       
class MP4MvhdAtom : public MP4Atom {
public:
	MP4MvhdAtom();
	void Generate();
	void Read();
protected:
	void AddProperties(u_int8_t version);
};

class MP4OhdrAtom : public MP4Atom {
 public:
  MP4OhdrAtom();
  ~MP4OhdrAtom();
  void Read();
};

class MP4RtpAtom : public MP4Atom {
public:
	MP4RtpAtom();
	void Generate();
	void Read();
	void Write();

protected:
	void AddPropertiesStsdType();
	void AddPropertiesHntiType();

	void GenerateStsdType();
	void GenerateHntiType();

	void ReadStsdType();
	void ReadHntiType();

	void WriteHntiType();
};

class MP4SdpAtom : public MP4Atom {
public:
	MP4SdpAtom();
	void Read();
	void Write();
};

class MP4SmiAtom : public MP4Atom {
 public:
  MP4SmiAtom(void);
  void Read();
};

class MP4StblAtom : public MP4Atom {
public:
	MP4StblAtom();
	void Generate();
};

class MP4StdpAtom : public MP4Atom {
public:
	MP4StdpAtom();
	void Read();
};

class MP4StscAtom : public MP4Atom {
public:
	MP4StscAtom();
	void Read();
};

class MP4StsdAtom : public MP4Atom {
public:
	MP4StsdAtom();
	void Read();
};

class MP4StszAtom : public MP4Atom {
public:
	MP4StszAtom();
	void Read();
	void Write();
};

class MP4Stz2Atom : public MP4Atom {
public:
	MP4Stz2Atom();
	void Read();
};

class MP4TextAtom : public MP4Atom {
public:
	MP4TextAtom();
	void Generate();
	void Read();
protected:
	void AddPropertiesStsdType();
	void AddPropertiesGmhdType();

	void GenerateStsdType();
	void GenerateGmhdType();
};

class MP4TfhdAtom : public MP4Atom {
public:
	MP4TfhdAtom();
	void Read();
protected:
	void AddProperties(u_int32_t flags);
};

class MP4TkhdAtom : public MP4Atom {
public:
	MP4TkhdAtom();
	void Generate();
	void Read();
protected:
	void AddProperties(u_int8_t version);
};
class MP4TrunAtom : public MP4Atom {
public:
	MP4TrunAtom();
	void Read();
protected:
	void AddProperties(u_int32_t flags);
};

class MP4UdtaAtom : public MP4Atom {
public:
	MP4UdtaAtom();
	void Read();
};

class MP4UrnAtom : public MP4Atom {
public:
	MP4UrnAtom();
	void Read();
};
class MP4VmhdAtom : public MP4Atom {
public:
	MP4VmhdAtom();
	void Generate();
};

class MP4HrefAtom : public MP4Atom {
 public:
  MP4HrefAtom();
  void Generate(void);
};

#endif /* __MP4_ATOMS_INCLUDED__ */
