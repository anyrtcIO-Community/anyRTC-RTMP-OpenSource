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

#define _3GP_MAJOR_BRAND "3gp5"
#define _3GP_MINOR_VERSION 0x0001

void MP4File::Make3GPCompliant(const char* fileName,  char* majorBrand, u_int32_t minorVersion, char** supportedBrands, u_int32_t supportedBrandsCount, bool deleteIodsAtom)
{
	char brand[5] = "3gp5";
	char* _3gpSupportedBrands[1] = { (char*)&brand };
	
	if (majorBrand) {
		if (!supportedBrands || !supportedBrandsCount) {
			throw new MP4Error("Invalid parameters", "MP4File::Make3GPCompliant");
		}
	}

	MakeFtypAtom(
			majorBrand ? majorBrand : (char*)brand,
			majorBrand ? minorVersion  : _3GP_MINOR_VERSION,
			majorBrand ? supportedBrands : (char**)_3gpSupportedBrands,
			majorBrand ? supportedBrandsCount : 1);

	if (deleteIodsAtom) {
		// Delete the iods atom, if it exists....
		MP4Atom* iodsAtom = m_pRootAtom->FindAtom("moov.iods");
		if (iodsAtom) {
			MP4Atom* moovAtom = m_pRootAtom->FindAtom("moov");
			ASSERT(moovAtom);

			moovAtom->DeleteChildAtom(iodsAtom);
		}
	}

}

void MP4File::MakeFtypAtom(char* majorBrand, u_int32_t minorVersion, char** supportedBrands, u_int32_t supportedBrandsCount)
{
	bool rewriteNeeded = false;
	u_int32_t currentSupportedBrandsCount;
	u_int32_t i;

		
	MP4Atom* ftypAtom = m_pRootAtom->FindAtom("ftyp");
	if (ftypAtom == NULL) {
	  ftypAtom = InsertChildAtom(m_pRootAtom, "ftyp", 0);
	}
	if (majorBrand == NULL)
	  return;
	MP4StringProperty* pMajorBrandProperty;
	if (!ftypAtom->FindProperty(
				   "ftyp.majorBrand",
				   (MP4Property**)&pMajorBrandProperty))
	  return;

	pMajorBrandProperty->SetValue(majorBrand);


	MP4Integer32Property* pMinorVersionProperty;
	if (!ftypAtom->FindProperty(
				   "ftype.minorVersion",
				   (MP4Property**)&pMinorVersionProperty))
	  return;

	pMinorVersionProperty->SetValue(minorVersion);

	MP4Integer32Property* pCompatibleBrandsCountProperty;
	if (!ftypAtom->FindProperty(
		"ftyp.compatibleBrandsCount",
		(MP4Property**)&pCompatibleBrandsCountProperty)) return;

	currentSupportedBrandsCount = pCompatibleBrandsCountProperty->GetValue();

	MP4TableProperty* pCompatibleBrandsProperty;
	if (!ftypAtom->FindProperty(
		"ftyp.compatibleBrands",
		(MP4Property**)&pCompatibleBrandsProperty)) return;

	MP4StringProperty* pBrandProperty = (MP4StringProperty*)
		pCompatibleBrandsProperty->GetProperty(0);
	ASSERT(pBrandProperty);

	for (i = 0 ; i < ((currentSupportedBrandsCount > supportedBrandsCount) ? supportedBrandsCount : currentSupportedBrandsCount) ; i++) {
		pBrandProperty->SetValue(supportedBrands[i], i);

	}

	if (i < supportedBrandsCount) {
		for ( ; i < supportedBrandsCount ; i++) {
			pBrandProperty->AddValue(supportedBrands[i]);
		}
	}

	if (currentSupportedBrandsCount != supportedBrandsCount) {
		rewriteNeeded = true;
		pBrandProperty->SetCount(supportedBrandsCount);
		pCompatibleBrandsCountProperty->SetReadOnly(false);
		pCompatibleBrandsCountProperty->SetValue(supportedBrandsCount);
		pCompatibleBrandsCountProperty->SetReadOnly(true);
	}

}
