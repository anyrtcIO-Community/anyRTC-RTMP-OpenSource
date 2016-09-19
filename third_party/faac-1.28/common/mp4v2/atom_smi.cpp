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
 *       Bill May  wmay@cisco.com
 *
 * Apple iTunes META data
 */

#include "mp4common.h"

MP4SmiAtom::MP4SmiAtom()
    : MP4Atom("meta")
{

  AddProperty( new MP4BytesProperty("metadata"));

}

void MP4SmiAtom::Read() 
{
	// calculate size of the metadata from the atom size
	((MP4BytesProperty*)m_pProperties[0])->SetValueSize(m_size);

	MP4Atom::Read();
}

