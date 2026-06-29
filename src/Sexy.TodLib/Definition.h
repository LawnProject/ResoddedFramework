#ifndef __TODDEFINITION_H__
#define __TODDEFINITION_H__

#include "CompiledDefinition.h"
#include "TodList.h"
#include "Reanimator.h"
#include "TodParticle.h"

enum class DefFieldType : int
{
	DT_INVALID,
	DT_INT,
	DT_FLOAT,
	DT_STRING,
	DT_ENUM,
	DT_VECTOR2,
	DT_ARRAY,
	DT_TRACK_FLOAT,
	DT_FLAGS,
	DT_IMAGE,
	DT_FONT
};

/*
	[For clarity, the following is defined as follows]

	A class used to store definition data for other classes is called a definition data class, denoted by _DefClass. 
	Correspondingly, the class defined by _DefClass is denoted by _Class.

	For example, 
	ReanimatorDefinition serves as the definition data class for the Reanimation class (an animation class)
	TodParticleDefinition serves as the definition data class for the TodParticleSystem class (a particle system class), etc.
*/

// ====================================================================================================
// Define the mark
// ----------------------------------------------------------------------------------------------------
// For data of an enumeration type determined by a flag bit, a DefSymbol records the value of one of its flag bits (of an enumeration item).
// ====================================================================================================
class DefSymbol
{
  public:
	int mSymbolValue;        // The value of the flag bit or the numerical value corresponding to the enumeration item; if it is -1, it means that the item does not exist.
	const char *mSymbolName; // The flag or the name of the enumeration item, when a null pointer indicates that the item does not exist, is used as a marker to indicate the end of the read.
};
extern DefSymbol gParticleFlagSymbols[];
extern DefSymbol gEmitterTypeSymbols[];
extern DefSymbol gParticleTypeSymbols[];

// ====================================================================================================
// Structure Fields
// ----------------------------------------------------------------------------------------------------
// The structure field records the data of a member variable (_MemVar) in a class and its structure within the class.
// ====================================================================================================
class DefField
{
  public:
	const char *mFieldName;  // The name of _MemVar. Pointing to an empty character array indicates that this variable does not exist, and is therefore used as a marker to indicate the end of reading.
	int mFieldOffset;        // Offset within the class (understand in conjunction with assembly language)
	DefFieldType mFieldType; // *_MemVar The data storage type varies, and the reading method also differs for different types of data.
	void *mExtraData;        // Additional data. Used for deep copying pointer variables contained in *_MemVar.
	                         // If _MemVar is a pointer variable pointing to other defined data, then mExtraData is a pointer to the definition structure diagram of the class defined by _MemVar;
	                         // If _MemVar is data of a flag or enumeration type, then mExtraData is a pointer to an array of DefSymbol arrays for each of its flag data; otherwise, mExtraData is a null pointer.
	                         // Although by using the definition structure diagram of a _DefClass class, it is possible to read all the data of the _DefClass through relevant functions (i.e., perform a shallow copy).
	                         // However, the data pointed to by some pointer variables in _DefClass still needs to be recursively read (i.e., deep copied) based on the definition structure diagram of the corresponding type.
	                         // In other words, by using a nested definition structure diagram, the pointers of various types of variables that originally had hierarchical relationships are "expanded" until there are no pointers in the current variable's data that can be "expanded", at which point the recursive reading ends.
};

// ====================================================================================================
// Define the structure diagram
// ----------------------------------------------------------------------------------------------------
// The definition structure diagram describes the storage format and retrieval method of defined data in a defined data class (_DefClass), similar to "_DefDefClass".
// ====================================================================================================
class DefMap
{
  public:
	DefField
	    *mMapFields;                   // An array of structure fields, recording the structure of each member variable in the _DefClass class within _DefClass (each record represents one structure).
	int mDefSize;                      // The memory size occupied by a _DefClass instance, which is also the length of the initial read, is typically sizeof(_DefClass).
	void *(*mConstructorFunc)(void *); // A pointer to the constructor of an instance of type _DefClass.
};

