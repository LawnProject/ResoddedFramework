#include "PakInterface.h"
#include <fstream>
#include <vector>
#include <cinttypes>

#if __clang__
#define stricmp _stricmp
#endif

enum
{
	FILEFLAGS_END = 0x80
};

PakInterface *gPakInterface = new PakInterface();

static std::string StringToUpper(const std::string &theString)
{
	std::string aString;

	for (unsigned i = 0; i < theString.length(); i++)
		aString += toupper(theString[i]);

	return aString;
}

PakInterface::PakInterface()
{
	if (gPakInterface == NULL)
		gPakInterface = this;
}

PakInterface::~PakInterface()
{
}


static void FixFileName(const char *theFileName, char *theUpperName)
{
	if ((theFileName[0] != 0) && (theFileName[1] == ':'))
	{
		char aDir[256];
		std::string aWorkingDir = std::filesystem::current_path().string();
		std::snprintf(aDir, sizeof(aDir), "%s", aWorkingDir.c_str());
		int aLen = strlen(aDir);
		aDir[aLen++] = '/';
		aDir[aLen] = 0;

		if (strnicmp(aDir, theFileName, aLen) == 0)
			theFileName += aLen;
	}

	bool lastSlash = false;
	const char *aSrc = theFileName;
	char *aDest = theUpperName;

	for (;;)
	{
		char c = *(aSrc++);

		if ((c == '\\') || (c == '/'))
		{
			if (!lastSlash)
				*(aDest++) = '/';
			lastSlash = true;
		}
		else if ((c == '.') && (lastSlash) && (*aSrc == '.'))
		{
			// We have a '/..' on our hands
			aDest--;
			while ((aDest > theUpperName + 1) && (*(aDest - 1) != '\\'))
				--aDest;
			aSrc++;
		}
		else
		{
			*(aDest++) = toupper((uint8_t)c);
			if (c == 0)
				break;
			lastSlash = false;
		}
	}
}


bool PakInterface::AddPakFile(const std::string &theFileName)
{
	std::ifstream aFile(theFileName, std::ios::binary | std::ios::ate);
	if (!aFile)
		return false;

	int aFileSize = aFile.tellg();
	aFile.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(aFileSize);
	if (!aFile.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(aFileSize)))
		return false;

	mPakCollectionList.push_back(PakCollection());
	PakCollection *aPakCollection = &mPakCollectionList.back();

    aPakCollection->mData = std::move(buffer);

	PakRecordMap::iterator aRecordItr =
		mPakRecordMap.insert(PakRecordMap::value_type(StringToUpper(theFileName), PakRecord())).first;
	PakRecord *aPakRecord = &(aRecordItr->second);
	aPakRecord->mCollection = aPakCollection;
	aPakRecord->mFileName = theFileName;
	aPakRecord->mStartPos = 0;
	aPakRecord->mSize = aFileSize;

	PFILE *aFP = FOpen(theFileName.c_str(), "rb");
	if (aFP == NULL)
		return false;

	uint32_t aMagic = 0;
	FRead(&aMagic, sizeof(uint32_t), 1, aFP);
	if (aMagic != 0xBAC04AC0)
	{
		FClose(aFP);
		return false;
	}

	uint32_t aVersion = 0;
	FRead(&aVersion, sizeof(uint32_t), 1, aFP);
	if (aVersion > 0)
	{
		FClose(aFP);
		return false;
	}

	int aPos = 0;

	for (;;)
	{
		uint8_t aFlags = 0;
		int aCount = FRead(&aFlags, 1, 1, aFP);
		if ((aFlags & FILEFLAGS_END) || (aCount == 0))
			break;

		uint8_t aNameWidth = 0;
		char aName[256];
		FRead(&aNameWidth, 1, 1, aFP);
		FRead(aName, 1, aNameWidth, aFP);
		aName[aNameWidth] = 0;

		int aSrcSize = 0;
		FRead(&aSrcSize, sizeof(int), 1, aFP);
		int64_t aFileTime;
		FRead(&aFileTime, sizeof(int64_t), 1, aFP);
		
		for (int i = 0; i < aNameWidth; i++) //windows....
		{
			if (aName[i] == '\\')
				aName[i] = '/';
		}
		char anUpperName[256];
		FixFileName(aName, anUpperName);

		PakRecordMap::iterator aRecordItr =
			mPakRecordMap.insert(PakRecordMap::value_type(StringToUpper(aName), PakRecord())).first;
		PakRecord *aPakRecord = &(aRecordItr->second);
		aPakRecord->mCollection = aPakCollection;
		aPakRecord->mFileName = anUpperName;
		aPakRecord->mStartPos = aPos;
		aPakRecord->mSize = aSrcSize;
		aPakRecord->mFileTime = aFileTime;

		aPos += aSrcSize;
	}

	int anOffset = FTell(aFP);

	// Now fix file starts
	aRecordItr = mPakRecordMap.begin();
	while (aRecordItr != mPakRecordMap.end())
	{
		PakRecord *aPakRecord = &(aRecordItr->second);
		if (aPakRecord->mCollection == aPakCollection)
			aPakRecord->mStartPos += anOffset;
		++aRecordItr;
	}

	FClose(aFP);

	return true;
}

