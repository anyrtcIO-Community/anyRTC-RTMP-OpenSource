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
 *		Dave Mackie		  dmackie@cisco.com
 *              Alix Marchandise-Franquet alix@cisco.com
 *              Ximpo Group Ltd.          mp4v2@ximpo.com
 *              Bill May                  wmay@cisco.com
 */

#include "mp4common.h"

MP4File::MP4File(u_int32_t verbosity)
{
	m_fileName = NULL;
	#ifdef _WIN32
	m_fileName_w = NULL;
	#endif
	m_pFile = NULL;
	m_virtual_IO = NULL;
	m_orgFileSize = 0;
	m_fileSize = 0;
	m_pRootAtom = NULL;
	m_odTrackId = MP4_INVALID_TRACK_ID;

	m_verbosity = verbosity;
	m_mode = 0;
	m_createFlags = 0;
	m_useIsma = false;

	m_pModificationProperty = NULL;
	m_pTimeScaleProperty = NULL;
	m_pDurationProperty = NULL;

	m_memoryBuffer = NULL;
	m_memoryBufferSize = 0;
	m_memoryBufferPosition = 0;

	m_numReadBits = 0;
	m_bufReadBits = 0;
	m_numWriteBits = 0;
	m_bufWriteBits = 0;
	m_editName = NULL;
#ifndef _WIN32
	m_tempFileName[0] = '\0';
#endif
	m_trakName[0] = '\0';
}

MP4File::~MP4File()
{
	MP4Free(m_fileName);
	#ifdef _WIN32
	MP4Free(m_fileName_w);
	#endif
	if (m_pFile != NULL) {
	  // not closed ?
	  m_virtual_IO->Close(m_pFile);
	  m_pFile = NULL;
	}
	delete m_pRootAtom;
	for (u_int32_t i = 0; i < m_pTracks.Size(); i++) {
		delete m_pTracks[i];
	}
	MP4Free(m_memoryBuffer);	// just in case
	CHECK_AND_FREE(m_editName);
	
}

void MP4File::Read(const char* fileName)
{
	m_fileName = MP4Stralloc(fileName);
	m_mode = 'r';

	Open("rb");

	ReadFromFile();

	CacheProperties();
}

#ifdef _WIN32
void MP4File::Read(const wchar_t* fileName)
{
	m_fileName_w = MP4Stralloc(fileName);
	m_mode = 'r';

	Open(L"rb");

	ReadFromFile();

	CacheProperties();
}
#endif

// benski>
void MP4File::ReadEx(const char *fileName, void *user, Virtual_IO *virtual_IO)
{
	m_fileName = MP4Stralloc(fileName);
	m_mode = 'r';

 	m_pFile = user;
	m_virtual_IO = virtual_IO;

	ASSERT(m_pFile);
	ASSERT(m_virtual_IO)
	
	m_orgFileSize = m_fileSize = m_virtual_IO->GetFileLength(m_pFile); 

	ReadFromFile();

	CacheProperties();
}


void MP4File::Create(const char* fileName, u_int32_t flags, 
		     int add_ftyp, int add_iods, 
		     char* majorBrand, u_int32_t minorVersion, 
		     char** supportedBrands, u_int32_t supportedBrandsCount)
{
	m_fileName = MP4Stralloc(fileName);
	m_mode = 'w';
	m_createFlags = flags;

	Open("wb+");

	// generate a skeletal atom tree
	m_pRootAtom = MP4Atom::CreateAtom(NULL);
	m_pRootAtom->SetFile(this);
	m_pRootAtom->Generate();

	if (add_ftyp != 0) {
	  MakeFtypAtom(majorBrand, minorVersion, 
		       supportedBrands, supportedBrandsCount);
	}

	CacheProperties();

	// create mdat, and insert it after ftyp, and before moov
	(void)InsertChildAtom(m_pRootAtom, "mdat", 
			      add_ftyp != 0 ? 1 : 0);

	// start writing
	m_pRootAtom->BeginWrite();
	if (add_iods != 0) {
	  (void)AddChildAtom("moov", "iods");
	}
}

bool MP4File::Use64Bits (const char *atomName)
{
  uint32_t atomid = ATOMID(atomName);
  if (atomid == ATOMID("mdat") || atomid == ATOMID("stbl")) {
    return (m_createFlags & MP4_CREATE_64BIT_DATA) == MP4_CREATE_64BIT_DATA;
  } 
  if (atomid == ATOMID("mvhd") ||
      atomid == ATOMID("tkhd") ||
      atomid == ATOMID("mdhd")) {
    return (m_createFlags & MP4_CREATE_64BIT_TIME) == MP4_CREATE_64BIT_TIME;
  }
  return false;
}

void MP4File::Check64BitStatus (const char *atomName)
{
  uint32_t atomid = ATOMID(atomName);

  if (atomid == ATOMID("mdat") || atomid == ATOMID("stbl")) {
    m_createFlags |= MP4_CREATE_64BIT_DATA;
  } else if (atomid == ATOMID("mvhd") ||
	     atomid == ATOMID("tkhd") ||
	     atomid == ATOMID("mdhd")) {
    m_createFlags |= MP4_CREATE_64BIT_TIME;
  }
}

    
bool MP4File::Modify(const char* fileName)
{
	m_fileName = MP4Stralloc(fileName);
	m_mode = 'r';

	Open("rb+");
	ReadFromFile();

	m_mode = 'w';

	// find the moov atom
	MP4Atom* pMoovAtom = m_pRootAtom->FindAtom("moov");
	u_int32_t numAtoms;

	if (pMoovAtom == NULL) {
		// there isn't one, odd but we can still proceed
	  return false;
	  //pMoovAtom = AddChildAtom(m_pRootAtom, "moov");
	} else {
		numAtoms = m_pRootAtom->GetNumberOfChildAtoms();

		// work backwards thru the top level atoms
		int32_t i;
		bool lastAtomIsMoov = true;
		MP4Atom* pLastAtom = NULL;

		for (i = numAtoms - 1; i >= 0; i--) {
			MP4Atom* pAtom = m_pRootAtom->GetChildAtom(i);
			const char* type = pAtom->GetType();
			
			// get rid of any trailing free or skips
			if (!strcmp(type, "free") || !strcmp(type, "skip")) {
				m_pRootAtom->DeleteChildAtom(pAtom);
				continue;
			}

			if (strcmp(type, "moov")) {
				if (pLastAtom == NULL) {
					pLastAtom = pAtom;
					lastAtomIsMoov = false;
				}
				continue;
			}

			// now at moov atom

			// multiple moov atoms?!?
			if (pAtom != pMoovAtom) {
				throw new MP4Error(
					"Badly formed mp4 file, multiple moov atoms", 
					"MP4Modify");
			}

			if (lastAtomIsMoov) {
				// position to start of moov atom,
				// effectively truncating file 
				// prior to adding new mdat
				SetPosition(pMoovAtom->GetStart());

			} else { // last atom isn't moov
				// need to place a free atom 
				MP4Atom* pFreeAtom = MP4Atom::CreateAtom("free");

				// in existing position of the moov atom
				m_pRootAtom->InsertChildAtom(pFreeAtom, i);
				m_pRootAtom->DeleteChildAtom(pMoovAtom);
				m_pRootAtom->AddChildAtom(pMoovAtom);

				// write free atom to disk
				SetPosition(pMoovAtom->GetStart());
				pFreeAtom->SetSize(pMoovAtom->GetSize());
				pFreeAtom->Write();

				// finally set our file position to the end of the last atom
				SetPosition(pLastAtom->GetEnd());
			}

			break;
		}
		ASSERT(i != -1);
	}

	CacheProperties();	// of moov atom

	numAtoms = m_pRootAtom->GetNumberOfChildAtoms();

	// insert another mdat prior to moov atom (the last atom)
	MP4Atom* pMdatAtom = InsertChildAtom(m_pRootAtom, "mdat", numAtoms - 1);

	// start writing new mdat
	pMdatAtom->BeginWrite(Use64Bits("mdat"));
	return true;
}

void MP4File::Optimize(const char* orgFileName, const char* newFileName)
{
	m_fileName = MP4Stralloc(orgFileName);
	m_mode = 'r';

	// first load meta-info into memory
	Open("rb");
	ReadFromFile();

	CacheProperties();	// of moov atom

	// now switch over to writing the new file
	MP4Free(m_fileName);
	#ifdef _WIN32
	MP4Free(m_fileName_w);
	#endif

	// create a temporary file if necessary
	if (newFileName == NULL) {
		m_fileName = MP4Stralloc(TempFileName());
	} else {
		m_fileName = MP4Stralloc(newFileName);
	}

	void* pReadFile = m_pFile;
	Virtual_IO *pReadIO = m_virtual_IO;
	m_pFile = NULL;
	m_mode = 'w';

	Open("wb");

	SetIntegerProperty("moov.mvhd.modificationTime", 
		MP4GetAbsTimestamp());

	// writing meta info in the optimal order
	((MP4RootAtom*)m_pRootAtom)->BeginOptimalWrite();

	// write data in optimal order
	RewriteMdat(pReadFile, m_pFile, pReadIO, m_virtual_IO);

	// finish writing
	((MP4RootAtom*)m_pRootAtom)->FinishOptimalWrite();

	// cleanup
	m_virtual_IO->Close(m_pFile);
	m_pFile = NULL;
	pReadIO->Close(pReadFile);

	// move temporary file into place
	if (newFileName == NULL) {
		Rename(m_fileName, orgFileName);
	}
}

void MP4File::RewriteMdat(void* pReadFile, void* pWriteFile,
			  Virtual_IO *readIO, Virtual_IO *writeIO)
{
	u_int32_t numTracks = m_pTracks.Size();

	MP4ChunkId* chunkIds = new MP4ChunkId[numTracks];
	MP4ChunkId* maxChunkIds = new MP4ChunkId[numTracks];
	MP4Timestamp* nextChunkTimes = new MP4Timestamp[numTracks];

	for (u_int32_t i = 0; i < numTracks; i++) {
		chunkIds[i] = 1;
		maxChunkIds[i] = m_pTracks[i]->GetNumberOfChunks();
		nextChunkTimes[i] = MP4_INVALID_TIMESTAMP;
	}

	while (true) {
		u_int32_t nextTrackIndex = (u_int32_t)-1;
		MP4Timestamp nextTime = MP4_INVALID_TIMESTAMP;

		for (u_int32_t i = 0; i < numTracks; i++) {
			if (chunkIds[i] > maxChunkIds[i]) {
				continue;
			}

			if (nextChunkTimes[i] == MP4_INVALID_TIMESTAMP) {
				MP4Timestamp chunkTime =
					m_pTracks[i]->GetChunkTime(chunkIds[i]);

				nextChunkTimes[i] = MP4ConvertTime(chunkTime,
					m_pTracks[i]->GetTimeScale(), GetTimeScale());
			}

			// time is not earliest so far
			if (nextChunkTimes[i] > nextTime) {
				continue;
			}

			// prefer hint tracks to media tracks if times are equal
			if (nextChunkTimes[i] == nextTime 
			  && strcmp(m_pTracks[i]->GetType(), MP4_HINT_TRACK_TYPE)) {
				continue;
			}

			// this is our current choice of tracks
			nextTime = nextChunkTimes[i];
			nextTrackIndex = i;
		}

		if (nextTrackIndex == (u_int32_t)-1) {
			break;
		}

		// point into original mp4 file for read chunk call
		m_pFile = pReadFile;
		m_virtual_IO = readIO;
		m_mode = 'r';

		u_int8_t* pChunk;
		u_int32_t chunkSize;

		m_pTracks[nextTrackIndex]->
			ReadChunk(chunkIds[nextTrackIndex], &pChunk, &chunkSize);

		// point back at the new mp4 file for write chunk
		m_pFile = pWriteFile;
		m_virtual_IO = writeIO;
		m_mode = 'w';

		m_pTracks[nextTrackIndex]->
			RewriteChunk(chunkIds[nextTrackIndex], pChunk, chunkSize);

		MP4Free(pChunk);

		chunkIds[nextTrackIndex]++;
		nextChunkTimes[nextTrackIndex] = MP4_INVALID_TIMESTAMP;
	}

	delete [] chunkIds;
	delete [] maxChunkIds;
	delete [] nextChunkTimes;
}

