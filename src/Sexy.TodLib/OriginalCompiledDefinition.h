#if SEXY_IS_X86

#pragma once

#define COMPILED_LEGACY_DEFINITION_MAGIC 0xDEADFED4L

#include "TodList.h"
#include "Reanimator.h"
#include "TodParticle.h"
#include <stdint.h>

class DefMap;
class DefSymbol;
class DefinitionArrayDef;
class FloatParameterTrack;

namespace LegacyDefinition
{

	// ====================================================================================================
	// Compress definition data header
	// ----------------------------------------------------------------------------------------------------
	// Add a compression definition header before the compressed data to record the verification cache value and the original data length, which is used to check data integrity during decompression.
	// ====================================================================================================
	class CompressedDefinitionHeader
	{
	  public:
		unsigned int mCookie;			 // Cache values ​​used for compression verification
		unsigned long mUncompressedSize; // Length of uncompressed data
	};

	bool DefinitionReadCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition);
	void *DefinitionUncompressCompiledBuffer(void *theCompressedBuffer, size_t theCompressedBufferSize, size_t &theUncompressedSize, const SexyString &theCompiledFilePath);
	uint32_t DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol *theSymbolMap);
	uint32_t DefinitionCalcHashDefMap(int aSchemaHash, DefMap *theDefMap, TodList<DefMap *> &theProgressMaps);
	uint32_t DefinitionCalcHash(DefMap *theDefMap);
	bool DefReadFromCacheString(void *&theReadPtr, char **theString);
	bool DefReadFromCacheArray(void *&theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap);
	bool DefReadFromCacheImage(void *&theReadPtr, Image **theImage);
	bool DefReadFromCacheFont(void *&theReadPtr, Font **theFont);
	bool DefReadFromCacheFloatTrack(void *&theReadPtr, FloatParameterTrack *theTrack);
	bool DefMapReadFromCache(void *&theReadPtr, DefMap *theDefMap, void *theDefinition);
}


#endif