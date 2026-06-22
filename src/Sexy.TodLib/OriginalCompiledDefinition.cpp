#if SEXY_IS_X86

#include "OriginalCompiledDefinition.h"
#include "../PakLib/PakInterface.h"
#include "Definition.h"
#include "../SexyAppFramework/PerfTimer.h"
#include <zlib.h>

bool LegacyDefinition::DefReadFromCacheArray(void *&theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap)
{
	int aDefSize;
	SMemR(theReadPtr, &aDefSize, sizeof(int)); // First read an integer representing the size of the definition struct class described by theDefMap
	if (aDefSize != theDefMap->mDefSize)       // Compare it with the size declared by the currently given definition struct map to check whether they match
	{
		TodTrace("[TodLib] - cache has old def: array size");
		return false;
	}
	if (theArray->mArrayCount == 0) // If there are no instances in the class, there is no need to read
		return true;

	int aArraySize = aDefSize * theArray->mArrayCount;
	void *pData = DefinitionAlloc(aArraySize); // Allocate memory and initialize it to 0
	theArray->mArrayData = pData;
	SMemR(theReadPtr, pData, aArraySize); // Still a rough read of all the data, then fix the pointers according to theDefMap's struct field array
	for (int i = 0; i < theArray->mArrayCount; i++)
		if (!LegacyDefinition::DefMapReadFromCache(
		        theReadPtr, theDefMap, (void *)((int)pData + theDefMap->mDefSize * i))) // The last argument represents pData[i]
			return false;
	return true;
}

bool LegacyDefinition::DefReadFromCacheFloatTrack(void *&theReadPtr, FloatParameterTrack *theTrack)
{
	int &aCountNodes = theTrack->mCountNodes;
	SMemR(theReadPtr, &aCountNodes, sizeof(int));
	if (aCountNodes > 0)
	{
		int aSize = aCountNodes * sizeof(FloatParameterTrackNode);
		FloatParameterTrackNode *aPtr = (FloatParameterTrackNode *)DefinitionAlloc(aSize);
		theTrack->mNodes = aPtr;
		SMemR(theReadPtr, aPtr, aSize);
	}
	return true;
}

bool LegacyDefinition::DefReadFromCacheString(void *&theReadPtr, const char **theString)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));
	TOD_ASSERT(aLen >= 0 && aLen <= 100000);
	if (aLen == 0)
		*theString = "";
	else
	{
		char *aPtr = (char *)DefinitionAlloc(aLen + 1);
		*theString = aPtr;
		SMemR(theReadPtr, aPtr, aLen);
		aPtr[aLen] = '\0';
	}
	return true;
}

bool LegacyDefinition::DefReadFromCacheImage(void *&theReadPtr, Image **theImage)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));        // Read the length of the image label character array
	char *aImageName = (char *)_alloca(aLen + 1); // Allocate memory space for the image label character array on the stack
	SMemR(theReadPtr, aImageName, aLen);          // Read the image label character array
	aImageName[aLen] = '\0';

	*theImage = nullptr;
	return aImageName[0] == '\0' || DefinitionLoadImage(theImage, aImageName);
}

bool LegacyDefinition::DefReadFromCacheFont(void *&theReadPtr, Font **theFont)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));       // Read the length of the font label character array
	char *aFontName = (char *)_alloca(aLen + 1); // Allocate memory space for the font label character array on the stack
	SMemR(theReadPtr, aFontName, aLen);          // Read the font label character array
	aFontName[aLen] = '\0';

	*theFont = nullptr;
	return aFontName[0] == '\0' || DefinitionLoadFont(theFont, aFontName);
}

bool LegacyDefinition::DefMapReadFromCache(void *&theReadPtr, DefMap *theDefMap, void *theDefinition)
{
	// Confirm each member variable one by one, and fix the pointer-type and flag-type variables among them
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		bool aSucceed = true;
		void *aDest = (void *)((int)theDefinition + aField->mFieldOffset); // Pointer to this member variable
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_STRING:
			aSucceed = LegacyDefinition::DefReadFromCacheString(theReadPtr, (const char **)aDest);
			break;
		case DefFieldType::DT_ARRAY:
			aSucceed = LegacyDefinition::DefReadFromCacheArray(theReadPtr, (DefinitionArrayDef *)aDest, (DefMap *)aField->mExtraData);
			break;
		case DefFieldType::DT_IMAGE:
			aSucceed = LegacyDefinition::DefReadFromCacheImage(theReadPtr, (Image **)aDest);
			break;
		case DefFieldType::DT_FONT:
			aSucceed = LegacyDefinition::DefReadFromCacheFont(theReadPtr, (Font **)aDest);
			break;
		case DefFieldType::DT_TRACK_FLOAT:
			aSucceed = LegacyDefinition::DefReadFromCacheFloatTrack(theReadPtr, (FloatParameterTrack *)aDest);
			break;
		}

		if (!aSucceed)
			return false;
	}
	return true;
}

