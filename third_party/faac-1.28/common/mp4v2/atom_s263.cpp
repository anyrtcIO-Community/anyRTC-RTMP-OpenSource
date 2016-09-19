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
 * 3GPP features implementation is based on 3GPP's TS26.234-v5.60,
 * and was contributed by Ximpo Group Ltd.
 *
 * Portions created by Ximpo Group Ltd. are
 * Copyright (C) Ximpo Group Ltd. 2003, 2004.  All Rights Reserved.
 *
 * Contributor(s):
 *              Ximpo Group Ltd.          mp4v2@ximpo.com
 */

#include "mp4common.h"

MP4S263Atom::MP4S263Atom() 
	: MP4Atom("s263") 
{
	AddReserved("reserved1", 6); /* 0 */

	AddProperty( /* 1 */
		new MP4Integer16Property("dataReferenceIndex"));

	AddReserved("reserved2", 16); /* 2 */

	AddProperty( /* 3 */
		new MP4Integer16Property("width"));

	AddProperty( /* 4 */
		new MP4Integer16Property("height"));

	AddReserved("reserved3", 50); /* 5 */


	ExpectChildAtom("d263", Required, OnlyOne);
}

void MP4S263Atom::Generate()
{
	MP4Atom::Generate();

	((MP4Integer16Property*)m_pProperties[1])->SetValue(1);

	// property reserved2 has non-zero fixed values
	static u_int8_t reserved3[50] = {
		0x00, 0x48, 0x00, 0x00, 
		0x00, 0x48, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x24, 
		0xFF, 0xFF
	};
	m_pProperties[5]->SetReadOnly(false);
	((MP4BytesProperty*)m_pProperties[5])->
		SetValue(reserved3, sizeof(reserved3));
	m_pProperties[5]->SetReadOnly(true);
}
