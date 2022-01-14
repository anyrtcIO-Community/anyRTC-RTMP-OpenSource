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
 * Contributer has declined to give copyright information, and gives
 * it freely to the world.
 */

#include "mp4common.h"

MP4TextAtom::MP4TextAtom() 
	: MP4Atom("text") 
{
	// The atom type "text" is used in two complete unrelated ways
	// i.e. it's real two atoms with the same name
	// To handle that we need to postpone property creation until
	// we know who our parent atom is (stsd or gmhd) which gives us
	// the context info we need to know who we are
}

void MP4TextAtom::AddPropertiesStsdType()
{

	AddReserved("reserved1", 6); /* 0 */

	AddProperty(new MP4Integer16Property("dataReferenceIndex"));/* 1 */

	AddProperty(new MP4Integer32Property("displayFlags")); /* 2 */
	AddProperty(new MP4Integer32Property("textJustification")); /* 3 */

	AddProperty(new MP4Integer16Property("bgColorRed")); /* 4 */
	AddProperty(new MP4Integer16Property("bgColorGreen")); /* 5 */
	AddProperty(new MP4Integer16Property("bgColorBlue")); /* 6 */

	AddProperty(new MP4Integer16Property("defTextBoxTop")); /* 7 */
	AddProperty(new MP4Integer16Property("defTextBoxLeft")); /* 8 */
	AddProperty(new MP4Integer16Property("defTextBoxBottom")); /* 9 */
	AddProperty(new MP4Integer16Property("defTextBoxRight")); /* 10 */

	AddReserved("reserved2", 8); /* 11 */

	AddProperty(new MP4Integer16Property("fontNumber")); /* 12 */
	AddProperty(new MP4Integer16Property("fontFace")); /* 13 */

	AddReserved("reserved3", 1); /* 14 */
	AddReserved("reserved4", 2); /* 15 */

	AddProperty(new MP4Integer16Property("foreColorRed")); /* 16 */
	AddProperty(new MP4Integer16Property("foreColorGreen")); /* 17 */
	AddProperty(new MP4Integer16Property("foreColorBlue")); /* 18 */

}

void MP4TextAtom::AddPropertiesGmhdType()
{

	AddProperty(new MP4BytesProperty("textData", 36)); /* 0 */

}


void MP4TextAtom::Generate()
{

	if (ATOMID(m_pParentAtom->GetType()) == ATOMID("stsd")) {
		AddPropertiesStsdType();
		GenerateStsdType();
	} else if (ATOMID(m_pParentAtom->GetType()) == ATOMID("gmhd")) {
		AddPropertiesGmhdType();
		GenerateGmhdType();
	} else {
		VERBOSE_WARNING(m_pFile->GetVerbosity(),
			printf("Warning: text atom in unexpected context, can not generate"));
	}

}

void MP4TextAtom::GenerateStsdType() 
{
	// generate children
	MP4Atom::Generate();

	((MP4Integer16Property*)m_pProperties[1])->SetValue(1);

	((MP4Integer32Property*)m_pProperties[2])->SetValue(1);
	((MP4Integer32Property*)m_pProperties[3])->SetValue(1);

}

void MP4TextAtom::GenerateGmhdType() 
{
	MP4Atom::Generate();

	// property 0 has non-zero fixed values
	static u_int8_t textData[36] = {
		0x00, 0x01, 
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00, 
		0x00, 0x00,
		0x00, 0x00, 
		0x00, 0x00,
		0x00, 0x01, 
		0x00, 0x00,
		0x00, 0x00, 
		0x00, 0x00,
		0x00, 0x00, 
		0x00, 0x00,
		0x00, 0x00, 
		0x00, 0x00,
		0x40, 0x00, 
		0x00, 0x00, 
	};
	((MP4BytesProperty*)m_pProperties[0])->SetValue(textData, sizeof(textData));
	
}

void MP4TextAtom::Read ()
{
  if (ATOMID(m_pParentAtom->GetType()) == ATOMID("stsd")) {
    AddPropertiesStsdType();
  } else if (ATOMID(m_pParentAtom->GetType()) == ATOMID("gmhd")) {
    AddPropertiesGmhdType();
  }
   
  MP4Atom::Read();
}