void MP4File::Open(const char* fmode)
{
	ASSERT(m_pFile == NULL);
	FILE *openFile = NULL;

#ifdef O_LARGEFILE
	// UGH! fopen doesn't open a file in 64-bit mode, period.
	// So we need to use open() and then fdopen()
	int fd;
	int flags = O_LARGEFILE;

	if (strchr(fmode, '+')) {
		flags |= O_CREAT | O_RDWR;
		if (fmode[0] == 'w') {
			flags |= O_TRUNC;
		}
	} else {
		if (fmode[0] == 'w') {
			flags |= O_CREAT | O_TRUNC | O_WRONLY;
		} else {
			flags |= O_RDONLY;
		}
	}
	fd = open(m_fileName, flags, 0666); 

	if (fd >= 0) {
		openFile = fdopen(fd, fmode);
	}
#else
	openFile = fopen(m_fileName, fmode);
#endif
	m_pFile = openFile;

	if (m_pFile == NULL) {
		throw new MP4Error(errno, "failed", "MP4Open");
	}
	
	m_virtual_IO = &FILE_virtual_IO;
	if (m_mode == 'r') {
	  m_orgFileSize = m_fileSize = m_virtual_IO->GetFileLength(m_pFile); // benski
	} else {
		m_orgFileSize = m_fileSize = 0;
	}
}

#ifdef _WIN32
void MP4File::Open(const wchar_t* fmode)
{
	ASSERT(m_pFile == NULL);
	FILE *openFile = NULL;
#ifdef O_LARGEFILE
	// UGH! fopen doesn't open a file in 64-bit mode, period.
	// So we need to use open() and then fdopen()
	int fd;
	int flags = O_LARGEFILE;

	if (strchr(fmode, '+')) {
		flags |= O_CREAT | O_RDWR;
		if (fmode[0] == 'w') {
			flags |= O_TRUNC;
		}
	} else {
		if (fmode[0] == 'w') {
			flags |= O_CREAT | O_TRUNC | O_WRONLY;
		} else {
			flags |= O_RDONLY;
		}
	}
	fd = _wopen(m_fileName_w, flags, 0666);

	if (fd >= 0) {
		openFile = _wfdopen(fd, fmode);
	}
#else
	openFile = _wfopen(m_fileName_w, fmode);
#endif
	m_pFile = openFile;

	if (m_pFile == NULL) {
		throw new MP4Error(errno, "failed", "MP4Open");
	}
	
	m_virtual_IO = &FILE_virtual_IO;
	if (m_mode == 'r') {
	  m_orgFileSize = m_fileSize = m_virtual_IO->GetFileLength(m_pFile); // benski
	} else {
		m_orgFileSize = m_fileSize = 0;
	}
}
#endif

void MP4File::ReadFromFile()
{
	// ensure we start at beginning of file
	SetPosition(0);

	// create a new root atom
	ASSERT(m_pRootAtom == NULL);
	m_pRootAtom = MP4Atom::CreateAtom(NULL);

	u_int64_t fileSize = GetSize();

	m_pRootAtom->SetFile(this);
	m_pRootAtom->SetStart(0);
	m_pRootAtom->SetSize(fileSize);
	m_pRootAtom->SetEnd(fileSize);

	m_pRootAtom->Read();

	// create MP4Track's for any tracks in the file
	GenerateTracks();
}

void MP4File::GenerateTracks()
{
	u_int32_t trackIndex = 0;

	while (true) {
		char trackName[32];
		snprintf(trackName, sizeof(trackName), "moov.trak[%u]", trackIndex);

		// find next trak atom
		MP4Atom* pTrakAtom = m_pRootAtom->FindAtom(trackName);

		// done, no more trak atoms
		if (pTrakAtom == NULL) {
			break;
		}

		// find track id property
		MP4Integer32Property* pTrackIdProperty = NULL;
		(void)pTrakAtom->FindProperty(
			"trak.tkhd.trackId",
			(MP4Property**)&pTrackIdProperty);

		// find track type property
		MP4StringProperty* pTypeProperty = NULL;
		(void)pTrakAtom->FindProperty(
			"trak.mdia.hdlr.handlerType",
			(MP4Property**)&pTypeProperty);

		// ensure we have the basics properties
		if (pTrackIdProperty && pTypeProperty) {

			m_trakIds.Add(pTrackIdProperty->GetValue());

			MP4Track* pTrack = NULL;
			try {
				if (!strcmp(pTypeProperty->GetValue(), MP4_HINT_TRACK_TYPE)) {
					pTrack = new MP4RtpHintTrack(this, pTrakAtom);
				} else {
					pTrack = new MP4Track(this, pTrakAtom);
				}
				m_pTracks.Add(pTrack);
			}
			catch (MP4Error* e) {
				VERBOSE_ERROR(m_verbosity, e->Print());
				delete e;
			}

			// remember when we encounter the OD track
			if (pTrack && !strcmp(pTrack->GetType(), MP4_OD_TRACK_TYPE)) {
				if (m_odTrackId == MP4_INVALID_TRACK_ID) {
					m_odTrackId = pTrackIdProperty->GetValue();
				} else {
					VERBOSE_READ(GetVerbosity(),
						printf("Warning: multiple OD tracks present\n"));
				}
			}
		} else {
			m_trakIds.Add(0);
		}

		trackIndex++;
	}
}

void MP4File::CacheProperties()
{
	FindIntegerProperty("moov.mvhd.modificationTime", 
		(MP4Property**)&m_pModificationProperty);

	FindIntegerProperty("moov.mvhd.timeScale", 
		(MP4Property**)&m_pTimeScaleProperty);

	FindIntegerProperty("moov.mvhd.duration", 
		(MP4Property**)&m_pDurationProperty);
}

void MP4File::BeginWrite()
{
	m_pRootAtom->BeginWrite();
}

void MP4File::FinishWrite()
{
	// for all tracks, flush chunking buffers
	for (u_int32_t i = 0; i < m_pTracks.Size(); i++) {
		ASSERT(m_pTracks[i]);
		m_pTracks[i]->FinishWrite();
	}
	// ask root atom to write
	m_pRootAtom->FinishWrite();

	// check if file shrunk, e.g. we deleted a track
	if (GetSize() < m_orgFileSize) {
		// just use a free atom to mark unused space
		// MP4Optimize() should be used to clean up this space
		MP4Atom* pFreeAtom = MP4Atom::CreateAtom("free");
		ASSERT(pFreeAtom);
		pFreeAtom->SetFile(this);
		int64_t size = m_orgFileSize - (m_fileSize + 8);
		if (size < 0) size = 0;
		pFreeAtom->SetSize(size);
		pFreeAtom->Write();
		delete pFreeAtom;
	}
}

void MP4File::UpdateDuration(MP4Duration duration)
{
	MP4Duration currentDuration = GetDuration();
	if (duration > currentDuration) {
		SetDuration(duration);
	}
}

void MP4File::Dump(FILE* pDumpFile, bool dumpImplicits)
{
	if (pDumpFile == NULL) {
		pDumpFile = stdout;
	}

	fprintf(pDumpFile, "Dumping %s meta-information...\n", m_fileName);
	m_pRootAtom->Dump(pDumpFile, 0, dumpImplicits);
}

void MP4File::Close()
{
	if (m_mode == 'w') {
		SetIntegerProperty("moov.mvhd.modificationTime", 
			MP4GetAbsTimestamp());

		FinishWrite();
	}

	m_virtual_IO->Close(m_pFile);
	m_pFile = NULL;
}

const char* MP4File::TempFileName()
{
	// there are so many attempts in libc to get this right
	// that for portablity reasons, it's best just to roll our own
#ifndef _WIN32
	u_int32_t i;
	for (i = getpid(); i < 0xFFFFFFFF; i++) {
		snprintf(m_tempFileName, sizeof(m_tempFileName), 
			 "./tmp%u.mp4", i);
		if (access(m_tempFileName, F_OK) != 0) {
			break;
		}
	}
	if (i == 0xFFFFFFFF) {
		throw new MP4Error("can't create temporary file", "TempFileName");
	}
#else
	GetTempFileNameA(".", // dir. for temp. files 
			"mp4",                // temp. filename prefix 
			0,                    // create unique name 
			m_tempFileName);        // buffer for name 
#endif

	return (char *)m_tempFileName;
}

void MP4File::Rename(const char* oldFileName, const char* newFileName)
{
	int rc;

#ifdef _WIN32
	rc = remove(newFileName);
	if (rc == 0) {
		rc = rename(oldFileName, newFileName);
	}
#else
	rc = rename(oldFileName, newFileName);
#endif
	if (rc != 0) {
		throw new MP4Error(errno, "can't overwrite existing file", "Rename");
	}
}

void MP4File::ProtectWriteOperation(char* where)
{
	if (m_mode == 'r') {
		throw new MP4Error("operation not permitted in read mode", where);
	}
}

MP4Track* MP4File::GetTrack(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)];
}

MP4Atom* MP4File::FindAtom(const char* name)
{
	MP4Atom* pAtom = NULL;
	if (!name || !strcmp(name, "")) {
		pAtom = m_pRootAtom;
	} else {
		pAtom = m_pRootAtom->FindAtom(name);
	}
	return pAtom;
}

MP4Atom* MP4File::AddChildAtom(
	const char* parentName, 
	const char* childName)
{
	return AddChildAtom(FindAtom(parentName), childName);
}

MP4Atom* MP4File::AddChildAtom(
	MP4Atom* pParentAtom, 
	const char* childName)
{
	return InsertChildAtom(pParentAtom, childName, 
		pParentAtom->GetNumberOfChildAtoms());
}

MP4Atom* MP4File::InsertChildAtom(
	const char* parentName, 
	const char* childName, 
	u_int32_t index)
{
	return InsertChildAtom(FindAtom(parentName), childName, index); 
}

MP4Atom* MP4File::InsertChildAtom(
	MP4Atom* pParentAtom, 
	const char* childName, 
	u_int32_t index)
{
	MP4Atom* pChildAtom = MP4Atom::CreateAtom(childName);

	ASSERT(pParentAtom);
	pParentAtom->InsertChildAtom(pChildAtom, index);

	pChildAtom->Generate();

	return pChildAtom;
}

MP4Atom* MP4File::AddDescendantAtoms(
	const char* ancestorName, 
	const char* descendantNames)
{
	return AddDescendantAtoms(FindAtom(ancestorName), descendantNames);
}

MP4Atom* MP4File::AddDescendantAtoms(
	MP4Atom* pAncestorAtom, const char* descendantNames)
{
	ASSERT(pAncestorAtom);

	MP4Atom* pParentAtom = pAncestorAtom;
	MP4Atom* pChildAtom = NULL;

	while (true) {
		char* childName = MP4NameFirst(descendantNames);

		if (childName == NULL) {
			break;
		}

		descendantNames = MP4NameAfterFirst(descendantNames);

		pChildAtom = pParentAtom->FindChildAtom(childName);

		if (pChildAtom == NULL) {
			pChildAtom = AddChildAtom(pParentAtom, childName);
		}

		pParentAtom = pChildAtom;

		MP4Free(childName);
	}

	return pChildAtom;
}

bool MP4File::FindProperty(const char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (pIndex) {
		*pIndex = 0;	// set the default answer for index
	}

	return m_pRootAtom->FindProperty(name, ppProperty, pIndex);
}

void MP4File::FindIntegerProperty(const char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (!FindProperty(name, ppProperty, pIndex)) {
		throw new MP4Error("no such property - %s", "MP4File::FindIntegerProperty", name);
	}

	switch ((*ppProperty)->GetType()) {
	case Integer8Property:
	case Integer16Property:
	case Integer24Property:
	case Integer32Property:
	case Integer64Property:
		break;
	default:
	  throw new MP4Error("type mismatch - property %s type %d", "MP4File::FindIntegerProperty", name, (*ppProperty)->GetType());
	}
}

u_int64_t MP4File::GetIntegerProperty(const char* name)
{
	MP4Property* pProperty;
	u_int32_t index;

	FindIntegerProperty(name, &pProperty, &index);

	return ((MP4IntegerProperty*)pProperty)->GetValue(index);
}

void MP4File::SetIntegerProperty(const char* name, u_int64_t value)
{
	ProtectWriteOperation("SetIntegerProperty");

	MP4Property* pProperty = NULL;
	u_int32_t index = 0;

	FindIntegerProperty(name, &pProperty, &index);

	((MP4IntegerProperty*)pProperty)->SetValue(value, index);
}

void MP4File::FindFloatProperty(const char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (!FindProperty(name, ppProperty, pIndex)) {
		throw new MP4Error("no such property - %s", "MP4File::FindFloatProperty", name);
	}
	if ((*ppProperty)->GetType() != Float32Property) {
		throw new MP4Error("type mismatch - property %s type %d", 
				   "MP4File::FindFloatProperty",
				   name, 
				   (*ppProperty)->GetType());
	}
}