void *__cdecl TodParticleDefinitionConstructor(void *thePointer);
void *__cdecl TodEmitterDefinitionConstructor(void *thePointer);
void *__cdecl ParticleFieldConstructor(void *thePointer);
void *__cdecl TrailDefinitionConstructor(void *thePointer);
void *__cdecl ReanimatorTransformConstructor(void *thePointer);
void *__cdecl ReanimatorTrackConstructor(void *thePointer);
void *__cdecl ReanimatorDefinitionConstructor(void *thePointer);

extern DefField gParticleFieldDefFields[];
extern DefMap gParticleFieldDefMap;

extern DefField gEmitterDefFields[];
extern DefMap gEmitterDefMap;

extern DefField gParticleDefFields[];
extern DefMap gParticleDefMap;

extern DefMap gTrailDefMap;

extern DefField gReanimatorTransformDefFields[];
extern DefMap gReanimatorTransformDefMap;

extern DefField gReanimatorTrackDefFields[];
extern DefMap gReanimatorTrackDefMap;

extern DefField gReanimatorDefFields[];
extern DefMap gReanimatorDefMap;

// ====================================================================================================
// Define array
// ----------------------------------------------------------------------------------------------------
// One array of definitions corresponds to one type of pointer variable that points to.
// ====================================================================================================
class DefinitionArrayDef
{
  public:
	void *mArrayData; // An array consisting of instances of a specific defined data type, such as the "track" definition in an animation definition.
	int mArrayCount;  // The size of the array, such as the number of "tracks" in an animation definition or the number of "emitters" in a particle system definition.
	                  // Define a combination of "array (pointer) + quantity" in the data class, which will be treated as a DefinitionArrayDef structure by DefField when read.
	                  // For example, *mEmitterDefs and mEmitterDefCount under TodParticleDefinition, and *mParticleFields and mParticleFieldCount under TodEmitterDefinition.
	                  // During reading, data items in mArrayCount are always correctly read on the first read (because they are integers), and therefore will also serve as a verification reference during the subsequent repair process of mArrayData.
};

// ====================================================================================================
// Define path
// ----------------------------------------------------------------------------------------------------
// Define a path to establish an association between a texture prefix and the folder path where the textures with that prefix are stored.
// ====================================================================================================
class DefLoadResPath
{
  public:
	const char *mPrefix;    // Image prefixes, such as "IMAGE_"
	const char *mDirectory; // The folder containing the textures corresponding to the prefix, such as "images\".
};

SexyString /*__cdecl*/ DefinitionGetCompiledFilePathFromXMLFilePath(const SexyString &theXMLFilePath);
bool IsFileInPakFile(const SexyString &theFilePath);
bool DefinitionIsCompiled(const SexyString &theXMLFilePath);
bool DefinitionReadCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition);
void DefinitionFillWithDefaults(DefMap *theDefMap, void *theDefinition);
void DefinitionXmlError(XMLParser *theXmlParser, char *theFormat, ...);
bool DefSymbolValueFromString(DefSymbol *theSymbolMap, const char *theName, int *theResultValue);
bool DefinitionReadXMLString(XMLParser *theXmlParser, SexyString &theValue);
bool DefinitionReadIntField(XMLParser *theXmlParser, int *theValue);
bool DefinitionReadFloatField(XMLParser *theXmlParser, float *theValue);
bool DefinitionReadStringField(XMLParser *theXmlParser, const char **theValue);
bool DefinitionReadEnumField(XMLParser *theXmlParser, int *theValue, DefSymbol *theSymbolMap);
bool DefinitionReadVector2Field(XMLParser *theXmlParser, SexyVector2 *theValue);
bool DefinitionReadArrayField(XMLParser *theXmlParser, DefinitionArrayDef *theArray, DefField *theField);
bool DefinitionReadFloatTrackField(XMLParser *theXmlParser, FloatParameterTrack *theTrack);
bool DefinitionReadFlagField(XMLParser *theXmlParser,
                             const SexyString &theElementName,
                             uintptr_t *theResultValue,
                             DefSymbol *theSymbolMap);