uint32_t LegacyDefinition::DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol *theSymbolMap)
{
	while (theSymbolMap->mSymbolName != nullptr)
	{
		aSchemaHash = crc32(aSchemaHash, (const Bytef *)theSymbolMap->mSymbolName, strlen(theSymbolMap->mSymbolName));
		aSchemaHash = crc32(aSchemaHash, (const Bytef *)&theSymbolMap->mSymbolValue, sizeof(int));
		theSymbolMap++;
	}
	return aSchemaHash;
}

uint32_t LegacyDefinition::DefinitionCalcHashDefMap(int aSchemaHash,
                                                    DefMap *theDefMap,
                                                    TodList<DefMap *> &theProgressMaps)
{
	for (TodListNode<DefMap *> *aNode = theProgressMaps.mHead; aNode != nullptr; aNode = aNode->mNext)
		if (aNode->mValue == theDefMap)
			return aSchemaHash;
	theProgressMaps.AddTail(theDefMap);

	aSchemaHash = crc32(aSchemaHash, (Bytef *)&theDefMap->mDefSize, sizeof(int));
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		aSchemaHash = crc32(aSchemaHash, (Bytef *)&aField->mFieldType, sizeof(DefFieldType));
		aSchemaHash = crc32(aSchemaHash, (Bytef *)&aField->mFieldOffset, sizeof(int));
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_ENUM:
		case DefFieldType::DT_FLAGS:
			aSchemaHash = LegacyDefinition::DefinitionCalcHashSymbolMap(aSchemaHash, (DefSymbol *)aField->mExtraData);
			break;
		case DefFieldType::DT_ARRAY:
			aSchemaHash = LegacyDefinition::DefinitionCalcHashDefMap(aSchemaHash, (DefMap *)aField->mExtraData, theProgressMaps);
			break;
		}
	}
	return aSchemaHash;
}

uint32_t LegacyDefinition::DefinitionCalcHash(DefMap *theDefMap)
{
	TodList<DefMap *> aProgressMaps;
	uint32_t aResult =
	    LegacyDefinition::DefinitionCalcHashDefMap(crc32(0L, (Bytef *)Z_NULL, NULL) + 1, theDefMap, aProgressMaps);
	return aResult;
}