float MP4File::GetFloatProperty(const char* name)
{
	MP4Property* pProperty;
	u_int32_t index;

	FindFloatProperty(name, &pProperty, &index);

	return ((MP4Float32Property*)pProperty)->GetValue(index);
}

void MP4File::SetFloatProperty(const char* name, float value)
{
	ProtectWriteOperation("SetFloatProperty");

	MP4Property* pProperty;
	u_int32_t index;

	FindFloatProperty(name, &pProperty, &index);

	((MP4Float32Property*)pProperty)->SetValue(value, index);
}

void MP4File::FindStringProperty(const char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (!FindProperty(name, ppProperty, pIndex)) {
		throw new MP4Error("no such property - %s", "MP4File::FindStringProperty", name);
	}
	if ((*ppProperty)->GetType() != StringProperty) {
		throw new MP4Error("type mismatch - property %s type %d", "MP4File::FindStringProperty",
				   name, (*ppProperty)->GetType());
	}
}

const char* MP4File::GetStringProperty(const char* name)
{
	MP4Property* pProperty;
	u_int32_t index;

	FindStringProperty(name, &pProperty, &index);

	return ((MP4StringProperty*)pProperty)->GetValue(index);
}

void MP4File::SetStringProperty(const char* name, const char* value)
{
	ProtectWriteOperation("SetStringProperty");

	MP4Property* pProperty;
	u_int32_t index;

	FindStringProperty(name, &pProperty, &index);

	((MP4StringProperty*)pProperty)->SetValue(value, index);
}

void MP4File::FindBytesProperty(const char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	if (!FindProperty(name, ppProperty, pIndex)) {
		throw new MP4Error("no such property %s", "MP4File::FindBytesProperty", name);
	}
	if ((*ppProperty)->GetType() != BytesProperty) {
		throw new MP4Error("type mismatch - property %s - type %d", "MP4File::FindBytesProperty", name, (*ppProperty)->GetType());
	}
}

void MP4File::GetBytesProperty(const char* name, 
	u_int8_t** ppValue, u_int32_t* pValueSize)
{
	MP4Property* pProperty;
	u_int32_t index;

	FindBytesProperty(name, &pProperty, &index);

	((MP4BytesProperty*)pProperty)->GetValue(ppValue, pValueSize, index);
}

void MP4File::SetBytesProperty(const char* name, 
	const u_int8_t* pValue, u_int32_t valueSize)
{
	ProtectWriteOperation("SetBytesProperty");

	MP4Property* pProperty;
	u_int32_t index;

	FindBytesProperty(name, &pProperty, &index);

	((MP4BytesProperty*)pProperty)->SetValue(pValue, valueSize, index);
}


// track functions

MP4TrackId MP4File::AddTrack(const char* type, u_int32_t timeScale)
{
	ProtectWriteOperation("AddTrack");

	// create and add new trak atom
	MP4Atom* pTrakAtom = AddChildAtom("moov", "trak");

	// allocate a new track id
	MP4TrackId trackId = AllocTrackId();

	m_trakIds.Add(trackId);

	// set track id
	MP4Integer32Property* pInteger32Property = NULL;
	(void)pTrakAtom->FindProperty("trak.tkhd.trackId", 
				       (MP4Property**)&pInteger32Property);
	ASSERT(pInteger32Property);
	pInteger32Property->SetValue(trackId);

	// set track type
	const char* normType = MP4NormalizeTrackType(type, m_verbosity);

	// sanity check for user defined types
	if (strlen(normType) > 4) {
		VERBOSE_WARNING(m_verbosity, 
			printf("AddTrack: type truncated to four characters\n"));
		// StringProperty::SetValue() will do the actual truncation
	}

	MP4StringProperty* pStringProperty = NULL;
	(void)pTrakAtom->FindProperty("trak.mdia.hdlr.handlerType", 
				      (MP4Property**)&pStringProperty);
	ASSERT(pStringProperty);
	pStringProperty->SetValue(normType);

	// set track time scale
	pInteger32Property = NULL;
	(void)pTrakAtom->FindProperty("trak.mdia.mdhd.timeScale", 
				       (MP4Property**)&pInteger32Property);
	ASSERT(pInteger32Property);
	pInteger32Property->SetValue(timeScale ? timeScale : 1000);

	// now have enough to create MP4Track object
	MP4Track* pTrack = NULL;
	if (!strcmp(normType, MP4_HINT_TRACK_TYPE)) {
	  pTrack = new MP4RtpHintTrack(this, pTrakAtom);
	} else {
	  pTrack = new MP4Track(this, pTrakAtom);
	}
	m_pTracks.Add(pTrack);

	// mark non-hint tracks as enabled
	if (strcmp(normType, MP4_HINT_TRACK_TYPE)) {
	  SetTrackIntegerProperty(trackId, "tkhd.flags", 1);
	}

	// mark track as contained in this file
	// LATER will provide option for external data references
	AddDataReference(trackId, NULL);

	return trackId;
}

void MP4File::AddTrackToIod(MP4TrackId trackId)
{
	MP4DescriptorProperty* pDescriptorProperty = NULL;
	(void)m_pRootAtom->FindProperty("moov.iods.esIds", 
				  (MP4Property**)&pDescriptorProperty);
	ASSERT(pDescriptorProperty);

	MP4Descriptor* pDescriptor = 
		pDescriptorProperty->AddDescriptor(MP4ESIDIncDescrTag);
	ASSERT(pDescriptor);

	MP4Integer32Property* pIdProperty = NULL;
	(void)pDescriptor->FindProperty("id", 
					(MP4Property**)&pIdProperty);
	ASSERT(pIdProperty);

	pIdProperty->SetValue(trackId);
}

 void MP4File::RemoveTrackFromIod(MP4TrackId trackId, bool shallHaveIods)
{
	MP4DescriptorProperty* pDescriptorProperty = NULL;
	if (!m_pRootAtom->FindProperty("moov.iods.esIds",
				       (MP4Property**)&pDescriptorProperty))
	  return;
#if 0
	// we may not have iods
	if (shallHaveIods) {
		ASSERT(pDescriptorProperty);
	} else {
		if (!pDescriptorProperty) {
			return;
		}
	}
#else
	if (pDescriptorProperty == NULL) {
	  return;
	}
#endif

	for (u_int32_t i = 0; i < pDescriptorProperty->GetCount(); i++) {
	  /* static */char name[32];
		snprintf(name, sizeof(name), "esIds[%u].id", i);

		MP4Integer32Property* pIdProperty = NULL;
		(void)pDescriptorProperty->FindProperty(name, 
			(MP4Property**)&pIdProperty);
		// wmay ASSERT(pIdProperty);

		if (pIdProperty != NULL && 
		    pIdProperty->GetValue() == trackId) {
			pDescriptorProperty->DeleteDescriptor(i);
			break;
		}
	}
}

void MP4File::AddTrackToOd(MP4TrackId trackId)
{
	if (!m_odTrackId) {
		return;
	}

	AddTrackReference(MakeTrackName(m_odTrackId, "tref.mpod"), trackId);
}

void MP4File::RemoveTrackFromOd(MP4TrackId trackId)
{
	if (!m_odTrackId) {
		return;
	}

	RemoveTrackReference(MakeTrackName(m_odTrackId, "tref.mpod"), trackId);
}

void MP4File::GetTrackReferenceProperties(const char* trefName,
	MP4Property** ppCountProperty, MP4Property** ppTrackIdProperty)
{
	char propName[1024];

	snprintf(propName, sizeof(propName), "%s.%s", trefName, "entryCount");
	(void)m_pRootAtom->FindProperty(propName, ppCountProperty);
	ASSERT(*ppCountProperty);

	snprintf(propName, sizeof(propName), "%s.%s", trefName, "entries.trackId");
	(void)m_pRootAtom->FindProperty(propName, ppTrackIdProperty);
	ASSERT(*ppTrackIdProperty);
}

void MP4File::AddTrackReference(const char* trefName, MP4TrackId refTrackId)
{
	MP4Integer32Property* pCountProperty = NULL;
	MP4Integer32Property* pTrackIdProperty = NULL;

	GetTrackReferenceProperties(trefName,
		(MP4Property**)&pCountProperty, 
		(MP4Property**)&pTrackIdProperty);

	pTrackIdProperty->AddValue(refTrackId);
	pCountProperty->IncrementValue();
}

u_int32_t MP4File::FindTrackReference(const char* trefName, 
	MP4TrackId refTrackId)
{
	MP4Integer32Property* pCountProperty = NULL;
	MP4Integer32Property* pTrackIdProperty = NULL;

	GetTrackReferenceProperties(trefName, 
		(MP4Property**)&pCountProperty, 
		(MP4Property**)&pTrackIdProperty);

	for (u_int32_t i = 0; i < pCountProperty->GetValue(); i++) {
		if (refTrackId == pTrackIdProperty->GetValue(i)) {
			return i + 1;	// N.B. 1 not 0 based index
		}
	}
	return 0;
}

void MP4File::RemoveTrackReference(const char* trefName, MP4TrackId refTrackId)
{
	MP4Integer32Property* pCountProperty = NULL;
	MP4Integer32Property* pTrackIdProperty = NULL;

	GetTrackReferenceProperties(trefName,
		(MP4Property**)&pCountProperty, 
		(MP4Property**)&pTrackIdProperty);

	for (u_int32_t i = 0; i < pCountProperty->GetValue(); i++) {
		if (refTrackId == pTrackIdProperty->GetValue(i)) {
			pTrackIdProperty->DeleteValue(i);
			pCountProperty->IncrementValue(-1);
		}
	}
}

void MP4File::AddDataReference(MP4TrackId trackId, const char* url)
{
	MP4Atom* pDrefAtom = 
		FindAtom(MakeTrackName(trackId, "mdia.minf.dinf.dref"));
	ASSERT(pDrefAtom);

	MP4Integer32Property* pCountProperty = NULL;
	(void)pDrefAtom->FindProperty("dref.entryCount", 
				(MP4Property**)&pCountProperty);
	ASSERT(pCountProperty);
	pCountProperty->IncrementValue();

	MP4Atom* pUrlAtom = AddChildAtom(pDrefAtom, "url ");

	if (url && url[0] != '\0') {
		pUrlAtom->SetFlags(pUrlAtom->GetFlags() & 0xFFFFFE);

		MP4StringProperty* pUrlProperty = NULL;
		(void)pUrlAtom->FindProperty("url .location",
					     (MP4Property**)&pUrlProperty);
		ASSERT(pUrlProperty);
		pUrlProperty->SetValue(url);
	} else {
		pUrlAtom->SetFlags(pUrlAtom->GetFlags() | 1);
	}
}

MP4TrackId MP4File::AddSystemsTrack(const char* type)
{
	const char* normType = MP4NormalizeTrackType(type, m_verbosity); 

	// TBD if user type, fix name to four chars, and warn

	MP4TrackId trackId = AddTrack(type, MP4_MSECS_TIME_SCALE);

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "nmhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "mp4s");

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the mp4s atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	SetTrackIntegerProperty(trackId, 
				"mdia.minf.stbl.stsd.mp4s.esds.ESID", 
#if 0
				// note - for a file, these values need to 
				// be 0 - wmay - 04/16/2003
				trackId
#else
				0
#endif
				);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4s.esds.decConfigDescr.objectTypeId", 
		MP4SystemsV1ObjectType);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4s.esds.decConfigDescr.streamType", 
		ConvertTrackTypeToStreamType(normType));

	return trackId;
}

MP4TrackId MP4File::AddODTrack()
{
	// until a demonstrated need emerges
	// we limit ourselves to one object description track
	if (m_odTrackId != MP4_INVALID_TRACK_ID) {
		throw new MP4Error("object description track already exists",
			"AddObjectDescriptionTrack");
	}

	m_odTrackId = AddSystemsTrack(MP4_OD_TRACK_TYPE);

	AddTrackToIod(m_odTrackId);

	(void)AddDescendantAtoms(MakeTrackName(m_odTrackId, NULL), "tref.mpod");

	return m_odTrackId;
}

MP4TrackId MP4File::AddSceneTrack()
{
	MP4TrackId trackId = AddSystemsTrack(MP4_SCENE_TRACK_TYPE);

	AddTrackToIod(trackId);
	AddTrackToOd(trackId);

	return trackId;
}

// NULL terminated list of brands which require the IODS atom
char *brandsWithIods[] = { "mp42",
                           "isom",
                           NULL};

