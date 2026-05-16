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

	size_t aFileOffset = 0;
	uint32_t aMagic = 0;

	memcpy(&aMagic, aPakCollection->mData.data() + aFileOffset, sizeof(uint32_t));
	aFileOffset += sizeof(uint32_t);

	if (aMagic != 0xBAC04AC0)
	{
		aMagic ^= 0xF7F7F7F7;
		if (aMagic != 0xBAC04AC0) //if the magic is 0xF7 encrypted, it means the whole file is
		{
			printf("[PakLib] - Invalid Magic Number received in Pak File: %s\n", theFileName.c_str());
			return false;
		}

		printf("[PakLib] - Pak File: %s is encrypted, decrypting now...\n", theFileName.c_str());
		
		for (uint8_t &byte : aPakRecord->mCollection->mData)
			byte ^= 0xF7;
	}

	uint32_t aVersion = 0;
	memcpy(&aVersion, aPakCollection->mData.data() + aFileOffset, sizeof(uint32_t));
	aFileOffset += sizeof(uint32_t);

	if (aVersion > 0)
	{
		printf("[PakLib] - Invalid version received in Pak File: %s\n", theFileName.c_str());
		return false;
	}

	int aPos = 0;

	for (;;)
	{
		uint8_t aFlags = 0;

		memcpy(&aFlags, aPakCollection->mData.data() + aFileOffset, sizeof(uint8_t));
		aFileOffset += sizeof(uint8_t);

		if ((aFlags & FILEFLAGS_END) || (aFileOffset >= aPakCollection->mData.size()))
			break;

		uint8_t aNameWidth = 0;
		char aName[256];

		memcpy(&aNameWidth, aPakCollection->mData.data() + aFileOffset, sizeof(uint8_t));
		aFileOffset += sizeof(uint8_t);

		memcpy(&aName, aPakCollection->mData.data() + aFileOffset, aNameWidth);
		aFileOffset += aNameWidth;

		aName[aNameWidth] = '\0';

		int aSrcSize = 0;

		memcpy(&aSrcSize, aPakCollection->mData.data() + aFileOffset, sizeof(int));
		aFileOffset += sizeof(int);

		int64_t aFileTime;
		memcpy(&aFileTime, aPakCollection->mData.data() + aFileOffset, sizeof(int64_t));
		aFileOffset += sizeof(int64_t);

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

	int anOffset = (int)aFileOffset;

	// Now fix file starts
	aRecordItr = mPakRecordMap.begin();
	while (aRecordItr != mPakRecordMap.end())
	{
		PakRecord *aPakRecord = &(aRecordItr->second);
		if (aPakRecord->mCollection == aPakCollection)
			aPakRecord->mStartPos += anOffset;
		++aRecordItr;
	}

	return true;
}

//0x5D85C0
PFILE *PakInterface::FOpen(const char *theFileName, const char *anAccess)
{
	if (std::filesystem::exists(theFileName))
	{
		FILE *aFP = fopen(theFileName, anAccess);
		if (aFP == NULL)
			return NULL;
		PFILE *aPFP = new PFILE;
		aPFP->mRecord = NULL;
		aPFP->mPos = 0;
		aPFP->mFP = aFP;
		return aPFP;
	}
	else if ((stricmp(anAccess, "r") == 0) || (stricmp(anAccess, "rb") == 0) || (stricmp(anAccess, "rt") == 0))
	{
		char anUpperName[256];
		FixFileName(theFileName, anUpperName);

		PakRecordMap::iterator anItr = mPakRecordMap.find(anUpperName);
		if (anItr != mPakRecordMap.end())
		{
			PFILE *aPFP = new PFILE;
			aPFP->mRecord = &anItr->second;
			aPFP->mPos = 0;
			aPFP->mFP = nullptr;
			return aPFP;
		}
	}
	return nullptr;
}

//0x5D8780
int PakInterface::FClose(PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		fclose(theFile->mFP);
	delete theFile;
	return 0;
}

//0x5D87B0
int PakInterface::FSeek(PFILE *theFile, long theOffset, int theOrigin)
{
	if (theFile->mFP != nullptr)
		return fseek(theFile->mFP, theOffset, theOrigin);
	if (theFile->mRecord != NULL)
	{
		if (theOrigin == SEEK_SET)
			theFile->mPos = theOffset;
		else if (theOrigin == SEEK_END)
			theFile->mPos = theFile->mRecord->mSize - theOffset;
		else if (theOrigin == SEEK_CUR)
			theFile->mPos += theOffset;

		// The current pointer position cannot exceed the entire file size, and cannot be less than 0.
		theFile->mPos = std::max(std::min(theFile->mPos, (long)theFile->mRecord->mSize), 0l);
	}
	return 0;
}

//0x5D8830
long PakInterface::FTell(PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return ftell(theFile->mFP);
	else if (theFile->mRecord != NULL)
		return theFile->mPos;
	return 0;
}

//0x5D8850
size_t PakInterface::FRead(void *thePtr, int theElemSize, int theCount, PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return fread(thePtr, theElemSize, theCount, theFile->mFP);
	else if (theFile->mRecord != NULL)
	{
		// 实际读取的字节数不能超过当前资源文件剩余可读取的字节数
		int aSizeBytes = std::min(1l * theElemSize * theCount, theFile->mRecord->mSize - theFile->mPos);

		// 取得在整个 pak 中开始读取的位置的指针
		uint8_t *src = theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos;
		uint8_t *dest = (uint8_t *)thePtr;
		memcpy(thePtr, src, aSizeBytes);
		theFile->mPos += aSizeBytes;	 // 读取完成后，移动当前读取位置的指针
		return aSizeBytes / theElemSize; // 返回实际读取的项数
	}
	return 0;
}

int PakInterface::FGetC(PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return fgetc(theFile->mFP);
	else if (theFile->mRecord != NULL)
	{
		for (;;)
		{
			if (theFile->mPos >= theFile->mRecord->mSize)
				return EOF;
			char aChar =
				*((char *)theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos++);
			if (aChar != '\r')
				return (uint8_t)aChar;
		}
	}
	return '\0';
}

int PakInterface::UnGetC(int theChar, PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return ungetc(theChar, theFile->mFP);
	else if (theFile->mRecord != NULL)
	{
		// This won't work if we're not pushing the same chars back in the stream
		theFile->mPos = std::max(theFile->mPos - 1, 0l);
	}
	return EOF;
}

char *PakInterface::FGetS(char *thePtr, int theSize, PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return fgets(thePtr, theSize, theFile->mFP);
	else if (theFile->mRecord != NULL)
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
				*((char *)theFile->mRecord->mCollection->mData.data() + theFile->mRecord->mStartPos + theFile->mPos++);
			if (aChar != '\r')
				thePtr[anIdx++] = aChar;
			if (aChar == '\n')
				break;
		}
		thePtr[anIdx] = 0;
		return thePtr;
	}
	return nullptr;
}

int PakInterface::FEof(PFILE *theFile)
{
	if (theFile->mFP != nullptr)
		return feof(theFile->mFP);
	else if (theFile->mRecord != NULL)
		return theFile->mPos >= theFile->mRecord->mSize;
	return true;
}
