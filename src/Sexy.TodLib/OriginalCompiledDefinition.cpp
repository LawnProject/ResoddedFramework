

#include "OriginalCompiledDefinition.h"
#include "../PakLib/PakInterface.h"
#include "Definition.h"
#include "../SexyAppFramework/PerfTimer.h"
#include <zlib.h>

bool LegacyDefinition::DefReadFromCacheArray(void *&theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap)
{
	int aDefSize;
	SMemR(theReadPtr, &aDefSize, sizeof(int)); // Size of the structure on x86

	if (theArray->mArrayCount == 0)
		return true;

	int aTargetArraySize = theDefMap->mDefSize * theArray->mArrayCount;
	void *pData = DefinitionAlloc(aTargetArraySize);
	theArray->mArrayData = pData;

	// 1. Read all fixed x86 structures at once
	int aTotalX86Size = aDefSize * theArray->mArrayCount;
	void *aX86Buffer = _alloca(aTotalX86Size);
	SMemR(theReadPtr, aX86Buffer, aTotalX86Size);

	// 2. Iterate and map them, which also reads their dynamic data
	for (int i = 0; i < theArray->mArrayCount; i++)
	{
		void *aElementDest = (void *)((uintptr_t)pData + (uintptr_t)theDefMap->mDefSize * i);
		void *aElementX86Src = (void *)((uintptr_t)aX86Buffer + aDefSize * i);
		if (!LegacyDefinition::DefMapReadFromCache(theReadPtr, theDefMap, aElementDest, aElementX86Src))
			return false;
	}
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

bool LegacyDefinition::DefReadFromCacheString(void *&theReadPtr, char **theString)
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
	SMemR(theReadPtr, &aLen, sizeof(int));		  // 读取贴图标签字符数组的长度
	char *aImageName = (char *)_alloca(aLen + 1); // 在栈上分配贴图标签字符数组的内存空间
	SMemR(theReadPtr, aImageName, aLen);		  // 读取贴图标签字符数组
	aImageName[aLen] = '\0';

	*theImage = nullptr;
	return aImageName[0] == '\0' || DefinitionLoadImage(theImage, aImageName);
}

bool LegacyDefinition::DefReadFromCacheFont(void *&theReadPtr, Font **theFont)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));		 // 读取字体标签字符数组的长度
	char *aFontName = (char *)_alloca(aLen + 1); // 在栈上分配字体标签字符数组的内存空间
	SMemR(theReadPtr, aFontName, aLen);			 // 读取字体标签字符数组
	aFontName[aLen] = '\0';

	*theFont = nullptr;
	return aFontName[0] == '\0' || DefinitionLoadFont(theFont, aFontName);
}