bool MP4File::ShallHaveIods()
{
	u_int32_t compatibleBrandsCount;
        MP4StringProperty *pMajorBrandProperty;
	
	MP4Atom* ftypAtom = m_pRootAtom->FindAtom("ftyp");
	if (ftypAtom == NULL) return false;
	
        // Check the major brand
	(void)ftypAtom->FindProperty(
				     "ftyp.majorBrand",
				     (MP4Property**)&pMajorBrandProperty);
	ASSERT(pMajorBrandProperty);
        for(u_int32_t j = 0 ; brandsWithIods[j] != NULL ; j++) {
                if (!strcasecmp( ((MP4StringProperty*)pMajorBrandProperty)->GetValue(),
                                 brandsWithIods[j]))
                        return true;
        }

        // Check the compatible brands
	MP4Integer32Property* pCompatibleBrandsCountProperty;
        (void)ftypAtom->FindProperty(
			"ftyp.compatibleBrandsCount",
			(MP4Property**)&pCompatibleBrandsCountProperty);
	ASSERT(pCompatibleBrandsCountProperty);
	
	compatibleBrandsCount  = pCompatibleBrandsCountProperty->GetValue();
	
	MP4TableProperty* pCompatibleBrandsProperty;
	(void)ftypAtom->FindProperty(
			"ftyp.compatibleBrands",
			(MP4Property**)&pCompatibleBrandsProperty);
	
	MP4StringProperty* pBrandProperty = (MP4StringProperty*)pCompatibleBrandsProperty->GetProperty(0);
	ASSERT(pBrandProperty);

        for(u_int32_t i = 0 ; i < compatibleBrandsCount ; i++) {
                for(u_int32_t j = 0 ; brandsWithIods[j] != NULL ; j++) {
                        if (!strcasecmp(pBrandProperty->GetValue(i), brandsWithIods[j]))
                                return true;
                }
        }

	return false;
}

void MP4File::SetAmrVendor(
		MP4TrackId trackId,
		u_int32_t vendor)
{
	SetTrackIntegerProperty(trackId, 
				"mdia.minf.stbl.stsd.*.damr.vendor",
				vendor);
}

void MP4File::SetAmrDecoderVersion(
		MP4TrackId trackId,
		u_int8_t decoderVersion)
{

	SetTrackIntegerProperty(trackId, 
				"mdia.minf.stbl.stsd.*.damr.decoderVersion",
				decoderVersion);
}

void MP4File::SetAmrModeSet(
		MP4TrackId trackId,
		u_int16_t modeSet)
{
  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.*.damr.modeSet",
			  modeSet);
}
uint16_t MP4File::GetAmrModeSet(MP4TrackId trackId)
{
  return GetTrackIntegerProperty(trackId, 
				 "mdia.minf.stbl.stsd.*.damr.modeSet");
}

MP4TrackId MP4File::AddAmrAudioTrack(
		        u_int32_t timeScale,
			u_int16_t modeSet,
			u_int8_t modeChangePeriod,
			u_int8_t framesPerSample,
			bool isAmrWB)
{
	
	u_int32_t fixedSampleDuration = (timeScale * 20)/1000; // 20mSec/Sample
	
	MP4TrackId trackId = AddTrack(MP4_AUDIO_TRACK_TYPE, timeScale);
	
	AddTrackToOd(trackId);
	
	SetTrackFloatProperty(trackId, "tkhd.volume", 1.0);
	
	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "smhd", 0);
	
	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), isAmrWB ? "sawb" : "samr");
	
	// stsd is a unique beast in that it has a count of the number
	// of child atoms that needs to be incremented after we add the mp4a atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
			MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
			(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();
	
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.*.timeScale", 
				timeScale);
	
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.*.damr.modeSet", 
				modeSet);
	
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.*.damr.modeChangePeriod", 
				modeChangePeriod);
	
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.*.damr.framesPerSample", 
				framesPerSample);
	
	
	m_pTracks[FindTrackIndex(trackId)]->
		SetFixedSampleDuration(fixedSampleDuration);

	return trackId;
}

MP4TrackId MP4File::AddAudioTrack(
	u_int32_t timeScale, 
	MP4Duration sampleDuration, 
	u_int8_t audioType)
{
	MP4TrackId trackId = AddTrack(MP4_AUDIO_TRACK_TYPE, timeScale);

	AddTrackToOd(trackId);

	SetTrackFloatProperty(trackId, "tkhd.volume", 1.0);

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "smhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "mp4a");

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the mp4a atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4a.timeScale", timeScale);

	SetTrackIntegerProperty(trackId, 
				"mdia.minf.stbl.stsd.mp4a.esds.ESID", 
#if 0
				// note - for a file, these values need to 
				// be 0 - wmay - 04/16/2003
				trackId
#else
				0
#endif
				);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4a.esds.decConfigDescr.objectTypeId", 
		audioType);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4a.esds.decConfigDescr.streamType", 
		MP4AudioStreamType);

	m_pTracks[FindTrackIndex(trackId)]->
		SetFixedSampleDuration(sampleDuration);

	return trackId;
}

MP4TrackId MP4File::AddEncAudioTrack(u_int32_t timeScale, 
				     MP4Duration sampleDuration, 
				     u_int8_t audioType,
				     u_int32_t scheme_type,
				     u_int16_t scheme_version,
                                     u_int8_t  key_ind_len,
                                     u_int8_t  iv_len,
                                     bool      selective_enc,
                                     const char *kms_uri,
				     bool use_ismacryp
                                     )
{
  u_int32_t original_fmt = 0;

  MP4TrackId trackId = AddTrack(MP4_AUDIO_TRACK_TYPE, timeScale);

  AddTrackToOd(trackId);

  SetTrackFloatProperty(trackId, "tkhd.volume", 1.0);

  (void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "smhd", 0);

  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "enca");

  // stsd is a unique beast in that it has a count of the number 
  // of child atoms that needs to be incremented after we add the enca atom
  MP4Integer32Property* pStsdCountProperty;
  FindIntegerProperty(MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		      (MP4Property**)&pStsdCountProperty);
  pStsdCountProperty->IncrementValue();


  /* set all the ismacryp-specific values */
  // original format is mp4a
  if (use_ismacryp) {
    original_fmt = ATOMID("mp4a");
    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.frma.data-format", 
			    original_fmt);

    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.enca.sinf"), 
		 "schm");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.enca.sinf"), 
		 "schi");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.enca.sinf.schi"), 
		 "iKMS");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.enca.sinf.schi"), 
		 "iSFM");
    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.schm.scheme_type", 
			    scheme_type);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.schm.scheme_version", 
			    scheme_version);
  
    SetTrackStringProperty(trackId,
			   "mdia.minf.stbl.stsd.enca.sinf.schi.iKMS.kms_URI", 
			   kms_uri);
    #if 0
    if (kms_uri != NULL) {
      free((void *)kms_uri);
    }  
    #endif

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.schi.iSFM.selective-encryption", 
			    selective_enc);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.schi.iSFM.key-indicator-length", 
			    key_ind_len);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.enca.sinf.schi.iSFM.IV-length", 
			    iv_len);
    /* end ismacryp */
  }

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.enca.timeScale", timeScale);

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.enca.esds.ESID", 
#if 0
			  // note - for a file, these values need to 
			  // be 0 - wmay - 04/16/2003
			  trackId
#else
			  0
#endif
			  );

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.enca.esds.decConfigDescr.objectTypeId", 
			  audioType);

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.enca.esds.decConfigDescr.streamType", 
			  MP4AudioStreamType);

  m_pTracks[FindTrackIndex(trackId)]->
    SetFixedSampleDuration(sampleDuration);

  return trackId;
}

MP4TrackId MP4File::AddCntlTrackDefault (uint32_t timeScale,
					 MP4Duration sampleDuration,
					 const char *type)
{
  MP4TrackId trackId = AddTrack(MP4_CNTL_TRACK_TYPE, timeScale);

  (void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "nmhd", 0);
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), type);

  // stsd is a unique beast in that it has a count of the number 
  // of child atoms that needs to be incremented after we add the mp4v atom
  MP4Integer32Property* pStsdCountProperty;
  FindIntegerProperty(
		      MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		      (MP4Property**)&pStsdCountProperty);
  pStsdCountProperty->IncrementValue();
  
  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsz.sampleSize", sampleDuration);
  
  m_pTracks[FindTrackIndex(trackId)]->
    SetFixedSampleDuration(sampleDuration);
  
  return trackId;
}

MP4TrackId MP4File::AddHrefTrack (uint32_t timeScale, 
				  MP4Duration sampleDuration,
				  const char *base_url)
{
  MP4TrackId trackId = AddCntlTrackDefault(timeScale, sampleDuration, "href");

  if (base_url != NULL) {
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.href"), 
		 "burl");
    SetTrackStringProperty(trackId, "mdia.minf.stbl.stsd.href.burl.base_url",
			   base_url);
  }

  return trackId;
}
		  
MP4TrackId MP4File::AddVideoTrackDefault(
	u_int32_t timeScale, 
	MP4Duration sampleDuration, 
	u_int16_t width, 
	u_int16_t height, 
	const char *videoType)
{
	MP4TrackId trackId = AddTrack(MP4_VIDEO_TRACK_TYPE, timeScale);

	AddTrackToOd(trackId);

	SetTrackFloatProperty(trackId, "tkhd.width", width);
	SetTrackFloatProperty(trackId, "tkhd.height", height);

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "vmhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), videoType);

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the mp4v atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsz.sampleSize", sampleDuration);

	m_pTracks[FindTrackIndex(trackId)]->
		SetFixedSampleDuration(sampleDuration);

	return trackId;
}
MP4TrackId MP4File::AddMP4VideoTrack(
	u_int32_t timeScale, 
	MP4Duration sampleDuration, 
	u_int16_t width, 
	u_int16_t height, 
	u_int8_t videoType)
{
  MP4TrackId trackId = AddVideoTrackDefault(timeScale, 
					    sampleDuration,
					    width, 
					    height,
					    "mp4v");

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4v.width", width);
	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4v.height", height);

	SetTrackIntegerProperty(trackId, 
				"mdia.minf.stbl.stsd.mp4v.esds.ESID", 
#if 0
				// note - for a file, these values need to 
				// be 0 - wmay - 04/16/2003
				trackId
#else
				0
#endif
				);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4v.esds.decConfigDescr.objectTypeId", 
		videoType);

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.mp4v.esds.decConfigDescr.streamType", 
		MP4VisualStreamType);

	return trackId;
}

// ismacrypted
MP4TrackId MP4File::AddEncVideoTrack(u_int32_t timeScale, 
				     MP4Duration sampleDuration, 
				     u_int16_t width, 
				     u_int16_t height, 
				     u_int8_t videoType,
				  	mp4v2_ismacrypParams *icPp,
					const char *oFormat
                                     )
{
  u_int32_t original_fmt = 0;

  MP4TrackId trackId = AddVideoTrackDefault(timeScale, 
					    sampleDuration,
					    width,
					    height,
					    "encv");

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.encv.width", width);
  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.encv.height", height);

  /* set all the ismacryp-specific values */

    original_fmt = ATOMID(oFormat);
	
    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.frma.data-format", 
			    original_fmt);

    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf"), 
		 "schm");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf"), 
		 "schi");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi"), 
		 "iKMS");
    (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi"), 
		 "iSFM");

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.schm.scheme_type", 
			    icPp->scheme_type);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.schm.scheme_version", 
			    icPp->scheme_version);
  
    SetTrackStringProperty(trackId,
			   "mdia.minf.stbl.stsd.encv.sinf.schi.iKMS.kms_URI", 
			   icPp->kms_uri);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.selective-encryption", 
			    icPp->selective_enc);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.key-indicator-length", 
			    icPp->key_ind_len);

    SetTrackIntegerProperty(trackId,
			    "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.IV-length", 
			    icPp->iv_len);

  #if 0
  if (icPp->kms_uri != NULL) {
    free(icPp->kms_uri);
  }  
  #endif

  SetTrackIntegerProperty(trackId, 
			  "mdia.minf.stbl.stsd.encv.esds.ESID", 
#if 0
			  // note - for a file, these values need to 
			  // be 0 - wmay - 04/16/2003
			  trackId
#else
			  0
#endif
			  );

  SetTrackIntegerProperty(trackId, 
		  "mdia.minf.stbl.stsd.encv.esds.decConfigDescr.objectTypeId", 
			  videoType);

  SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.encv.esds.decConfigDescr.streamType", 
			  MP4VisualStreamType);

  return trackId;
}