bool DefinitionReadImageField(XMLParser *theXmlParser, Image **theImage);
bool DefinitionReadFontField(XMLParser *theXmlParser, Font **theFont);
bool DefinitionReadField(XMLParser *theXmlParser, DefMap *theDefMap, void *theDefinition, bool *theDone);
bool DefinitionWriteCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition);
bool DefinitionCompileFile(const SexyString theXMLFilePath,
                           const SexyString &theCompiledFilePath,
                           DefMap *theDefMap,
                           void *theDefinition);
void *DefinitionAlloc(int theSize);
void DefinitionFree(void *&theMemory);
void *DefinitionUncompressCompiledBuffer(const CompiledDefinitionHeader *aHeader,
                                         void *theCompressedBuffer,
                                         size_t theCompressedBufferSize,
                                         const SexyString &theCompiledFilePath);
uint32_t /*__cdecl*/ DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol *theSymbolMap);
uint32_t /*__cdecl*/ DefinitionCalcHashDefMap(int aSchemaHash, DefMap *theDefMap, TodList<DefMap *> &theProgressMaps);
uint32_t /*__cdecl*/ DefinitionCalcHash(DefMap *theDefMap);
bool DefReadFromCacheInt(void *&theReadPtr, int *theInt);
bool DefReadFromCacheFloat(void *&theReadPtr, float *theFloat);
bool DefReadFromCacheFlag(void *&theReadPtr, uint32_t *theFlag);
bool DefReadFromCacheString(void *&theReadPtr, const char **theString);
bool DefReadFromCacheVector2(void *&theReadPtr, SexyVector2 *theVector);
bool DefReadFromCacheArray(void *&theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap);
bool DefReadFromCacheImage(void *&theReadPtr, Image **theImage);
bool DefReadFromCacheFont(void *&theReadPtr, Font **theFont);
bool DefReadFromCacheFloatTrack(void *&theReadPtr, FloatParameterTrack *theTrack);
bool DefMapReadFromCache(void *&theReadPtr, DefMap *theDefMap, void *theDefinition);

void DefWriteToCacheString(DefinitionCompiler *theReadPtr, char **theString);
void DefWriteToCacheArray(DefinitionCompiler *theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap);
void DefWriteToCacheImage(DefinitionCompiler *theReadPtr, Image **theImage);
void DefWriteToCacheFont(DefinitionCompiler *theReadPtr, Font **theFont);
void DefWriteToCacheFloatTrack(DefinitionCompiler *theReadPtr, FloatParameterTrack *theTrack);
void DefMapWriteToCache(DefinitionCompiler *theReadPtr, DefMap *theDefMap, void *theDefinition);

bool DefinitionCompileAndLoad(const SexyString &theXMLFilePath, DefMap *theDefMap, void *theDefinition, bool recompile);
bool DefinitionLoadMap(XMLParser *theXmlParser, DefMap *theDefMap, void *theDefinition);
bool DefinitionLoadImage(Image **theImage, const SexyString &theName);
bool DefinitionLoadFont(Font **theFont, const SexyString &theName);
bool DefinitionLoadXML(const SexyString &theFilename, DefMap *theDefMap, void *theDefinition, bool recompile);
void DefinitionFreeArrayField(DefinitionArrayDef *theArray, DefMap *theDefMap);
void DefinitionFreeMap(DefMap *theDefMap, void *theDefinition);

bool FloatTrackIsSet(const FloatParameterTrack &theTrack);
void FloatTrackSetDefault(FloatParameterTrack &theTrack, float theValue);
float FloatTrackEvaluate(FloatParameterTrack &theTrack, float theTimeValue, float theInterp);
float FloatTrackEvaluateFromLastTime(FloatParameterTrack &theTrack, float theTimeValue, float theInterp);
bool FloatTrackIsConstantZero(FloatParameterTrack &theTrack);

#endif