bool LegacyDefinition::DefMapReadFromCache(void *&theReadPtr, DefMap *theDefMap, void *theDefinition,
										   void *theX86Buffer)
{
	// Map fields from x86 buffer to x64 structure.
	int aCurrentX86Offset = 0;
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		void *aDest = (void *)((uintptr_t)theDefinition + aField->mFieldOffset);
		void *aSrc = (void *)((uintptr_t)theX86Buffer + aCurrentX86Offset);

		bool aSucceed = true;
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_INT:
		case DefFieldType::DT_ENUM:
		case DefFieldType::DT_FLAGS:
			memcpy(aDest, aSrc, 4);
			aCurrentX86Offset += 4;
			break;
		case DefFieldType::DT_FLOAT:
			memcpy(aDest, aSrc, 4);
			aCurrentX86Offset += 4;
			break;
		case DefFieldType::DT_VECTOR2:
			memcpy(aDest, aSrc, 8);
			aCurrentX86Offset += 8;
			break;
		case DefFieldType::DT_STRING:
			aSucceed = LegacyDefinition::DefReadFromCacheString(theReadPtr, (char **)aDest);
			aCurrentX86Offset += 4; // x86 pointer
			break;
		case DefFieldType::DT_ARRAY: {
			// Read the count from the x86 buffer (it was at offset 4 after the pointer)
			int aArrayCount = *(int *)((uintptr_t)aSrc + 4);
			if (aArrayCount < 0 || aArrayCount > 1000000)
				return false;
			((DefinitionArrayDef *)aDest)->mArrayCount = aArrayCount;
			aSucceed = LegacyDefinition::DefReadFromCacheArray(theReadPtr, (DefinitionArrayDef *)aDest,
															   (DefMap *)aField->mExtraData);
			aCurrentX86Offset += 8; // x86 pointer + count
			break;
		}
		case DefFieldType::DT_IMAGE:
			aSucceed = LegacyDefinition::DefReadFromCacheImage(theReadPtr, (Image **)aDest);
			aCurrentX86Offset += 4;
			break;
		case DefFieldType::DT_FONT:
			aSucceed = LegacyDefinition::DefReadFromCacheFont(theReadPtr, (Font **)aDest);
			aCurrentX86Offset += 4;
			break;
		case DefFieldType::DT_TRACK_FLOAT:
			aSucceed = LegacyDefinition::DefReadFromCacheFloatTrack(theReadPtr, (FloatParameterTrack *)aDest);
			aCurrentX86Offset += 4;
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

uint32_t LegacyDefinition::DefinitionCalcHashDefMap(int aSchemaHash, DefMap *theDefMap,
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
			aSchemaHash =
				LegacyDefinition::DefinitionCalcHashDefMap(aSchemaHash, (DefMap *)aField->mExtraData, theProgressMaps);
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

bool LegacyDefinition::DefinitionReadCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap,
												  void *theDefinition)
{
	PerfTimer aTimer;
	aTimer.Start();
	PFILE *pFile = p_fopen(theCompiledFilePath.c_str(), "rb");
	if (pFile)
	{
		p_fseek(pFile, 0, 2);					 // Move the pointer to the read location to the end of the file
		size_t aCompressedSize = p_ftell(pFile); // The offset obtained at this time is the size of the entire file
		p_fseek(pFile, 0, 0); // Then move the pointer to the read position back to the beginning of the file
		void *aCompressedBuffer = DefinitionAlloc(aCompressedSize);
		// Read the file, and determine whether the actual read size is the complete file size, if it is not equal, it is determined that the read failed
		bool aReadCompressedFailed =
			p_fread(aCompressedBuffer, sizeof(char), aCompressedSize, pFile) != aCompressedSize;
		p_fclose(pFile);		   // Close the resource file stream and free up the memory occupied by pFile
		if (aReadCompressedFailed) // Determine whether the reading is successful
		{
			TodTrace("Failed to read compiled file: %s\n", theCompiledFilePath.c_str());
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
					TodTrace("Compiled file size too small: %s\n", theCompiledFilePath.c_str());
				else
				{
					// A pointer to copy a copy of the decompressed data is used to move when reading, and the original pointer will be used to calculate the size of the read area and delete[] operations in the future.
					void *aBufferPtr = aUncompressedBuffer;
					uint32_t aCashHash;
					SMemR(aBufferPtr, &aCashHash, sizeof(uint32_t)); //Read the CRC check value of the record
					if (false && aCashHash != aDefHash)				 // Bypassed for x64 compatibility
						TodTrace("Compiled file schema wrong: %s\n", theCompiledFilePath.c_str());
					else
					{
						// Calculate root x86 size
						int aX86Size = 0;
						for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
						{
							if (aField->mFieldType == DefFieldType::DT_VECTOR2 ||
								aField->mFieldType == DefFieldType::DT_ARRAY)
								aX86Size += 8;
							else
								aX86Size += 4;
						}

						// ReanimatorDefinition has an unmapped pointer `mReanimAtlas` at the end
						// which takes 4 bytes on x86. We must include it so the stream pointer
						// correctly advances to the start of the dynamic array data.
						if (aX86Size == 12)
							aX86Size = 16;

						void *aX86Buffer = _alloca(aX86Size);
						SMemR(aBufferPtr, aX86Buffer, aX86Size);

						bool aResult =
							LegacyDefinition::DefMapReadFromCache(aBufferPtr, theDefMap, theDefinition, aX86Buffer);

						size_t aReadMemSize = (uintptr_t)aBufferPtr - (uintptr_t)aUncompressedBuffer;
						DefinitionFree(aUncompressedBuffer);

						// We also bypass the read size check because structure sizes differ.
						if (aResult && false && aReadMemSize != aUncompressedSize)
							TodTrace("Compiled file wrong size: %s\n", theCompiledFilePath.c_str());
						return aResult;
					}
				}
			}
			DefinitionFree(aUncompressedBuffer);
		}
	}
	return false;
}

void *LegacyDefinition::DefinitionUncompressCompiledBuffer(void *theCompressedBuffer, size_t theCompressedBufferSize,
														   size_t &theUncompressedSize,
														   const SexyString &theCompiledFilePath)
{
	auto sz = theCompressedBufferSize;
	// theCompressedBuffer The first two four-byte bytes contain special data, and this check verifies whether its length is sufficient for 8 bytes (i.e., 2 four-byte bytes).
	if (theCompressedBufferSize < 8)
	{
		TodTrace("Compile def too small", theCompiledFilePath.c_str());
		return nullptr;
	}
	// 将 theCompressedBuffer 的前两个四字节视为一个 CompressedDefinitionHeader
	LegacyDefinition::CompressedDefinitionHeader *aHeader = (CompressedDefinitionHeader *)theCompressedBuffer;
	if (aHeader->mCookie != COMPILED_LEGACY_DEFINITION_MAGIC)
	{
		TodTrace("Compiled fire cookie wrong: %s\n", theCompiledFilePath.c_str());
		return nullptr;
	}
	Bytef *aUncompressedBuffer = (Bytef *)DefinitionAlloc(aHeader->mUncompressedSize);
	theCompressedBufferSize = aHeader->mUncompressedSize; //my addition
	Bytef *aSrc = (Bytef *)theCompressedBuffer +
				  sizeof(LegacyDefinition::CompressedDefinitionHeader); // 实际解压数据从第 3 个四字节开始
	int aResult = uncompress(aUncompressedBuffer, (uLongf *)&theCompressedBufferSize, aSrc,
							 sz - sizeof(LegacyDefinition::CompressedDefinitionHeader));
	TOD_ASSERT(aResult == Z_OK);
	TOD_ASSERT(theCompressedBufferSize == aHeader->mUncompressedSize);
	theUncompressedSize = aHeader->mUncompressedSize;
	return aUncompressedBuffer;
}