MP4TrackId MP4File::AddH264VideoTrack(
	u_int32_t timeScale, 
	MP4Duration sampleDuration, 
	u_int16_t width, 
	u_int16_t height, 
	uint8_t AVCProfileIndication,
	uint8_t profile_compat,
	uint8_t AVCLevelIndication,
	uint8_t sampleLenFieldSizeMinusOne)
{
  MP4TrackId trackId = AddVideoTrackDefault(timeScale, 
					    sampleDuration,
					    width, 
					    height,
					    "avc1");

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.avc1.width", width);
	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.avc1.height", height);

	//FIXME - check this 
	// shouldn't need this
	#if 0
	AddChildAtom(MakeTrackName(trackId,
				   "mdia.minf.stbl.stsd.avc1"),
		     "avcC");
        #endif
	
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.avc1.avcC.AVCProfileIndication",
				AVCProfileIndication);
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.avc1.avcC.profile_compatibility",
				profile_compat);
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.avc1.avcC.AVCLevelIndication",
				AVCLevelIndication);
	SetTrackIntegerProperty(trackId,
				"mdia.minf.stbl.stsd.avc1.avcC.lengthSizeMinusOne",
				sampleLenFieldSizeMinusOne);
	

	return trackId;
}

MP4TrackId MP4File::AddEncH264VideoTrack(
	u_int32_t timeScale, 
	MP4Duration sampleDuration, 
	u_int16_t width, 
	u_int16_t height, 
  	MP4Atom *srcAtom,
        mp4v2_ismacrypParams *icPp)

{

  u_int32_t original_fmt = 0;
  MP4Atom *avcCAtom;

  MP4TrackId trackId = AddVideoTrackDefault(timeScale, 
					    sampleDuration,
					    width, 
					    height,
					    "encv");

  SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.width", width);
  SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.height", height);
  
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv"), "avcC");
  
  // create default values
  avcCAtom = FindAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.avcC"));
  
  // export source atom 
  ((MP4AvcCAtom *) srcAtom)->Clone((MP4AvcCAtom *)avcCAtom);
  
  /* set all the ismacryp-specific values */
  
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf"), "schm");
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf"), "schi");
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi"), "iKMS");
  (void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi"), "iSFM");

    // per ismacrypt E&A V1.1 section 9.1.2.1 'avc1' is renamed '264b'
    // avc1 must not appear as a sample entry name or original format name
    original_fmt = ATOMID("264b");
    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.frma.data-format", 
			    original_fmt);

    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schm.scheme_type", 
			    icPp->scheme_type);

    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schm.scheme_version", 
			    icPp->scheme_version);
  
    SetTrackStringProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi.iKMS.kms_URI", 
			   icPp->kms_uri);

    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.selective-encryption", 
			    icPp->selective_enc);

    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.key-indicator-length", 
			    icPp->key_ind_len);

    SetTrackIntegerProperty(trackId, "mdia.minf.stbl.stsd.encv.sinf.schi.iSFM.IV-length", 
			    icPp->iv_len);


	return trackId;
}


void MP4File::AddH264SequenceParameterSet (MP4TrackId trackId,
					   const uint8_t *pSequence,
					   uint16_t sequenceLen)
{
  const char *format;
  MP4Atom *avcCAtom;

  // get 4cc media format - can be avc1 or encv for ismacrypted track
  format = GetTrackMediaDataName(trackId);

  if (!strcasecmp(format, "avc1"))
 	 avcCAtom = FindAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.avc1.avcC"));
  else if (!strcasecmp(format, "encv"))
 	 avcCAtom = FindAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.avcC"));
  else 
        // huh?  unknown track format 
	return;


  MP4BitfieldProperty *pCount;
  MP4Integer16Property *pLength;
  MP4BytesProperty *pUnit;
  if ((avcCAtom->FindProperty("avcC.numOfSequenceParameterSets",
			     (MP4Property **)&pCount) == false) ||
      (avcCAtom->FindProperty("avcC.sequenceEntries.sequenceParameterSetLength",
			      (MP4Property **)&pLength) == false) ||
      (avcCAtom->FindProperty("avcC.sequenceEntries.sequenceParameterSetNALUnit",
			      (MP4Property **)&pUnit) == false)) {
    VERBOSE_ERROR(m_verbosity, WARNING("Could not find avcC properties"));
    return;
  }
  uint32_t count = pCount->GetValue();
  
  if (count > 0) {
    // see if we already exist
    for (uint32_t index = 0; index < count; index++) {
      if (pLength->GetValue(index) == sequenceLen) {
	uint8_t *seq;
	uint32_t seqlen;
	pUnit->GetValue(&seq, &seqlen, index);
	if (memcmp(seq, pSequence, sequenceLen) == 0) {
	  free(seq);
	  return;
	}
	free(seq);
      }
    }
  }
  pLength->AddValue(sequenceLen);
  pUnit->AddValue(pSequence, sequenceLen);
  pCount->IncrementValue();

  return;
}
void MP4File::AddH264PictureParameterSet (MP4TrackId trackId,
					  const uint8_t *pPict,
					  uint16_t pictLen)
{
  MP4Atom *avcCAtom = 
    FindAtom(MakeTrackName(trackId,
			   "mdia.minf.stbl.stsd.avc1.avcC"));
  MP4Integer8Property *pCount;
  MP4Integer16Property *pLength;
  MP4BytesProperty *pUnit;
  if ((avcCAtom->FindProperty("avcC.numOfPictureParameterSets",
			     (MP4Property **)&pCount) == false) ||
      (avcCAtom->FindProperty("avcC.pictureEntries.pictureParameterSetLength",
			      (MP4Property **)&pLength) == false) ||
      (avcCAtom->FindProperty("avcC.pictureEntries.pictureParameterSetNALUnit",
			      (MP4Property **)&pUnit) == false)) {
    VERBOSE_ERROR(m_verbosity, 
		  WARNING("Could not find avcC picture table properties"));
    return;
  }
  uint32_t count = pCount->GetValue();
  
  if (count > 0) {
    // see if we already exist
    for (uint32_t index = 0; index < count; index++) {
      if (pLength->GetValue(index) == pictLen) {
	uint8_t *seq;
	uint32_t seqlen;
	pUnit->GetValue(&seq, &seqlen, index);
	if (memcmp(seq, pPict, pictLen) == 0) {
	  VERBOSE_WRITE(m_verbosity, 
			fprintf(stderr, "picture matches %d\n", index));
	  free(seq);
	  return;
	}
	free(seq);
      }
    }
  }
  pLength->AddValue(pictLen);
  pUnit->AddValue(pPict, pictLen);
  pCount->IncrementValue();
  VERBOSE_WRITE(m_verbosity, 
		fprintf(stderr, "new picture added %d\n", pCount->GetValue()));

  return;
}
void  MP4File::SetH263Vendor(
		MP4TrackId trackId,
		u_int32_t vendor)
{
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.vendor",
			vendor);
}

void MP4File::SetH263DecoderVersion(
		MP4TrackId trackId,
		u_int8_t decoderVersion)
{
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.decoderVersion",
			decoderVersion);
}

void MP4File::SetH263Bitrates(
	MP4TrackId trackId,
	u_int32_t avgBitrate,
	u_int32_t maxBitrate)
{
	SetTrackIntegerProperty(trackId, 
			"mdia.minf.stbl.stsd.s263.d263.bitr.avgBitrate", 
			avgBitrate);
	
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.bitr.maxBitrate",
			maxBitrate);

}

MP4TrackId MP4File::AddH263VideoTrack(
		u_int32_t timeScale,
		MP4Duration sampleDuration,
		u_int16_t width,
		u_int16_t height,
		u_int8_t h263Level,
		u_int8_t h263Profile,
		u_int32_t avgBitrate,
		u_int32_t maxBitrate)

{
  MP4TrackId trackId = AddVideoTrackDefault(timeScale, 
					    sampleDuration,
					    width,
					    height,
					    "s263");
	
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.width", width);
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.height", height);
	
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.h263Level", h263Level);
	
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.h263Profile", h263Profile);

	// Add the bitr atom
	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.s263.d263"), 
			"bitr");

	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.bitr.avgBitrate", avgBitrate);

	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsd.s263.d263.bitr.maxBitrate", maxBitrate);
	
	
	SetTrackIntegerProperty(trackId,
			"mdia.minf.stbl.stsz.sampleSize", sampleDuration);
	
	return trackId;

}

MP4TrackId MP4File::AddHintTrack(MP4TrackId refTrackId)
{
	// validate reference track id
  (void)FindTrackIndex(refTrackId);

	MP4TrackId trackId = 
		AddTrack(MP4_HINT_TRACK_TYPE, GetTrackTimeScale(refTrackId));

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "hmhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "rtp ");

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the rtp atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	SetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.rtp .tims.timeScale", 
		GetTrackTimeScale(trackId));

	(void)AddDescendantAtoms(MakeTrackName(trackId, NULL), "tref.hint");

	AddTrackReference(MakeTrackName(trackId, "tref.hint"), refTrackId);

	(void)AddDescendantAtoms(MakeTrackName(trackId, NULL), "udta.hnti.sdp ");

	(void)AddDescendantAtoms(MakeTrackName(trackId, NULL), "udta.hinf");

	return trackId;
}

MP4TrackId MP4File::AddTextTrack(MP4TrackId refTrackId)
{
	// validate reference track id
  (void)FindTrackIndex(refTrackId);

	MP4TrackId trackId = 
		AddTrack(MP4_TEXT_TRACK_TYPE, GetTrackTimeScale(refTrackId));

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "gmhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "text");

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the text atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	return trackId;
}

MP4TrackId MP4File::AddChapterTextTrack(MP4TrackId refTrackId)
{
	// validate reference track id
	(void)FindTrackIndex(refTrackId);

	MP4TrackId trackId = 
		AddTrack(MP4_TEXT_TRACK_TYPE, GetTrackTimeScale(refTrackId));

	(void)InsertChildAtom(MakeTrackName(trackId, "mdia.minf"), "gmhd", 0);

	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd"), "text");

	// stsd is a unique beast in that it has a count of the number 
	// of child atoms that needs to be incremented after we add the text atom
	MP4Integer32Property* pStsdCountProperty;
	FindIntegerProperty(
		MakeTrackName(trackId, "mdia.minf.stbl.stsd.entryCount"),
		(MP4Property**)&pStsdCountProperty);
	pStsdCountProperty->IncrementValue();

	// add a "text" atom to the generic media header
	// this is different to the stsd "text" atom added above
	// truth be told, it's not clear what this second "text" atom does,
	// but all iTunes Store movies (with chapter markers) have it,
	// as do all movies with chapter tracks made by hand in QuickTime Pro
	(void)AddChildAtom(MakeTrackName(trackId, "mdia.minf.gmhd"), "text");

	// disable the chapter text track
	// it won't display anyway, as it has zero display size,
	// but nonetheless it's good to disable it
	// the track still operates as a chapter track when disabled
    MP4Atom *pTkhdAtom = FindAtom(MakeTrackName(trackId, "tkhd"));
	if (pTkhdAtom) {
	   	pTkhdAtom->SetFlags(0xE);
	}

	// add a "chapter" track reference to our reference track,
	// pointing to this new chapter track
	(void)AddDescendantAtoms(MakeTrackName(refTrackId, NULL), "tref.chap");
	AddTrackReference(MakeTrackName(refTrackId, "tref.chap"), trackId);

	return trackId;
}

void MP4File::DeleteTrack(MP4TrackId trackId)
{
	ProtectWriteOperation("MP4DeleteTrack");

	u_int32_t trakIndex = FindTrakAtomIndex(trackId);
	u_int16_t trackIndex = FindTrackIndex(trackId);
	MP4Track* pTrack = m_pTracks[trackIndex];

	MP4Atom* pTrakAtom = pTrack->GetTrakAtom();
	ASSERT(pTrakAtom);

	MP4Atom* pMoovAtom = FindAtom("moov");
	ASSERT(pMoovAtom);

	RemoveTrackFromIod(trackId, ShallHaveIods());
	RemoveTrackFromOd(trackId);

	if (trackId == m_odTrackId) {
		m_odTrackId = 0;
	}

	pMoovAtom->DeleteChildAtom(pTrakAtom);

	m_trakIds.Delete(trakIndex);

	m_pTracks.Delete(trackIndex);

	delete pTrack;
	delete pTrakAtom;
}

u_int32_t MP4File::GetNumberOfTracks(const char* type, u_int8_t subType)
{
	if (type == NULL) {
		return m_pTracks.Size();
	} 

	u_int32_t typeSeen = 0;
	const char* normType = MP4NormalizeTrackType(type, m_verbosity);

	for (u_int32_t i = 0; i < m_pTracks.Size(); i++) {
		if (!strcmp(normType, m_pTracks[i]->GetType())) {
			if (subType) {
				if (normType == MP4_AUDIO_TRACK_TYPE) {
					if (subType != GetTrackEsdsObjectTypeId(m_pTracks[i]->GetId())) {
						continue;
					}
				} else if (normType == MP4_VIDEO_TRACK_TYPE) {
					if (subType != GetTrackEsdsObjectTypeId(m_pTracks[i]->GetId())) {
						continue;
					}
				} 
				// else unknown subtype, ignore it
			}
			typeSeen++;
		}
	}
	return typeSeen;
}

