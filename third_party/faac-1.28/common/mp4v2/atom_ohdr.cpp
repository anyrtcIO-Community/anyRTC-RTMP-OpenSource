/**	\file atom_ohdr.cpp

  	\author Danijel Kopcinovic (danijel.kopcinovic@adnecto.net)
*/

#include "mp4common.h"

/*! \brief Patch class for read/write operations when string is 0-length.

    We want to use string property, but mpeg4ip doesn't support ohdr way of
    encoding of string (in ohdr atom we first have 3 lengths of 3 strings and
    then their string values, and it cannot be simulated with any of the
    current mpeg4ip string property parameters), so we have to write our own
    Read() and Write() routines.
*/
class OhdrMP4StringProperty: public MP4StringProperty {
public:
  /*! \brief Constructor.
  
      \param name                name of the property.
      \param useCountedFormat    counted format flag.
      \param useUnicode          unicode flag.
  */
	OhdrMP4StringProperty(char* name, bool useCountedFormat = false,
    bool useUnicode = false): MP4StringProperty(name, useCountedFormat,
    useUnicode) {
  }
    
  /*! \brief Read property from file.
  
      \param pFile                input, file handle.
      \param index                input, index to read.
  */
	void Read(MP4File* pFile, u_int32_t index = 0) {
	  MP4Free(m_values[index]);
	  m_values[index] = (char*)MP4Calloc(m_fixedLength + 1);
	  (void)pFile->ReadBytes((u_int8_t*)m_values[index], m_fixedLength);
  }
	
  /*! \brief Write property to file.
  
      \param pFile                input, file handle.
      \param index                input, index to write.
  */
	void Write(MP4File* pFile, u_int32_t index = 0) {
    pFile->WriteBytes((u_int8_t*)m_values[index], m_fixedLength);
  }
};

/*! \brief OMA DRM headers atom.

    Contained in OMA DRM key management atom. It must contain content identifier.
*/
/*! \brief Constructor.
*/
MP4OhdrAtom::MP4OhdrAtom(): MP4Atom("ohdr") {
	AddVersionAndFlags();

	AddProperty(new MP4Integer8Property("EncryptionMethod"));
	AddProperty(new MP4Integer8Property("EncryptionPadding"));
	AddProperty(new MP4Integer64Property("PlaintextLength"));
	AddProperty(new MP4Integer16Property("ContentIDLength"));
	AddProperty(new MP4Integer16Property("RightsIssuerURLLength"));
	AddProperty(new MP4Integer16Property("TextualHeadersLength"));
	AddProperty(new OhdrMP4StringProperty("ContentID"));
	AddProperty(new OhdrMP4StringProperty("RightsIssuerURL"));
	AddProperty(new MP4BytesProperty("TextualHeaders"));
}

MP4OhdrAtom::~MP4OhdrAtom() {
}

/*! \brief Read atom.
*/
void MP4OhdrAtom::Read() {
  ReadProperties(0, 8);
  MP4Property* lProperty;
  MP4Property* property;
  lProperty = GetProperty(5);
  property = GetProperty(8);
  ((OhdrMP4StringProperty*)property)->SetFixedLength(
    ((MP4Integer16Property*)lProperty)->GetValue());
  lProperty = GetProperty(6);
  property = GetProperty(9);
  ((OhdrMP4StringProperty*)property)->SetFixedLength(
    ((MP4Integer16Property*)lProperty)->GetValue());
  lProperty = GetProperty(7);
  property = GetProperty(10);
  ((MP4BytesProperty*)property)->SetFixedSize(
    ((MP4Integer16Property*)lProperty)->GetValue());
  ReadProperties(8, 3);
}
