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
 *		Bill May		wmay@cisco.com
 */

#include "mp4common.h"

MP4SoundAtom::MP4SoundAtom(const char *atomid) 
	: MP4Atom(atomid) 
{
	AddReserved("reserved1", 6); /* 0 */

	AddProperty( /* 1 */
		new MP4Integer16Property("dataReferenceIndex"));
	AddProperty( /* 2 */
		    new MP4Integer16Property("soundVersion"));
	AddReserved( "reserved2", 6); /* 3 */
	
	AddProperty( /* 4 */
		    new MP4Integer16Property("channels"));
	AddProperty( /* 5 */
		    new MP4Integer16Property("sampleSize"));
	AddProperty( /* 6 */
		    new MP4Integer16Property("packetSize"));
	AddProperty( /* 7 */
		    new MP4Integer32Property("timeScale"));

	if (ATOMID(atomid) == ATOMID("mp4a")) {
	  AddReserved("reserved3", 2); /* 8 */
	  ExpectChildAtom("esds", Required, OnlyOne);
	  ExpectChildAtom("wave", Optional, OnlyOne);
	} else if (ATOMID(atomid) == ATOMID("alac")) {
	  AddReserved("reserved3", 2); /* 8 */
	  ExpectChildAtom("alac", Optional, Optional);
	  //AddProperty( new MP4BytesProperty("alacInfo", 36));
	}
}

void MP4SoundAtom::AddProperties (uint8_t version)
{
  if (version > 0) {
    AddProperty( /* 8 */
		new MP4Integer32Property("samplesPerPacket"));
    AddProperty( /* 9 */
		new MP4Integer32Property("bytesPerPacket"));
    AddProperty( /* 10 */
		new MP4Integer32Property("bytesPerFrame"));
    AddProperty( /* 11 */
		new MP4Integer32Property("bytesPerSample"));
  }
  if (version == 2) {
    AddReserved("reserved4", 20);
  }
}
void MP4SoundAtom::Generate()
{
	MP4Atom::Generate();

	((MP4Integer16Property*)m_pProperties[1])->SetValue(1);

	// property reserved2 has non-zero fixed values
	((MP4Integer16Property*)m_pProperties[2])->SetValue(0);
	static const u_int8_t reserved2[6] = {
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 
	};
	m_pProperties[3]->SetReadOnly(false);
	((MP4BytesProperty*)m_pProperties[3])->
		SetValue(reserved2, sizeof(reserved2));
	m_pProperties[3]->SetReadOnly(true);
	((MP4Integer16Property*)m_pProperties[4])->SetValue(2);
	((MP4Integer16Property*)m_pProperties[5])->SetValue(0x0010);
	((MP4Integer16Property*)m_pProperties[6])->SetValue(0);

}

void MP4SoundAtom::Read()
{
  MP4Atom *parent = GetParentAtom();
  if (ATOMID(parent->GetType()) != ATOMID("stsd")) {
    // Quicktime has an interesting thing - they'll put an mp4a atom
    // which is blank inside a wave atom, which is inside an mp4a atom
    // we have a mp4a inside an wave inside an mp4a - delete all properties
    m_pProperties.Delete(8);
    m_pProperties.Delete(7);
    m_pProperties.Delete(6);
    m_pProperties.Delete(5);
    m_pProperties.Delete(4);
    m_pProperties.Delete(3);
    m_pProperties.Delete(2);
    m_pProperties.Delete(1);
    m_pProperties.Delete(0);
    if (ATOMID(GetType()) == ATOMID("alac")) {
      AddProperty(new MP4BytesProperty("decoderConfig", m_size));
      ReadProperties();
    }
    if (m_pChildAtomInfos.Size() > 0) {
      ReadChildAtoms();
    }
  } else {
    ReadProperties(0, 3); // read first 3 properties
    AddProperties(((MP4IntegerProperty *)m_pProperties[2])->GetValue());
    ReadProperties(3); // continue
    if (m_pChildAtomInfos.Size() > 0) {
      ReadChildAtoms();
    }
  }
  Skip();
}