MP4TrackId MP4File::AllocTrackId()
{
	MP4TrackId trackId = 
		GetIntegerProperty("moov.mvhd.nextTrackId");

	if (trackId <= 0xFFFF) {
		// check that nextTrackid is correct
		try {
		  (void)FindTrackIndex(trackId);
			// ERROR, this trackId is in use
		}
		catch (MP4Error* e) {
			// OK, this trackId is not in use, proceed
			delete e;
			SetIntegerProperty("moov.mvhd.nextTrackId", trackId + 1);
			return trackId;
		}
	}

	// we need to search for a track id
	for (trackId = 1; trackId <= 0xFFFF; trackId++) {
		try {
		  (void)FindTrackIndex(trackId);
			// KEEP LOOKING, this trackId is in use
		}
		catch (MP4Error* e) {
			// OK, this trackId is not in use, proceed
			delete e;
			return trackId;
		}
	}

	// extreme case where mp4 file has 2^16 tracks in it
	throw new MP4Error("too many existing tracks", "AddTrack");
	return MP4_INVALID_TRACK_ID;		// to keep MSVC happy
}

MP4TrackId MP4File::FindTrackId(u_int16_t trackIndex, 
				const char* type, u_int8_t subType)
{
  if (type == NULL) {
    return m_pTracks[trackIndex]->GetId();
  } 

  u_int32_t typeSeen = 0;
  const char* normType = MP4NormalizeTrackType(type, m_verbosity);

  for (u_int32_t i = 0; i < m_pTracks.Size(); i++) {
    if (!strcmp(normType, m_pTracks[i]->GetType())) {
      if (subType) {
	if (normType == MP4_AUDIO_TRACK_TYPE) {
	  if (subType != GetTrackEsdsObjectTypeId(m_pTracks[i]->GetId())) {
	    continue;
	  }
	} else if (normType == MP4_VIDEO_TRACK_TYPE) {
	  if (subType != GetTrackEsdsObjectTypeId(m_pTracks[i]->GetId())) {
	    continue;
	  }
	} 
	// else unknown subtype, ignore it
      }

      if (trackIndex == typeSeen) {
	return m_pTracks[i]->GetId();
      }

      typeSeen++;
    }
  }

  throw new MP4Error("Track index doesn't exist - track %d type %s", 
		     "FindTrackId", 
		     trackIndex, type); 
  return MP4_INVALID_TRACK_ID; // satisfy MS compiler
}

u_int16_t MP4File::FindTrackIndex(MP4TrackId trackId)
{
	for (u_int32_t i = 0; i < m_pTracks.Size() && i <= 0xFFFF; i++) {
		if (m_pTracks[i]->GetId() == trackId) {
			return (u_int16_t)i;
		}
	}
	
	throw new MP4Error("Track id %d doesn't exist", "FindTrackIndex", trackId); 
	return (u_int16_t)-1; // satisfy MS compiler
}

u_int16_t MP4File::FindTrakAtomIndex(MP4TrackId trackId)
{
	if (trackId) {
		for (u_int32_t i = 0; i < m_trakIds.Size(); i++) {
			if (m_trakIds[i] == trackId) {
				return i;
			}
		}
	}

	throw new MP4Error("Track id %d doesn't exist", "FindTrakAtomIndex",
			   trackId); 
	return (u_int16_t)-1; // satisfy MS compiler
}

u_int32_t MP4File::GetSampleSize(MP4TrackId trackId, MP4SampleId sampleId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetSampleSize(sampleId);
}

u_int32_t MP4File::GetTrackMaxSampleSize(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetMaxSampleSize();
}

MP4SampleId MP4File::GetSampleIdFromTime(MP4TrackId trackId, 
	MP4Timestamp when, bool wantSyncSample)
{
	return m_pTracks[FindTrackIndex(trackId)]->
		GetSampleIdFromTime(when, wantSyncSample);
}

MP4Timestamp MP4File::GetSampleTime(
	MP4TrackId trackId, MP4SampleId sampleId)
{
	MP4Timestamp timestamp;
	m_pTracks[FindTrackIndex(trackId)]->
		GetSampleTimes(sampleId, &timestamp, NULL);
	return timestamp;
}

MP4Duration MP4File::GetSampleDuration(
	MP4TrackId trackId, MP4SampleId sampleId)
{
	MP4Duration duration;
	m_pTracks[FindTrackIndex(trackId)]->
		GetSampleTimes(sampleId, NULL, &duration);
	return duration; 
}

MP4Duration MP4File::GetSampleRenderingOffset(
	MP4TrackId trackId, MP4SampleId sampleId)
{
	return m_pTracks[FindTrackIndex(trackId)]->
		GetSampleRenderingOffset(sampleId);
}

bool MP4File::GetSampleSync(MP4TrackId trackId, MP4SampleId sampleId)
{
	return m_pTracks[FindTrackIndex(trackId)]->IsSyncSample(sampleId);
}

void MP4File::ReadSample(MP4TrackId trackId, MP4SampleId sampleId,
		u_int8_t** ppBytes, u_int32_t* pNumBytes, 
		MP4Timestamp* pStartTime, MP4Duration* pDuration,
		MP4Duration* pRenderingOffset, bool* pIsSyncSample)
{
	m_pTracks[FindTrackIndex(trackId)]->
		ReadSample(sampleId, ppBytes, pNumBytes, 
			pStartTime, pDuration, pRenderingOffset, pIsSyncSample);
}

void MP4File::WriteSample(MP4TrackId trackId,
		const u_int8_t* pBytes, u_int32_t numBytes,
		MP4Duration duration, MP4Duration renderingOffset, bool isSyncSample)
{
	ProtectWriteOperation("MP4WriteSample");

	m_pTracks[FindTrackIndex(trackId)]->
		WriteSample(pBytes, numBytes, duration, renderingOffset, isSyncSample);

	m_pModificationProperty->SetValue(MP4GetAbsTimestamp());
}

void MP4File::SetSampleRenderingOffset(MP4TrackId trackId, 
	MP4SampleId sampleId, MP4Duration renderingOffset)
{
	ProtectWriteOperation("MP4SetSampleRenderingOffset");

	m_pTracks[FindTrackIndex(trackId)]->
		SetSampleRenderingOffset(sampleId, renderingOffset);

	m_pModificationProperty->SetValue(MP4GetAbsTimestamp());
}

char* MP4File::MakeTrackName(MP4TrackId trackId, const char* name)
{
	u_int16_t trakIndex = FindTrakAtomIndex(trackId);

	if (name == NULL || name[0] == '\0') {
		snprintf(m_trakName, sizeof(m_trakName), 
			"moov.trak[%u]", trakIndex);
	} else {
		snprintf(m_trakName, sizeof(m_trakName), 
			"moov.trak[%u].%s", trakIndex, name);
	}
	return m_trakName;
}

MP4Atom *MP4File::FindTrackAtom (MP4TrackId trackId, const char *name)
{
  return FindAtom(MakeTrackName(trackId, name));
}

u_int64_t MP4File::GetTrackIntegerProperty(MP4TrackId trackId, const char* name)
{
	return GetIntegerProperty(MakeTrackName(trackId, name));
}

void MP4File::SetTrackIntegerProperty(MP4TrackId trackId, const char* name, 
	int64_t value)
{
	SetIntegerProperty(MakeTrackName(trackId, name), value);
}

float MP4File::GetTrackFloatProperty(MP4TrackId trackId, const char* name)
{
	return GetFloatProperty(MakeTrackName(trackId, name));
}

void MP4File::SetTrackFloatProperty(MP4TrackId trackId, const char* name, 
	float value)
{
	SetFloatProperty(MakeTrackName(trackId, name), value);
}

const char* MP4File::GetTrackStringProperty(MP4TrackId trackId, const char* name)
{
	return GetStringProperty(MakeTrackName(trackId, name));
}

void MP4File::SetTrackStringProperty(MP4TrackId trackId, const char* name,
	const char* value)
{
	SetStringProperty(MakeTrackName(trackId, name), value);
}

void MP4File::GetTrackBytesProperty(MP4TrackId trackId, const char* name, 
	u_int8_t** ppValue, u_int32_t* pValueSize)
{
	GetBytesProperty(MakeTrackName(trackId, name), ppValue, pValueSize);
}

void MP4File::SetTrackBytesProperty(MP4TrackId trackId, const char* name, 
	const u_int8_t* pValue, u_int32_t valueSize)
{
	SetBytesProperty(MakeTrackName(trackId, name), pValue, valueSize);
}


// file level convenience functions

MP4Duration MP4File::GetDuration()
{
	return m_pDurationProperty->GetValue();
}

void MP4File::SetDuration(MP4Duration value)
{
	m_pDurationProperty->SetValue(value);
}

u_int32_t MP4File::GetTimeScale()
{
	return m_pTimeScaleProperty->GetValue();
}

void MP4File::SetTimeScale(u_int32_t value)
{
	if (value == 0) {
		throw new MP4Error("invalid value", "SetTimeScale");
	}
	m_pTimeScaleProperty->SetValue(value);
}

u_int8_t MP4File::GetODProfileLevel()
{
	return GetIntegerProperty("moov.iods.ODProfileLevelId");
}

void MP4File::SetODProfileLevel(u_int8_t value)
{
	SetIntegerProperty("moov.iods.ODProfileLevelId", value);
}
 
u_int8_t MP4File::GetSceneProfileLevel()
{
	return GetIntegerProperty("moov.iods.sceneProfileLevelId");
}

void MP4File::SetSceneProfileLevel(u_int8_t value)
{
	SetIntegerProperty("moov.iods.sceneProfileLevelId", value);
}
 
u_int8_t MP4File::GetVideoProfileLevel()
{
	return GetIntegerProperty("moov.iods.visualProfileLevelId");
}

void MP4File::SetVideoProfileLevel(u_int8_t value)
{
	SetIntegerProperty("moov.iods.visualProfileLevelId", value);
}
 
u_int8_t MP4File::GetAudioProfileLevel()
{
	return GetIntegerProperty("moov.iods.audioProfileLevelId");
}

void MP4File::SetAudioProfileLevel(u_int8_t value)
{
	SetIntegerProperty("moov.iods.audioProfileLevelId", value);
}
 
u_int8_t MP4File::GetGraphicsProfileLevel()
{
	return GetIntegerProperty("moov.iods.graphicsProfileLevelId");
}

void MP4File::SetGraphicsProfileLevel(u_int8_t value)
{
	SetIntegerProperty("moov.iods.graphicsProfileLevelId", value);
}
 
const char* MP4File::GetSessionSdp()
{
	return GetStringProperty("moov.udta.hnti.rtp .sdpText");
}

void MP4File::SetSessionSdp(const char* sdpString)
{
  (void)AddDescendantAtoms("moov", "udta.hnti.rtp ");

	SetStringProperty("moov.udta.hnti.rtp .sdpText", sdpString);
}

void MP4File::AppendSessionSdp(const char* sdpFragment)
{
	const char* oldSdpString = NULL;
	try {
		oldSdpString = GetSessionSdp();
	}
	catch (MP4Error* e) {
		delete e;
		SetSessionSdp(sdpFragment);
		return;
	}

	char* newSdpString =
		(char*)MP4Malloc(strlen(oldSdpString) + strlen(sdpFragment) + 1);
	strcpy(newSdpString, oldSdpString);
	strcat(newSdpString, sdpFragment);
	SetSessionSdp(newSdpString);
	MP4Free(newSdpString);
}

//
// ismacrypt API - retrieve OriginalFormatBox 
//
// parameters are assumed to have been sanity tested in mp4.cpp
// don't call this unless media data name is 'encv',
// results may otherwise be unpredictable.
//
// input:
// trackID - valid encv track ID for this file
// buflen  - length of oFormat, minimum is 5 (4cc plus null terminator)
//
// output:
// oFormat - buffer to return null terminated string containing 
//           track original format
// return:
// 0       - original format returned OK
// 1       - buffer length error or problem retrieving track property
//
//
bool MP4File::GetTrackMediaDataOriginalFormat(MP4TrackId trackId, 
	char *originalFormat, u_int32_t buflen)
{
  u_int32_t format;

  if (buflen < 5)
	return false;

  format = GetTrackIntegerProperty(trackId,
	        "mdia.minf.stbl.stsd.*.sinf.frma.data-format");

  IDATOM(format, originalFormat);
  return true;

}
	     