//0x5D85C0
PFILE *PakInterface::FOpen(const char *theFileName, const char *anAccess)
{
	if ((stricmp(anAccess, "r") == 0) || (stricmp(anAccess, "rb") == 0) || (stricmp(anAccess, "rt") == 0))
	{
		char anUpperName[256];
		FixFileName(theFileName, anUpperName);

		PakRecordMap::iterator anItr = mPakRecordMap.find(anUpperName);
		if (anItr != mPakRecordMap.end())
		{
			PFILE *aPFP = new PFILE;
			aPFP->mRecord = &anItr->second;
			aPFP->mPos = 0;
			aPFP->mFP = NULL;
			return aPFP;
		}
	}

	FILE *aFP = fopen(theFileName, anAccess);
	if (aFP == NULL)
		return NULL;
	PFILE *aPFP = new PFILE;
	aPFP->mRecord = NULL;
	aPFP->mPos = 0;
	aPFP->mFP = aFP;
	return aPFP;
}

//0x5D8780
int PakInterface::FClose(PFILE *theFile)
{
	if (theFile->mRecord == NULL)
		fclose(theFile->mFP);
	delete theFile;
	return 0;
}

//0x5D87B0
int PakInterface::FSeek(PFILE *theFile, long theOffset, int theOrigin)
{
	if (theFile->mRecord != NULL)
	{
		if (theOrigin == SEEK_SET)
			theFile->mPos = theOffset;
		else if (theOrigin == SEEK_END)
			theFile->mPos = theFile->mRecord->mSize - theOffset;
		else if (theOrigin == SEEK_CUR)
			theFile->mPos += theOffset;

		// 当前指针位置不能超过整个文件的大小，且不能小于 0
		theFile->mPos = std::max(std::min(theFile->mPos, (long)theFile->mRecord->mSize), 0l);
		return 0;
	}
	else
		return fseek(theFile->mFP, theOffset, theOrigin);
}

//0x5D8830
long PakInterface::FTell(PFILE *theFile)
{
	if (theFile->mRecord != NULL)
		return theFile->mPos;
	else
		return ftell(theFile->mFP);
}

//0x5D8850
size_t PakInterface::FRead(void *thePtr, int theElemSize, int theCount, PFILE *theFile)
{
	if (theFile->mRecord != NULL)
	{
		// 实际读取的字节数不能超过当前资源文件剩余可读取的字节数
		int aSizeBytes = std::min(1l * theElemSize * theCount, theFile->mRecord->mSize - theFile->mPos);

		// 取得在整个 pak 中开始读取的位置的指针
		uint8_t *src = theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos;
		uint8_t *dest = (uint8_t *)thePtr;
		for (int i = 0; i < aSizeBytes; i++)
			*(dest++) = (*src++) ^ 0xF7; // 'Decrypt'
		theFile->mPos += aSizeBytes;	 // 读取完成后，移动当前读取位置的指针
		return aSizeBytes / theElemSize; // 返回实际读取的项数
	}

	return fread(thePtr, theElemSize, theCount, theFile->mFP);
}

int PakInterface::FGetC(PFILE *theFile)
{
	if (theFile->mRecord != NULL)
	{
		for (;;)
		{
			if (theFile->mPos >= theFile->mRecord->mSize)
				return EOF;
			char aChar =
				*((char *)theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos++) ^
				0xF7;
			if (aChar != '\r')
				return (uint8_t)aChar;
		}
	}

	return fgetc(theFile->mFP);
}

int PakInterface::UnGetC(int theChar, PFILE *theFile)
{
	if (theFile->mRecord != NULL)
	{
		// This won't work if we're not pushing the same chars back in the stream
		theFile->mPos = std::max(theFile->mPos - 1, 0l);
		return theChar;
	}

	return ungetc(theChar, theFile->mFP);
}

char *PakInterface::FGetS(char *thePtr, int theSize, PFILE *theFile)
{
	if (theFile->mRecord != NULL)
	{
		int anIdx = 0;
		while (anIdx < theSize)
		{
			if (theFile->mPos >= theFile->mRecord->mSize)
			{
				if (anIdx == 0)
					return NULL;
				break;
			}
			char aChar =
				*((char *)theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos++) ^
				0xF7;
			if (aChar != '\r')
				thePtr[anIdx++] = aChar;
			if (aChar == '\n')
				break;
		}
		thePtr[anIdx] = 0;
		return thePtr;
	}

	return fgets(thePtr, theSize, theFile->mFP);
}

int PakInterface::FEof(PFILE *theFile)
{
	if (theFile->mRecord != NULL)
		return theFile->mPos >= theFile->mRecord->mSize;
	else
		return feof(theFile->mFP);
}
