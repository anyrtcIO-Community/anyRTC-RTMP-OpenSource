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

#define AMR_VENDOR 0x6d346970

MP4DamrAtom::MP4DamrAtom() 
	: MP4Atom("damr") 
{
	AddProperty( /* 0 */
		new MP4Integer32Property("vendor"));

	AddProperty( /* 1 */
		new MP4Integer8Property("decoderVersion"));

	AddProperty( /* 2 */
		new MP4Integer16Property("modeSet"));

	AddProperty( /* 3 */
		new MP4Integer8Property("modeChangePeriod"));

	AddProperty( /* 4 */
		new MP4Integer8Property("framesPerSample"));

}

void MP4DamrAtom::Generate()
{
	MP4Atom::Generate();

       ((MP4Integer32Property*)m_pProperties[0])->SetValue(AMR_VENDOR);
       ((MP4Integer8Property*)m_pProperties[1])->SetValue(1);

}