// track level convenience functions

MP4SampleId MP4File::GetTrackNumberOfSamples(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetNumberOfSamples();
}

const char* MP4File::GetTrackType(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetType();
}

const char *MP4File::GetTrackMediaDataName (MP4TrackId trackId)
{
  MP4Atom *pChild;
  MP4Atom *pAtom = 
    FindAtom(MakeTrackName(trackId, 
			   "mdia.minf.stbl.stsd"));
  if (pAtom->GetNumberOfChildAtoms() != 1) {
    VERBOSE_ERROR(m_verbosity, 
		  fprintf(stderr, "track %d has more than 1 child atoms in stsd\n", trackId));
    return NULL;
  }
  pChild = pAtom->GetChildAtom(0);
  return pChild->GetType();
}
	     

u_int32_t MP4File::GetTrackTimeScale(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetTimeScale();
}

void MP4File::SetTrackTimeScale(MP4TrackId trackId, u_int32_t value)
{
	if (value == 0) {
		throw new MP4Error("invalid value", "SetTrackTimeScale");
	}
	SetTrackIntegerProperty(trackId, "mdia.mdhd.timeScale", value);
}

MP4Duration MP4File::GetTrackDuration(MP4TrackId trackId)
{
	return GetTrackIntegerProperty(trackId, "mdia.mdhd.duration");
}

u_int8_t MP4File::GetTrackEsdsObjectTypeId(MP4TrackId trackId)
{
	// changed mp4a to * to handle enca case
  try {
	return GetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.*.esds.decConfigDescr.objectTypeId");
  } catch (MP4Error *e) {
    delete e;
    return GetTrackIntegerProperty(trackId, 
		"mdia.minf.stbl.stsd.*.*.esds.decConfigDescr.objectTypeId");
  }
}

u_int8_t MP4File::GetTrackAudioMpeg4Type(MP4TrackId trackId)
{
	// verify that track is an MPEG-4 audio track 
	if (GetTrackEsdsObjectTypeId(trackId) != MP4_MPEG4_AUDIO_TYPE) {
		return MP4_MPEG4_INVALID_AUDIO_TYPE;
	}

	u_int8_t* pEsConfig = NULL;
	u_int32_t esConfigSize;

	// The Mpeg4 audio type (AAC, CELP, HXVC, ...)
	// is the first 5 bits of the ES configuration

	GetTrackESConfiguration(trackId, &pEsConfig, &esConfigSize);

	if (esConfigSize < 1) {
	  free(pEsConfig);
		return MP4_MPEG4_INVALID_AUDIO_TYPE;
	}

	u_int8_t mpeg4Type = ((pEsConfig[0] >> 3) & 0x1f);
	// TTTT TXXX XXX  potentially 6 bits of extension.
	if (mpeg4Type == 0x1f) {
	  if (esConfigSize < 2) {
	    free(pEsConfig);
	    return MP4_MPEG4_INVALID_AUDIO_TYPE;
	  }
	  mpeg4Type = 32 + 
	    (((pEsConfig[0] & 0x7) << 3) | ((pEsConfig[1] >> 5) & 0x7));
	}

	free(pEsConfig);

	return mpeg4Type;
}


MP4Duration MP4File::GetTrackFixedSampleDuration(MP4TrackId trackId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetFixedSampleDuration();
}

double MP4File::GetTrackVideoFrameRate(MP4TrackId trackId)
{
	MP4SampleId numSamples =
		GetTrackNumberOfSamples(trackId);
	u_int64_t 
		msDuration =
		ConvertFromTrackDuration(trackId, 
			GetTrackDuration(trackId), MP4_MSECS_TIME_SCALE);

	if (msDuration == 0) {
		return 0.0;
	}

	return ((double)numSamples / UINT64_TO_DOUBLE(msDuration)) * MP4_MSECS_TIME_SCALE;
}

int MP4File::GetTrackAudioChannels (MP4TrackId trackId)
{
  return GetTrackIntegerProperty(trackId, 
				 "mdia.minf.stbl.stsd.*[0].channels");
}

// true if media track encrypted according to ismacryp
bool MP4File::IsIsmaCrypMediaTrack(MP4TrackId trackId)
{
    if (GetTrackIntegerProperty(trackId,
			        "mdia.minf.stbl.stsd.*.sinf.frma.data-format")
	!= (u_int64_t)-1) {
	return true;
    }
    return false;
}


void MP4File::GetTrackESConfiguration(MP4TrackId trackId, 
	u_int8_t** ppConfig, u_int32_t* pConfigSize)
{
  try {
	GetTrackBytesProperty(trackId, 
		"mdia.minf.stbl.stsd.*[0].esds.decConfigDescr.decSpecificInfo[0].info",
		ppConfig, pConfigSize);
  } catch (MP4Error *e) {
    delete e;
	GetTrackBytesProperty(trackId, 
		"mdia.minf.stbl.stsd.*[0].*.esds.decConfigDescr.decSpecificInfo[0].info",
		ppConfig, pConfigSize);
  }
}

void MP4File::GetTrackVideoMetadata(MP4TrackId trackId, 
	u_int8_t** ppConfig, u_int32_t* pConfigSize)
{
	GetTrackBytesProperty(trackId, 
		"mdia.minf.stbl.stsd.*[0].*.metadata",
		ppConfig, pConfigSize);
}

void MP4File::SetTrackESConfiguration(MP4TrackId trackId, 
	const u_int8_t* pConfig, u_int32_t configSize)
{
	// get a handle on the track decoder config descriptor 
	MP4DescriptorProperty* pConfigDescrProperty = NULL;
	if (FindProperty(MakeTrackName(trackId, 
				       "mdia.minf.stbl.stsd.*[0].esds.decConfigDescr.decSpecificInfo"),
			 (MP4Property**)&pConfigDescrProperty) == false ||
	    pConfigDescrProperty == NULL) {
		// probably trackId refers to a hint track
		throw new MP4Error("no such property", "MP4SetTrackESConfiguration");
	}

	// lookup the property to store the configuration
	MP4BytesProperty* pInfoProperty = NULL;
	(void)pConfigDescrProperty->FindProperty("decSpecificInfo[0].info",
						  (MP4Property**)&pInfoProperty);

	// configuration being set for the first time
	if (pInfoProperty == NULL) {
		// need to create a new descriptor to hold it
		MP4Descriptor* pConfigDescr =
			pConfigDescrProperty->AddDescriptor(MP4DecSpecificDescrTag);
		pConfigDescr->Generate();

		(void)pConfigDescrProperty->FindProperty(
			"decSpecificInfo[0].info",
			(MP4Property**)&pInfoProperty);
		ASSERT(pInfoProperty);
	}

	// set the value
	pInfoProperty->SetValue(pConfig, configSize);
}


void MP4File::GetTrackH264SeqPictHeaders (MP4TrackId trackId,
					  uint8_t ***pppSeqHeader,
					  uint32_t **ppSeqHeaderSize,
					  uint8_t ***pppPictHeader,
					  uint32_t **ppPictHeaderSize)
{
  uint32_t count;
  const char *format;
  MP4Atom *avcCAtom;

  *pppSeqHeader = NULL; *pppPictHeader = NULL;
  *ppSeqHeaderSize = NULL; *ppPictHeaderSize = NULL;

  // get 4cc media format - can be avc1 or encv for ismacrypted track
  format = GetTrackMediaDataName (trackId);

  if (!strcasecmp(format, "avc1"))
 	 avcCAtom = FindAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.avc1.avcC"));
  else if (!strcasecmp(format, "encv"))
 	 avcCAtom = FindAtom(MakeTrackName(trackId, "mdia.minf.stbl.stsd.encv.avcC"));
  else 
        // huh?  unknown track format 
	return;

  MP4BitfieldProperty *pSeqCount;
  MP4IntegerProperty *pSeqLen, *pPictCount, *pPictLen;
  MP4BytesProperty *pSeqVal, *pPictVal;

  if ((avcCAtom->FindProperty("avcC.numOfSequenceParameterSets",
			     (MP4Property **)&pSeqCount) == false) ||
      (avcCAtom->FindProperty("avcC.sequenceEntries.sequenceParameterSetLength",
			      (MP4Property **)&pSeqLen) == false) ||
      (avcCAtom->FindProperty("avcC.sequenceEntries.sequenceParameterSetNALUnit",
			      (MP4Property **)&pSeqVal) == false)) {
    VERBOSE_ERROR(m_verbosity, WARNING("Could not find avcC properties"));
    return ;
  }
  uint8_t **ppSeqHeader =
    (uint8_t **)malloc((pSeqCount->GetValue() + 1) * sizeof(uint8_t *));
  if (ppSeqHeader == NULL) return;
  *pppSeqHeader = ppSeqHeader;

  uint32_t *pSeqHeaderSize = 
    (uint32_t *)malloc((pSeqCount->GetValue() + 1) * sizeof(uint32_t *));

  if (pSeqHeaderSize == NULL) return;

  *ppSeqHeaderSize = pSeqHeaderSize;
  for (count = 0; count < pSeqCount->GetValue(); count++) {
    pSeqVal->GetValue(&(ppSeqHeader[count]), &(pSeqHeaderSize[count]),
		      count);
  }
  ppSeqHeader[count] = NULL;
  pSeqHeaderSize[count] = 0;

  if ((avcCAtom->FindProperty("avcC.numOfPictureParameterSets",
			     (MP4Property **)&pPictCount) == false) ||
      (avcCAtom->FindProperty("avcC.pictureEntries.pictureParameterSetLength",
			      (MP4Property **)&pPictLen) == false) ||
      (avcCAtom->FindProperty("avcC.pictureEntries.pictureParameterSetNALUnit",
			      (MP4Property **)&pPictVal) == false)) {
    VERBOSE_ERROR(m_verbosity, 
		  WARNING("Could not find avcC picture table properties"));
    return ;
  }
  uint8_t 
  **ppPictHeader = 
    (uint8_t **)malloc((pPictCount->GetValue() + 1) * sizeof(uint8_t *));
  if (ppPictHeader == NULL) return;
  uint32_t *pPictHeaderSize = 
    (uint32_t *)malloc((pPictCount->GetValue() + 1)* sizeof(uint32_t *));
  if (pPictHeaderSize == NULL) {
    free(ppPictHeader);
    return;
  }
  *pppPictHeader = ppPictHeader;
  *ppPictHeaderSize = pPictHeaderSize;

  for (count = 0; count < pPictCount->GetValue(); count++) {
    pPictVal->GetValue(&(ppPictHeader[count]), &(pPictHeaderSize[count]),
		       count);
  }
  ppPictHeader[count] = NULL;
  pPictHeaderSize[count] = 0;
  return ;
}



const char* MP4File::GetHintTrackSdp(MP4TrackId hintTrackId)
{
	return GetTrackStringProperty(hintTrackId, "udta.hnti.sdp .sdpText");
}

void MP4File::SetHintTrackSdp(MP4TrackId hintTrackId, const char* sdpString)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4SetHintTrackSdp");
	}

	(void)AddDescendantAtoms(
		MakeTrackName(hintTrackId, NULL), "udta.hnti.sdp ");

	SetTrackStringProperty(hintTrackId, "udta.hnti.sdp .sdpText", sdpString);
}

void MP4File::AppendHintTrackSdp(MP4TrackId hintTrackId, 
	const char* sdpFragment)
{
	const char* oldSdpString = NULL;
	try {
		oldSdpString = GetHintTrackSdp(hintTrackId);
	}
	catch (MP4Error* e) {
		delete e;
		SetHintTrackSdp(hintTrackId, sdpFragment);
		return;
	}

	char* newSdpString =
		(char*)MP4Malloc(strlen(oldSdpString) + strlen(sdpFragment) + 1);
	strcpy(newSdpString, oldSdpString);
	strcat(newSdpString, sdpFragment);
	SetHintTrackSdp(hintTrackId, newSdpString);
	MP4Free(newSdpString);
}

void MP4File::GetHintTrackRtpPayload(
	MP4TrackId hintTrackId,
	char** ppPayloadName,
	u_int8_t* pPayloadNumber,
	u_int16_t* pMaxPayloadSize,
	char **ppEncodingParams)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetHintTrackRtpPayload");
	}

	((MP4RtpHintTrack*)pTrack)->GetPayload(
		ppPayloadName, pPayloadNumber, pMaxPayloadSize, ppEncodingParams);
}

