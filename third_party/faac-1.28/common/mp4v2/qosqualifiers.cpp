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
#if 1
MP4QosDescriptorBase::MP4QosDescriptorBase (u_int8_t tag)
  : MP4Descriptor(tag)
{
  switch (tag) {
  case MP4QosDescrTag:
	AddProperty( /* 0 */
		new MP4Integer8Property("predefined"));
	AddProperty( /* 1 */
		new MP4QosQualifierProperty("qualifiers",
					    MP4QosTagsStart, 
					    MP4QosTagsEnd, Optional, Many));
	break;
  case MP4MaxDelayQosTag:
	AddProperty( /* 0 */
		new MP4Integer32Property("maxDelay"));
	break;
  case MP4PrefMaxDelayQosTag:
	AddProperty( /* 0 */
		new MP4Integer32Property("prefMaxDelay"));
	break;
  case MP4LossProbQosTag:
	AddProperty( /* 0 */
		new MP4Float32Property("lossProb"));
	break;
  case MP4MaxGapLossQosTag:
	AddProperty( /* 0 */
		new MP4Integer32Property("maxGapLoss"));
	break;
  case MP4MaxAUSizeQosTag:
	AddProperty( /* 0 */
		new MP4Integer32Property("maxAUSize"));
	break;
  case MP4AvgAUSizeQosTag:
	AddProperty( /* 0 */
		    new MP4Integer32Property("avgAUSize"));
	break;
  case MP4MaxAURateQosTag:
	AddProperty( /* 0 */
		new MP4Integer32Property("maxAURate"));
	break;
  }
}

#else
MP4QosDescriptor::MP4QosDescriptor()
	: MP4Descriptor(MP4QosDescrTag)
{
	AddProperty( /* 0 */
		new MP4Integer8Property("predefined"));
	AddProperty( /* 1 */
		new MP4QosQualifierProperty("qualifiers",
			MP4QosTagsStart, MP4QosTagsEnd, Optional, Many));
}

MP4MaxDelayQosQualifier::MP4MaxDelayQosQualifier()
	: MP4QosQualifier(MP4MaxDelayQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("maxDelay"));
}

MP4PrefMaxDelayQosQualifier::MP4PrefMaxDelayQosQualifier()
	: MP4QosQualifier(MP4PrefMaxDelayQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("prefMaxDelay"));
}

MP4LossProbQosQualifier::MP4LossProbQosQualifier()
	: MP4QosQualifier(MP4LossProbQosTag)
{
	AddProperty( /* 0 */
		new MP4Float32Property("lossProb"));
}

MP4MaxGapLossQosQualifier::MP4MaxGapLossQosQualifier()
	: MP4QosQualifier(MP4MaxGapLossQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("maxGapLoss"));
}

MP4MaxAUSizeQosQualifier::MP4MaxAUSizeQosQualifier()
	: MP4QosQualifier(MP4MaxAUSizeQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("maxAUSize"));
}

MP4AvgAUSizeQosQualifier::MP4AvgAUSizeQosQualifier()
	: MP4QosQualifier(MP4AvgAUSizeQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("avgAUSize"));
}

MP4MaxAURateQosQualifier::MP4MaxAURateQosQualifier()
	: MP4QosQualifier(MP4MaxAURateQosTag)
{
	AddProperty( /* 0 */
		new MP4Integer32Property("maxAURate"));
}
#endif
MP4UnknownQosQualifier::MP4UnknownQosQualifier()
	: MP4Descriptor()
{
	AddProperty( /* 0 */
		new MP4BytesProperty("data"));
}

void MP4UnknownQosQualifier::Read(MP4File* pFile)
{
	ReadHeader(pFile);

	/* byte properties need to know how long they are before reading */
	((MP4BytesProperty*)m_pProperties[0])->SetValueSize(m_size);

	ReadProperties(pFile);
}

MP4Descriptor* MP4QosQualifierProperty::CreateDescriptor(u_int8_t tag) 
{
	MP4Descriptor* pDescriptor = NULL;
	switch (tag) {
#if 0
	case MP4MaxDelayQosTag:
		pDescriptor = new MP4MaxDelayQosQualifier();
		break;
	case MP4PrefMaxDelayQosTag:
		pDescriptor = new MP4PrefMaxDelayQosQualifier();
		break;
	case MP4LossProbQosTag:
		pDescriptor = new MP4LossProbQosQualifier();
		break;
	case MP4MaxGapLossQosTag:
		pDescriptor = new MP4MaxGapLossQosQualifier();
		break;
	case MP4MaxAUSizeQosTag:
		pDescriptor = new MP4MaxAUSizeQosQualifier();
		break;
	case MP4AvgAUSizeQosTag:
		pDescriptor = new MP4AvgAUSizeQosQualifier();
		break;
	case MP4MaxAURateQosTag:
		pDescriptor = new MP4MaxAURateQosQualifier();
		break;
#else
	case MP4MaxDelayQosTag:
	case MP4PrefMaxDelayQosTag:
	case MP4LossProbQosTag:
	case MP4MaxGapLossQosTag:
	case MP4MaxAUSizeQosTag:
	case MP4AvgAUSizeQosTag:
	case MP4MaxAURateQosTag:
	  pDescriptor = new MP4QosDescriptorBase(tag);
	  break;
#endif
	default:
		pDescriptor = new MP4UnknownQosQualifier();
		pDescriptor->SetTag(tag);
	}
	
	return pDescriptor;
}