bool LegacyDefinition::DefinitionReadCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition)
{
	PerfTimer aTimer;
	aTimer.Start();
	PFILE *pFile = p_fopen(theCompiledFilePath.c_str(), "rb");
	if (pFile)
	{
		p_fseek(pFile, 0, 2);                    // Move the pointer to the read location to the end of the file
		size_t aCompressedSize = p_ftell(pFile); // The offset obtained at this time is the size of the entire file
		p_fseek(pFile, 0, 0);                    // Then move the pointer to the read position back to the beginning of the file
		void *aCompressedBuffer = DefinitionAlloc(aCompressedSize);
		// Read the file, and determine whether the actual read size is the complete file size, if it is not equal, it is determined that the read failed
		bool aReadCompressedFailed =
		    p_fread(aCompressedBuffer, sizeof(char), aCompressedSize, pFile) != aCompressedSize;
		p_fclose(pFile);           // Close the resource file stream and free up the memory occupied by pFile
		if (aReadCompressedFailed) // Determine whether the reading is successful
		{
			TodTrace("[TodLib] - Failed to read compiled file: %s\n", theCompiledFilePath.c_str());
			DefinitionFree(aCompressedBuffer);
		}
		else
		{
			size_t aUncompressedSize;
			void *aUncompressedBuffer = LegacyDefinition::DefinitionUncompressCompiledBuffer(
			    aCompressedBuffer, aCompressedSize, aUncompressedSize, theCompiledFilePath);
			DefinitionFree(aCompressedBuffer);
			if (aUncompressedBuffer)
			{
				uint32_t aDefHash = LegacyDefinition::DefinitionCalcHash(
				    theDefMap); // Calculate the CRC check value, which will be used to detect the integrity of the data
				if (aUncompressedSize <
				    theDefMap->mDefSize +
				        sizeof(
				            uint32_t)) // Detect whether the length of the decompressed data is sufficient for the length of "define data + a check value to record data"
					TodTrace("[TodLib] - Compiled file size too small: %s\n", theCompiledFilePath.c_str());
				else
				{
					// A pointer to copy a copy of the decompressed data is used to move when reading, and the original pointer will be used to calculate the size of the read area and delete[] operations in the future.
					void *aBufferPtr = aUncompressedBuffer;
					uint32_t aCashHash;
					SMemR(aBufferPtr, &aCashHash, sizeof(uint32_t)); //Read the CRC check value of the record
					if (aCashHash !=
					    aDefHash) // Determine whether the check value is consistent, if it is inconsistent, the data is wrong
						TodTrace("[TodLib] - Compiled file schema wrong: %s\n", theCompiledFilePath.c_str());
					else
					{
						//  Officially started reading definition data
						// Roughly read the definition data of the original type of theDefinition for the first time, and gulp all the recorded data into theDefinition.
						// At this time, all the data of theDefinition's original non-pointer type will be read correctly, but the variables of its pointer type will be read and assigned as wild pointers.
						// The problem of these wild pointers will be fixed in DefMapReadFromCache() with the help of the corresponding DefField's mExtraData in the future
						SMemR(aBufferPtr, theDefinition, theDefMap->mDefSize);
						// Repair the wild pointer and flag data, and save the result of whether it is successful, and use it as the return value later
						bool aResult = LegacyDefinition::DefMapReadFromCache(aBufferPtr, theDefMap, theDefinition);
						size_t aReadMemSize = (uint32_t)aBufferPtr - (uint32_t)aUncompressedBuffer;
						DefinitionFree(aUncompressedBuffer);
						if (aResult && aReadMemSize != aUncompressedSize)
							TodTrace("[TodLib] - Compiled file wrong size: %s\n", theCompiledFilePath.c_str());
						return aResult;
					}
				}
			}
			DefinitionFree(aUncompressedBuffer);
		}
	}
	return false;
}

void *LegacyDefinition::DefinitionUncompressCompiledBuffer(void *theCompressedBuffer,
                                                           size_t theCompressedBufferSize,
                                                           size_t &theUncompressedSize,
                                                           const SexyString &theCompiledFilePath)
{
	auto sz = theCompressedBufferSize;
	// theCompressedBuffer The first two four-byte bytes contain special data, and this check verifies whether its length is sufficient for 8 bytes (i.e., 2 four-byte bytes).
	if (theCompressedBufferSize < 8)
	{
		TodTrace("[TodLib] - Compile def too small", theCompiledFilePath.c_str());
		return nullptr;
	}
	// Treat the first two four-byte words of theCompressedBuffer as a CompressedDefinitionHeader
	LegacyDefinition::CompressedDefinitionHeader *aHeader = (CompressedDefinitionHeader *)theCompressedBuffer;
	if (aHeader->mCookie != COMPILED_LEGACY_DEFINITION_MAGIC)
	{
		TodTrace("[TodLib] - Compiled fire cookie wrong: %s\n", theCompiledFilePath.c_str());
		return nullptr;
	}
	Bytef *aUncompressedBuffer = (Bytef *)DefinitionAlloc(aHeader->mUncompressedSize);
	theCompressedBufferSize = aHeader->mUncompressedSize; //my addition
	Bytef *aSrc = (Bytef *)theCompressedBuffer +
	              sizeof(LegacyDefinition::CompressedDefinitionHeader); // The actual data to decompress starts at the 3rd four-byte word
	int aResult = uncompress(aUncompressedBuffer,
	                         (uLongf *)&theCompressedBufferSize,
	                         aSrc,
	                         sz - sizeof(LegacyDefinition::CompressedDefinitionHeader));
	TOD_ASSERT(aResult == Z_OK);
	TOD_ASSERT(theCompressedBufferSize == aHeader->mUncompressedSize);
	theUncompressedSize = aHeader->mUncompressedSize;
	return aUncompressedBuffer;
}

#endif