void MP4File::SetHintTrackRtpPayload(MP4TrackId hintTrackId,
	const char* payloadName, u_int8_t* pPayloadNumber, u_int16_t maxPayloadSize,
				     const char *encoding_params,
				     bool include_rtp_map,
				     bool include_mpeg4_esid)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4SetHintTrackRtpPayload");
	}

	u_int8_t payloadNumber;
	if (pPayloadNumber && *pPayloadNumber != MP4_SET_DYNAMIC_PAYLOAD) {
		payloadNumber = *pPayloadNumber;
	} else {
		payloadNumber = AllocRtpPayloadNumber();
		if (pPayloadNumber) {
			*pPayloadNumber = payloadNumber;
		}
	}

	((MP4RtpHintTrack*)pTrack)->SetPayload(
		payloadName, payloadNumber, maxPayloadSize, encoding_params,
		include_rtp_map, include_mpeg4_esid);
}

u_int8_t MP4File::AllocRtpPayloadNumber()
{
	MP4Integer32Array usedPayloads;
	u_int32_t i;

	// collect rtp payload numbers in use by existing tracks
	for (i = 0; i < m_pTracks.Size(); i++) {
		MP4Atom* pTrakAtom = m_pTracks[i]->GetTrakAtom();

		MP4Integer32Property* pPayloadProperty = NULL;
		if (pTrakAtom->FindProperty("trak.udta.hinf.payt.payloadNumber",
			(MP4Property**)&pPayloadProperty) &&
		    pPayloadProperty) {
			usedPayloads.Add(pPayloadProperty->GetValue());
		}
	}

	// search dynamic payload range for an available slot
	u_int8_t payload;
	for (payload = 96; payload < 128; payload++) {
		for (i = 0; i < usedPayloads.Size(); i++) {
			if (payload == usedPayloads[i]) {
				break;
			}
		}
		if (i == usedPayloads.Size()) {
			break;
		}
	}

	if (payload >= 128) {
		throw new MP4Error("no more available rtp payload numbers",
			"AllocRtpPayloadNumber");
	}

	return payload;
}

MP4TrackId MP4File::GetHintTrackReferenceTrackId(
	MP4TrackId hintTrackId)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetHintTrackReferenceTrackId");
	}

	MP4Track* pRefTrack = ((MP4RtpHintTrack*)pTrack)->GetRefTrack();

	if (pRefTrack == NULL) {
		return MP4_INVALID_TRACK_ID;
	}
	return pRefTrack->GetId();
}

void MP4File::ReadRtpHint(
	MP4TrackId hintTrackId,
	MP4SampleId hintSampleId,
	u_int16_t* pNumPackets)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", "MP4ReadRtpHint");
	}
	((MP4RtpHintTrack*)pTrack)->
		ReadHint(hintSampleId, pNumPackets);
}

u_int16_t MP4File::GetRtpHintNumberOfPackets(
	MP4TrackId hintTrackId)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetRtpHintNumberOfPackets");
	}
	return ((MP4RtpHintTrack*)pTrack)->GetHintNumberOfPackets();
}

int8_t MP4File::GetRtpPacketBFrame(
	MP4TrackId hintTrackId,
	u_int16_t packetIndex)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetRtpHintBFrame");
	}
	return ((MP4RtpHintTrack*)pTrack)->GetPacketBFrame(packetIndex);
}

int32_t MP4File::GetRtpPacketTransmitOffset(
	MP4TrackId hintTrackId,
	u_int16_t packetIndex)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetRtpPacketTransmitOffset");
	}
	return ((MP4RtpHintTrack*)pTrack)->GetPacketTransmitOffset(packetIndex);
}

void MP4File::ReadRtpPacket(
	MP4TrackId hintTrackId,
	u_int16_t packetIndex,
	u_int8_t** ppBytes, 
	u_int32_t* pNumBytes,
	u_int32_t ssrc,
	bool includeHeader,
	bool includePayload)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", "MP4ReadPacket");
	}
	((MP4RtpHintTrack*)pTrack)->ReadPacket(
		packetIndex, ppBytes, pNumBytes,
		ssrc, includeHeader, includePayload);
}

MP4Timestamp MP4File::GetRtpTimestampStart(
	MP4TrackId hintTrackId)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4GetRtpTimestampStart");
	}
	return ((MP4RtpHintTrack*)pTrack)->GetRtpTimestampStart();
}

void MP4File::SetRtpTimestampStart(
	MP4TrackId hintTrackId,
	MP4Timestamp rtpStart)
{
	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4SetRtpTimestampStart");
	}
	((MP4RtpHintTrack*)pTrack)->SetRtpTimestampStart(rtpStart);
}

void MP4File::AddRtpHint(MP4TrackId hintTrackId, 
	bool isBframe, u_int32_t timestampOffset)
{
	ProtectWriteOperation("MP4AddRtpHint");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", "MP4AddRtpHint");
	}
	((MP4RtpHintTrack*)pTrack)->AddHint(isBframe, timestampOffset);
}

void MP4File::AddRtpPacket(
	MP4TrackId hintTrackId, bool setMbit, int32_t transmitOffset)
{
	ProtectWriteOperation("MP4AddRtpPacket");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", "MP4AddRtpPacket");
	}
	((MP4RtpHintTrack*)pTrack)->AddPacket(setMbit, transmitOffset);
}

void MP4File::AddRtpImmediateData(MP4TrackId hintTrackId, 
	const u_int8_t* pBytes, u_int32_t numBytes)
{
	ProtectWriteOperation("MP4AddRtpImmediateData");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4AddRtpImmediateData");
	}
	((MP4RtpHintTrack*)pTrack)->AddImmediateData(pBytes, numBytes);
}

void MP4File::AddRtpSampleData(MP4TrackId hintTrackId, 
	MP4SampleId sampleId, u_int32_t dataOffset, u_int32_t dataLength)
{
	ProtectWriteOperation("MP4AddRtpSampleData");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4AddRtpSampleData");
	}
	((MP4RtpHintTrack*)pTrack)->AddSampleData(
		sampleId, dataOffset, dataLength);
}

void MP4File::AddRtpESConfigurationPacket(MP4TrackId hintTrackId)
{
	ProtectWriteOperation("MP4AddRtpESConfigurationPacket");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4AddRtpESConfigurationPacket");
	}
	((MP4RtpHintTrack*)pTrack)->AddESConfigurationPacket();
}

void MP4File::WriteRtpHint(MP4TrackId hintTrackId,
	MP4Duration duration, bool isSyncSample)
{
	ProtectWriteOperation("MP4WriteRtpHint");

	MP4Track* pTrack = m_pTracks[FindTrackIndex(hintTrackId)];

	if (strcmp(pTrack->GetType(), MP4_HINT_TRACK_TYPE)) {
		throw new MP4Error("track is not a hint track", 
			"MP4WriteRtpHint");
	}
	((MP4RtpHintTrack*)pTrack)->WriteHint(duration, isSyncSample);
}

u_int64_t MP4File::ConvertFromMovieDuration(
	MP4Duration duration,
	u_int32_t timeScale)
{
	return MP4ConvertTime((u_int64_t)duration, 
		GetTimeScale(), timeScale);
}

u_int64_t MP4File::ConvertFromTrackTimestamp(
	MP4TrackId trackId, 
	MP4Timestamp timeStamp,
	u_int32_t timeScale)
{
	return MP4ConvertTime(timeStamp, 
		GetTrackTimeScale(trackId), timeScale);
}

MP4Timestamp MP4File::ConvertToTrackTimestamp(
	MP4TrackId trackId, 
	u_int64_t timeStamp,
	u_int32_t timeScale)
{
	return (MP4Timestamp)MP4ConvertTime(timeStamp, 
		timeScale, GetTrackTimeScale(trackId));
}

u_int64_t MP4File::ConvertFromTrackDuration(
	MP4TrackId trackId, 
	MP4Duration duration,
	u_int32_t timeScale)
{
	return MP4ConvertTime((u_int64_t)duration, 
		GetTrackTimeScale(trackId), timeScale);
}

MP4Duration MP4File::ConvertToTrackDuration(
	MP4TrackId trackId, 
	u_int64_t duration,
	u_int32_t timeScale)
{
	return (MP4Duration)MP4ConvertTime(duration, 
		timeScale, GetTrackTimeScale(trackId));
}

u_int8_t MP4File::ConvertTrackTypeToStreamType(const char* trackType)
{
	u_int8_t streamType;

	if (!strcmp(trackType, MP4_OD_TRACK_TYPE)) {
		streamType = MP4ObjectDescriptionStreamType;
	} else if (!strcmp(trackType, MP4_SCENE_TRACK_TYPE)) {
		streamType = MP4SceneDescriptionStreamType;
	} else if (!strcmp(trackType, MP4_CLOCK_TRACK_TYPE)) {
		streamType = MP4ClockReferenceStreamType;
	} else if (!strcmp(trackType, MP4_MPEG7_TRACK_TYPE)) {
		streamType = MP4Mpeg7StreamType;
	} else if (!strcmp(trackType, MP4_OCI_TRACK_TYPE)) {
		streamType = MP4OCIStreamType;
	} else if (!strcmp(trackType, MP4_IPMP_TRACK_TYPE)) {
		streamType = MP4IPMPStreamType;
	} else if (!strcmp(trackType, MP4_MPEGJ_TRACK_TYPE)) {
		streamType = MP4MPEGJStreamType;
	} else {
		streamType = MP4UserPrivateStreamType;
	}

	return streamType;
}

// edit list

char* MP4File::MakeTrackEditName(
	MP4TrackId trackId,
	MP4EditId editId,
	const char* name)
{
	char* trakName = MakeTrackName(trackId, NULL);

	if (m_editName == NULL) {
	  m_editName = (char *)malloc(1024);
	  if (m_editName == NULL) return NULL;
	}
	snprintf(m_editName, 1024,
		"%s.edts.elst.entries[%u].%s", 
		trakName, editId - 1, name);
	return m_editName;
}

MP4EditId MP4File::AddTrackEdit(
	MP4TrackId trackId,
	MP4EditId editId)
{
	ProtectWriteOperation("AddTrackEdit");
	return m_pTracks[FindTrackIndex(trackId)]->AddEdit(editId);
}

void MP4File::DeleteTrackEdit(
	MP4TrackId trackId,
	MP4EditId editId)
{
	ProtectWriteOperation("DeleteTrackEdit");
	m_pTracks[FindTrackIndex(trackId)]->DeleteEdit(editId);
}

u_int32_t MP4File::GetTrackNumberOfEdits(
	MP4TrackId trackId)
{
	return GetTrackIntegerProperty(trackId, "edts.elst.entryCount");
}

MP4Duration MP4File::GetTrackEditTotalDuration(
	MP4TrackId trackId,
	MP4EditId editId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetEditTotalDuration(editId);
}

MP4Timestamp MP4File::GetTrackEditStart(
	MP4TrackId trackId,
	MP4EditId editId)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetEditStart(editId);
}

MP4Timestamp MP4File::GetTrackEditMediaStart(
	MP4TrackId trackId,
	MP4EditId editId)
{
	return GetIntegerProperty(
		MakeTrackEditName(trackId, editId, "mediaTime"));
}

void MP4File::SetTrackEditMediaStart(
	MP4TrackId trackId,
	MP4EditId editId,
	MP4Timestamp startTime)
{
	SetIntegerProperty(
		MakeTrackEditName(trackId, editId, "mediaTime"),
		startTime);
}

MP4Duration MP4File::GetTrackEditDuration(
	MP4TrackId trackId,
	MP4EditId editId)
{
	return GetIntegerProperty(
		MakeTrackEditName(trackId, editId, "segmentDuration"));
}

void MP4File::SetTrackEditDuration(
	MP4TrackId trackId,
	MP4EditId editId,
	MP4Duration duration)
{
	SetIntegerProperty(
		MakeTrackEditName(trackId, editId, "segmentDuration"),
		duration);
}

bool MP4File::GetTrackEditDwell(
	MP4TrackId trackId,
	MP4EditId editId)
{
	return (GetIntegerProperty(
		MakeTrackEditName(trackId, editId, "mediaRate")) == 0);
}

void MP4File::SetTrackEditDwell(
	MP4TrackId trackId,
	MP4EditId editId,
	bool dwell)
{
	SetIntegerProperty(
		MakeTrackEditName(trackId, editId, "mediaRate"),
		(dwell ? 0 : 1));
}

MP4SampleId MP4File::GetSampleIdFromEditTime(
	MP4TrackId trackId,
	MP4Timestamp when,
	MP4Timestamp* pStartTime,
	MP4Duration* pDuration)
{
	return m_pTracks[FindTrackIndex(trackId)]->GetSampleIdFromEditTime(
		when, pStartTime, pDuration);
}

