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

#include "mp4common.h"

MP4TkhdAtom::MP4TkhdAtom() 
	: MP4Atom("tkhd")
{
	AddVersionAndFlags();
}

void MP4TkhdAtom::AddProperties(u_int8_t version) 
{
	if (version == 1) {
		AddProperty( /* 2 */
			new MP4Integer64Property("creationTime"));
		AddProperty( /* 3 */
			new MP4Integer64Property("modificationTime"));
	} else { // version == 0
		AddProperty( /* 2 */
			new MP4Integer32Property("creationTime"));
		AddProperty( /* 3 */
			new MP4Integer32Property("modificationTime"));
	}

	AddProperty( /* 4 */
		new MP4Integer32Property("trackId"));
	AddReserved("reserved1", 4); /* 5 */

	if (version == 1) {
		AddProperty( /* 6 */
			new MP4Integer64Property("duration"));
	} else {
		AddProperty( /* 6 */
			new MP4Integer32Property("duration"));
	}

	AddReserved("reserved2", 12); /* 7 */

	MP4Float32Property* pProp;

	pProp = new MP4Float32Property("volume");
	pProp->SetFixed16Format();
	AddProperty(pProp); /* 8 */

	AddReserved("reserved3", 2); /* 9 */

	AddProperty(new MP4BytesProperty("matrix", 36)); /* 10 */

	pProp = new MP4Float32Property("width");
	pProp->SetFixed32Format();
	AddProperty(pProp); /* 11 */

	pProp = new MP4Float32Property("height");
	pProp->SetFixed32Format();
	AddProperty(pProp); /* 12 */
}

void MP4TkhdAtom::Generate() 
{
	u_int8_t version = m_pFile->Use64Bits(GetType()) ? 1 : 0;
	SetVersion(version);
	AddProperties(version);

	MP4Atom::Generate();

	// set creation and modification times
	MP4Timestamp now = MP4GetAbsTimestamp();
	if (version == 1) {
		((MP4Integer64Property*)m_pProperties[2])->SetValue(now);
		((MP4Integer64Property*)m_pProperties[3])->SetValue(now);
	} else {
		((MP4Integer32Property*)m_pProperties[2])->SetValue(now);
		((MP4Integer32Property*)m_pProperties[3])->SetValue(now);
	}

	// property "matrix" has non-zero fixed values
	// this default identity matrix indicates no transformation, i.e.
	// 1, 0, 0
	// 0, 1, 0
	// 0, 0, 1
	// see http://developer.apple.com/documentation/QuickTime/QTFF/QTFFChap4/chapter_5_section_4.html

	static u_int8_t matrix[36] = {
		0x00, 0x01, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x01, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x40, 0x00, 0x00, 0x00, 
	};

	((MP4BytesProperty*)m_pProperties[10])->
		SetValue(matrix, sizeof(matrix));
}

void MP4TkhdAtom::Read() 
{
	/* read atom version */
	ReadProperties(0, 1);

	/* need to create the properties based on the atom version */
	AddProperties(GetVersion());

	/* now we can read the remaining properties */
	ReadProperties(1);

	Skip();	// to end of atom
}
