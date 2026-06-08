#ifndef __PAKINTERFACE_H__
#define __PAKINTERFACE_H__

#include <vector>
#include <filesystem>
#include <map>
#include <list>
#include <string>


class PakCollection;

class PakRecord
{
  public:
	PakCollection *mCollection;
	std::string mFileName;
	int64_t mFileTime;
	int mStartPos;
	int mSize;
};

typedef std::map<std::string, PakRecord> PakRecordMap;

class PakCollection
{
  public:
	std::vector<uint8_t> mData;

	const uint8_t *data() const
	{
		return mData.data();
	}
	uint8_t *data()
	{
		return mData.data();
	}

	size_t size() const
	{
		return mData.size();
	}
};

typedef std::list<PakCollection> PakCollectionList;

struct PFILE
{
	PakRecord *mRecord;
	long mPos;
	FILE *mFP;
};

struct PFindData
{
	std::filesystem::directory_iterator it;
	std::filesystem::directory_iterator end;
	std::string pattern;
};

class PakInterfaceBase
{
  public:
	virtual PFILE *FOpen(const char *theFileName, const char *theAccess) = 0;
	virtual PFILE *FOpen(const wchar_t *theFileName, const wchar_t *theAccess)
	{
		return NULL;
	}
	virtual int FClose(PFILE *theFile) = 0;
	virtual int FSeek(PFILE *theFile, long theOffset, int theOrigin) = 0;
	virtual long FTell(PFILE *theFile) = 0;
	virtual size_t FRead(void *thePtr, int theElemSize, int theCount, PFILE *theFile) = 0;
	virtual int FGetC(PFILE *theFile) = 0;
	virtual int UnGetC(int theChar, PFILE *theFile) = 0;
	virtual char *FGetS(char *thePtr, int theSize, PFILE *theFile) = 0;
	virtual wchar_t *FGetS(wchar_t *thePtr, int theSize, PFILE *theFile)
	{
		return thePtr;
	}
	virtual int FEof(PFILE *theFile) = 0;
};

class PakInterface : public PakInterfaceBase
{
  public:
	PakCollectionList mPakCollectionList;
	PakRecordMap mPakRecordMap;

	std::string mDecryptPassword;

  public:
	PakInterface();
	~PakInterface();

	bool AddPakFile(const std::string &theFileName);
	PFILE *FOpen(const char *theFileName, const char *theAccess);
	int FClose(PFILE *theFile);
	int FSeek(PFILE *theFile, long theOffset, int theOrigin);
	long FTell(PFILE *theFile);
	size_t FRead(void *thePtr, int theElemSize, int theCount, PFILE *theFile);
	int FGetC(PFILE *theFile);
	int UnGetC(int theChar, PFILE *theFile);
	char *FGetS(char *thePtr, int theSize, PFILE *theFile);
	int FEof(PFILE *theFile);

	PFindData FindFirstFile(const std::string &pattern);
	bool FindNextFile(PFindData &fd, std::string &outName);
	bool FindClose(PFindData &fd);
};

extern PakInterface *gPakInterface;

static PFILE *p_fopen(const char *theFileName, const char *theAccess)
{
	if (gPakInterface != NULL)
		return gPakInterface->FOpen(theFileName, theAccess);
	FILE *aFP = fopen(theFileName, theAccess);
	if (aFP == NULL)
		return NULL;
	PFILE *aPFile = new PFILE();
	aPFile->mRecord = NULL;
	aPFile->mPos = 0;
	aPFile->mFP = aFP;
	return aPFile;
}

static int p_fclose(PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FClose(theFile);
	int aResult = fclose(theFile->mFP);
	delete theFile;
	return aResult;
}

static int p_fseek(PFILE *theFile, long theOffset, int theOrigin)
{
	if (gPakInterface != NULL)
		return gPakInterface->FSeek(theFile, theOffset, theOrigin);
	return fseek(theFile->mFP, theOffset, theOrigin);
}

static long p_ftell(PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FTell(theFile);
	return ftell(theFile->mFP);
}

static size_t p_fread(void *thePtr, int theSize, int theCount, PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FRead(thePtr, theSize, theCount, theFile);
	return fread(thePtr, theSize, theCount, theFile->mFP);
}

static size_t p_fwrite(const void *thePtr, int theSize, int theCount, PFILE *theFile)
{
	if (theFile->mFP == NULL)
		return 0;
	return fwrite(thePtr, theSize, theCount, theFile->mFP);
}

static int p_fgetc(PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FGetC(theFile);
	return fgetc(theFile->mFP);
}

static int p_ungetc(int theChar, PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->UnGetC(theChar, theFile);
	return ungetc(theChar, theFile->mFP);
}

static char *p_fgets(char *thePtr, int theSize, PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FGetS(thePtr, theSize, theFile);
	return fgets(thePtr, theSize, theFile->mFP);
}

static int p_feof(PFILE *theFile)
{
	if (gPakInterface != NULL)
		return gPakInterface->FEof(theFile);
	return feof(theFile->mFP);
}
#endif //__PAKINTERFACE_H__
