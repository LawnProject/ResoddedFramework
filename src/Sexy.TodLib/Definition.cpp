#include "Trail.h"
#include <assert.h>
#include "TodDebug.h"
#include "Definition.h"
#include <zlib.h>
#include "../PakLib/PakInterface.h"
#include "../SexyAppFramework/PerfTimer.h"
#include "../SexyAppFramework/XMLParser.h" 
#include <fstream>
#include "OriginalCompiledDefinition.h"

DefSymbol gTrailFlagDefSymbols[] = {
	{0, "Loops"},
	{-1, nullptr}};
DefField gTrailDefFields[] = {
	{"Image", offsetof(TrailDefinition, mImage), DefFieldType::DT_IMAGE, nullptr},
	{"MaxPoints", offsetof(TrailDefinition, mMaxPoints), DefFieldType::DT_INT, nullptr},
	{"MinPointDistance", offsetof(TrailDefinition, mMinPointDistance), DefFieldType::DT_FLOAT, nullptr},
	{"TrailFlags", offsetof(TrailDefinition, mTrailFlags), DefFieldType::DT_FLAGS, gTrailFlagDefSymbols},
	{"WidthOverLength", offsetof(TrailDefinition, mWidthOverLength), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"WidthOverTime", offsetof(TrailDefinition, mWidthOverTime), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"AlphaOverLength", offsetof(TrailDefinition, mAlphaOverLength), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"AlphaOverTime", offsetof(TrailDefinition, mAlphaOverTime), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"TrailDuration", offsetof(TrailDefinition, mTrailDuration), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr}};
DefMap gTrailDefMap = {gTrailDefFields, sizeof(TrailDefinition), TrailDefinitionConstructor};

DefSymbol gParticleFlagSymbols[] = {
	{0, "RandomLaunchSpin"},
	{1, "AlignLaunchSpin"},
	{2, "AlignToPixel"},
	{4, "ParticleLoops"},
	{3, "SystemLoops"},
	{5, "ParticlesDontFollow"},
	{6, "RandomStartTime"},
	{7, "DieIfOverloaded"},
	{8, "Additive"},
	{9, "FullScreen"},
	{10, "SoftwareOnly"},
	{11, "HardwareOnly"},
	{-1, nullptr}};
DefSymbol gEmitterTypeSymbols[] = {
	{0, "Circle"},
	{1, "Box"},
	{2, "BoxPath"},
	{3, "CirclePath"},
	{4, "CircleEvenSpacing"},
	{-1, nullptr}};
DefSymbol gParticleTypeSymbols[] = {
	{1, "Friction"},
	{2, "Acceleration"},
	{3, "Attractor"},
	{4, "MaxVelocity"},
	{5, "Velocity"},
	{6, "Position"},
	{7, "SystemPosition"},
	{8, "GroundConstraint"},
	{9, "Shake"},
	{10, "Circle"},
	{11, "Away"},
	{-1, nullptr}};

DefField gParticleFieldDefFields[] = {
	{"FieldType", offsetof(ParticleField, mFieldType), DefFieldType::DT_ENUM, gParticleTypeSymbols},
	{"x", offsetof(ParticleField, mX), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"y", offsetof(ParticleField, mY), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr},
};
DefMap gParticleFieldDefMap = {gParticleFieldDefFields, sizeof(ParticleField), ParticleFieldConstructor};

DefField gEmitterDefFields[] = {
	{"Image", offsetof(TodEmitterDefinition, mImage), DefFieldType::DT_IMAGE, nullptr},
	{"ImageRow", offsetof(TodEmitterDefinition, mImageRow), DefFieldType::DT_INT, nullptr},
	{"ImageCol", offsetof(TodEmitterDefinition, mImageCol), DefFieldType::DT_INT, nullptr},
	{"ImageFrames", offsetof(TodEmitterDefinition, mImageFrames), DefFieldType::DT_INT, nullptr},
	{"Animated", offsetof(TodEmitterDefinition, mAnimated), DefFieldType::DT_INT, nullptr},
	{"ParticleFlags", offsetof(TodEmitterDefinition, mParticleFlags), DefFieldType::DT_FLAGS, gParticleFlagSymbols},
	{"EmitterType", offsetof(TodEmitterDefinition, mEmitterType), DefFieldType::DT_ENUM, gEmitterTypeSymbols},
	{"Name", offsetof(TodEmitterDefinition, mName), DefFieldType::DT_STRING, nullptr},
	{"SystemDuration", offsetof(TodEmitterDefinition, mSystemDuration), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"OnDuration", offsetof(TodEmitterDefinition, mOnDuration), DefFieldType::DT_STRING, nullptr},
	{"CrossFadeDuration", offsetof(TodEmitterDefinition, mCrossFadeDuration), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SpawnRate", offsetof(TodEmitterDefinition, mSpawnRate), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SpawnMinActive", offsetof(TodEmitterDefinition, mSpawnMinActive), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SpawnMaxActive", offsetof(TodEmitterDefinition, mSpawnMaxActive), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SpawnMaxLaunched", offsetof(TodEmitterDefinition, mSpawnMaxLaunched), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterRadius", offsetof(TodEmitterDefinition, mEmitterRadius), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterOffsetX", offsetof(TodEmitterDefinition, mEmitterOffsetX), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterOffsetY", offsetof(TodEmitterDefinition, mEmitterOffsetY), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterBoxX", offsetof(TodEmitterDefinition, mEmitterBoxX), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterBoxY", offsetof(TodEmitterDefinition, mEmitterBoxY), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterPath", offsetof(TodEmitterDefinition, mEmitterPath), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterSkewX", offsetof(TodEmitterDefinition, mEmitterSkewX), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"EmitterSkewY", offsetof(TodEmitterDefinition, mEmitterSkewY), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleDuration", offsetof(TodEmitterDefinition, mParticleDuration), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SystemRed", offsetof(TodEmitterDefinition, mSystemRed), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SystemGreen", offsetof(TodEmitterDefinition, mSystemGreen), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SystemBlue", offsetof(TodEmitterDefinition, mSystemBlue), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SystemAlpha", offsetof(TodEmitterDefinition, mSystemAlpha), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"SystemBrightness", offsetof(TodEmitterDefinition, mSystemBrightness), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"LaunchSpeed", offsetof(TodEmitterDefinition, mLaunchSpeed), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"LaunchAngle", offsetof(TodEmitterDefinition, mLaunchAngle), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"Field", offsetof(TodEmitterDefinition, mParticleFields), DefFieldType::DT_ARRAY, &gParticleFieldDefMap},
	{"SystemField", offsetof(TodEmitterDefinition, mSystemFields), DefFieldType::DT_ARRAY, &gParticleFieldDefMap},
	{"ParticleRed", offsetof(TodEmitterDefinition, mParticleRed), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleGreen", offsetof(TodEmitterDefinition, mParticleGreen), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleBlue", offsetof(TodEmitterDefinition, mParticleBlue), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleAlpha", offsetof(TodEmitterDefinition, mParticleAlpha), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleBrightness", offsetof(TodEmitterDefinition, mParticleBrightness), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleSpinAngle", offsetof(TodEmitterDefinition, mParticleSpinAngle), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleSpinSpeed", offsetof(TodEmitterDefinition, mParticleSpinSpeed), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleScale", offsetof(TodEmitterDefinition, mParticleScale), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ParticleStretch", offsetof(TodEmitterDefinition, mParticleStretch), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"CollisionReflect", offsetof(TodEmitterDefinition, mCollisionReflect), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"CollisionSpin", offsetof(TodEmitterDefinition, mCollisionSpin), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ClipTop", offsetof(TodEmitterDefinition, mClipTop), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ClipBottom", offsetof(TodEmitterDefinition, mClipBottom), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ClipLeft", offsetof(TodEmitterDefinition, mClipLeft), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"ClipRight", offsetof(TodEmitterDefinition, mClipRight), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"AnimationRate", offsetof(TodEmitterDefinition, mAnimationRate), DefFieldType::DT_TRACK_FLOAT, nullptr},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr},
};
DefMap gEmitterDefMap = {gEmitterDefFields, sizeof(TodEmitterDefinition), TodEmitterDefinitionConstructor};

DefField gParticleDefFields[] = {
	{"Emitter", 0x0, DefFieldType::DT_ARRAY, &gEmitterDefMap},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr}};
DefMap gParticleDefMap = {
	gParticleDefFields, sizeof(TodParticleDefinition), TodParticleDefinitionConstructor};

DefField gReanimatorTransformDefFields[] = {
	{"x", offsetof(ReanimatorTransform, mTransX), DefFieldType::DT_FLOAT, nullptr},
	{"y", offsetof(ReanimatorTransform, mTransY), DefFieldType::DT_FLOAT, nullptr},
	{"kx", offsetof(ReanimatorTransform, mSkewX), DefFieldType::DT_FLOAT, nullptr},
	{"ky", offsetof(ReanimatorTransform, mSkewY), DefFieldType::DT_FLOAT, nullptr},
	{"sx", offsetof(ReanimatorTransform, mScaleX), DefFieldType::DT_FLOAT, nullptr},
	{"sy", offsetof(ReanimatorTransform, mScaleY), DefFieldType::DT_FLOAT, nullptr},
	{"f", offsetof(ReanimatorTransform, mFrame), DefFieldType::DT_FLOAT, nullptr},
	{"a", offsetof(ReanimatorTransform, mAlpha), DefFieldType::DT_FLOAT, nullptr},
	{"i", offsetof(ReanimatorTransform, mImage), DefFieldType::DT_IMAGE, nullptr},
	{"font", offsetof(ReanimatorTransform, mFont), DefFieldType::DT_FONT, nullptr},
	{"text", offsetof(ReanimatorTransform, mText), DefFieldType::DT_STRING, nullptr},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr}
};
DefMap gReanimatorTransformDefMap = { gReanimatorTransformDefFields, sizeof(ReanimatorTransform), ReanimatorTransformConstructor};

DefField gReanimatorTrackDefFields[] = {
	{"name", offsetof(ReanimatorTrack, mName), DefFieldType::DT_STRING, nullptr},
	{"t", offsetof(ReanimatorTrack, mTransforms), DefFieldType::DT_ARRAY, &gReanimatorTransformDefMap},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr}
};
DefMap gReanimatorTrackDefMap = {
	gReanimatorTrackDefFields, sizeof(ReanimatorTrack), ReanimatorTrackConstructor};

DefField gReanimatorDefFields[] = {
	{"track", offsetof(ReanimatorDefinition, mTracks), DefFieldType::DT_ARRAY, &gReanimatorTrackDefMap},
	{"fps", offsetof(ReanimatorDefinition, mFPS), DefFieldType::DT_FLOAT, nullptr},
	{"", 0x0, DefFieldType::DT_INVALID, nullptr}
};
DefMap gReanimatorDefMap = {
	gReanimatorDefFields, sizeof(ReanimatorDefinition), ReanimatorDefinitionConstructor};

static DefLoadResPath gDefLoadResPaths[4] = {
	{"IMAGE_", ""}, {"IMAGE_", "particles/"}, {"IMAGE_REANIM_", "reanim/"}, {"IMAGE_REANIM_", "images/"}};

void *ParticleFieldConstructor(void *thePointer)
{
	if (thePointer)
	{
		((ParticleField *)thePointer)->mX.mNodes = nullptr;
		((ParticleField *)thePointer)->mX.mCountNodes = 0;
		((ParticleField *)thePointer)->mY.mNodes = nullptr;
		((ParticleField *)thePointer)->mY.mCountNodes = 0;
		((ParticleField *)thePointer)->mFieldType = ParticleFieldType::FIELD_INVALID;
	}
	return thePointer;
}

void *TodEmitterDefinitionConstructor(void *thePointer)
{
	if (thePointer)
	{
		memset(thePointer, NULL, sizeof(TodEmitterDefinition));
		((TodEmitterDefinition *)thePointer)->mImageFrames = 1;
		((TodEmitterDefinition *)thePointer)->mEmitterType = EmitterType::EMITTER_BOX;
		((TodEmitterDefinition *)thePointer)->mName = "";
		((TodEmitterDefinition *)thePointer)->mOnDuration = "";
		((TodEmitterDefinition *)thePointer)->mImageRow = 0;
		((TodEmitterDefinition *)thePointer)->mImageCol = 0;
		((TodEmitterDefinition *)thePointer)->mAnimated = 0;
		((TodEmitterDefinition *)thePointer)->mImage = nullptr;
	}
	return thePointer;
}

void *TodParticleDefinitionConstructor(void *thePointer)
{
	if (thePointer)
	{
		((TodParticleDefinition *)thePointer)->mEmitterDefs = nullptr;
		((TodParticleDefinition *)thePointer)->mEmitterDefCount = 0;
	}
	return thePointer;
}

void *TrailDefinitionConstructor(void *thePointer)
{
	if (thePointer)
	{
		memset(thePointer, 0, sizeof(TrailDefinition));
		((TrailDefinition *)thePointer)->mMinPointDistance = 1.0f;
		((TrailDefinition *)thePointer)->mMaxPoints = 2;
		((TrailDefinition *)thePointer)->mTrailFlags = 0U;
		((TrailDefinition *)thePointer)->mImage = nullptr;
	}
	return thePointer;
}

void *ReanimatorTransformConstructor(void *thePointer)
{
	if (thePointer)
	{
		((ReanimatorTransform *)thePointer)->mTransX = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mTransY = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mSkewX = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mSkewY = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mScaleX = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mScaleY = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mFrame = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mAlpha = DEFAULT_FIELD_PLACEHOLDER;
		((ReanimatorTransform *)thePointer)->mImage = nullptr;
		((ReanimatorTransform *)thePointer)->mFont = nullptr;
		((ReanimatorTransform *)thePointer)->mText = "";
	}
	return thePointer;
}

void *ReanimatorTrackConstructor(void *thePointer)
{
	if (thePointer)
	{
		((ReanimatorTrack *)thePointer)->mName = "";
		((ReanimatorTrack *)thePointer)->mTransforms = nullptr;
		((ReanimatorTrack *)thePointer)->mTransformCount = 0;
	}
	return thePointer;
}

void *ReanimatorDefinitionConstructor(void *thePointer)
{
	if (thePointer)
	{
		((ReanimatorDefinition *)thePointer)->mTracks = nullptr;
		((ReanimatorDefinition *)thePointer)->mTrackCount = 0;
		((ReanimatorDefinition *)thePointer)->mFPS = 12.0f;
		((ReanimatorDefinition *)thePointer)->mReanimAtlas = nullptr;
	}
	return thePointer;
}

void *DefinitionAlloc(int theSize)
{
	void *aPtr = new char[theSize]();
	TOD_ASSERT(aPtr);
	return aPtr;
}

void DefinitionFree(void* &theMemory)
{
	delete[] theMemory;
	theMemory = nullptr;
}


bool DefinitionLoadImage(Image **theImage, const SexyString &theName)
{
	// If the texture file path does not exist, there is no need to obtain the texture.
	if (theName.size() == 0)
	{
		*theImage = nullptr;
		return true;
	}

	// Try using File Explorer to load textures from XML.
	Image *anImage = (Image *)gSexyAppBase->mResourceManager->LoadImage(theName);
	if (anImage)
	{
		*theImage = anImage;
		return true;
	}

	// Manually load textures from possible texture paths.
	for (const DefLoadResPath &aLoadResPath : gDefLoadResPaths)
	{
		int aNameLen = theName.size();
		int aPrefixLen = strlen(aLoadResPath.mPrefix);
		if (aPrefixLen < aNameLen)
		{
			SexyString aPathToTry = aLoadResPath.mDirectory + theName.substr(aPrefixLen, aNameLen);
			SharedImageRef aImageRef = gSexyAppBase->GetSharedImage(aPathToTry);
			if ((Image *)aImageRef != nullptr)
			{
				TodHesitationTrace("Load Image '%s'", theName.c_str());
				TodAddImageToMap(&aImageRef, theName);
				TodMarkImageForSanding((Image *)aImageRef);
				*theImage = (Image *)aImageRef;
				return true;
			}
		}
	}
	return false;
}

bool DefinitionLoadFont(Font **theFont, const SexyString &theName)
{
	Font *aFont = gSexyAppBase->mResourceManager->LoadFont(SexyStringToString(theName));
	*theFont = aFont;
	return aFont != nullptr;
}

bool DefinitionLoadXML(const SexyString &theFileName, DefMap *theDefMap, void *theDefinition)
{
	return DefinitionCompileAndLoad(theFileName, theDefMap, theDefinition);
}

bool DefReadFromCacheArray(void *&theReadPtr, DefinitionArrayDef *theArray, DefMap *theDefMap)
{
	uint32_t anArraySize;
	SMemR(theReadPtr, &anArraySize, sizeof(uint32_t)); // Read the amount of elements in the array
	theArray->mArrayCount = anArraySize;
	if (theArray->mArrayCount == 0) // If there are no instances of the class, then there is no need to read them.
		return true;

	uint32_t aArraySize = theDefMap->mDefSize * theArray->mArrayCount;
	void *pData = DefinitionAlloc(aArraySize); // Allocate memory and initialize it with 0.
	theArray->mArrayData = pData;
	for (int i = 0; i < theArray->mArrayCount; i++)
	{

		void *element = (uint8_t *)pData + theDefMap->mDefSize * i;
		if (!DefMapReadFromCache(theReadPtr, theDefMap, element)) // The last parameter represents pData[i].
			return false;
	}

	return true;
}

bool DefReadFromCacheFloatTrack(void *&theReadPtr, FloatParameterTrack *theTrack)
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
bool DefReadFromCacheInt(void*& theReadPtr, int* theInt)
{
	int aValue;
	SMemR(theReadPtr, &aValue, sizeof(int));
	*theInt = aValue;
	return true;
}

bool DefReadFromCacheFloat(void*& theReadPtr, float* theFloat)
{
	float aValue;
	SMemR(theReadPtr, &aValue, sizeof(float));
	*theFloat = aValue;
	return true;
}

bool DefReadFromCacheFlag(void *&theReadPtr, uint32_t *theFlag)
{
	uint32_t aValue;
	SMemR(theReadPtr, &aValue, sizeof(uint32_t));
	*theFlag = aValue;
	return true;
}

bool DefReadFromCacheString(void *&theReadPtr, char **theString)
{
	uint32_t aLen;
	SMemR(theReadPtr, &aLen, sizeof(uint32_t));
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

bool DefReadFromCacheVector2(void *&theReadPtr, SexyVector2 *theVector)
{
	SexyVector2 aValue;
	SMemR(theReadPtr, &aValue, sizeof(SexyVector2));
	*theVector = aValue;
	return true;
}

bool DefReadFromCacheImage(void *&theReadPtr, Image **theImage)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));		  // 读取贴图标签字符数组的长度
	char *aImageName = (char *)_alloca(aLen + 1); // 在栈上分配贴图标签字符数组的内存空间
	SMemR(theReadPtr, aImageName, aLen);		  // 读取贴图标签字符数组
	aImageName[aLen] = '\0';

	*theImage = nullptr;
	return aImageName[0] == '\0' || DefinitionLoadImage(theImage, aImageName);
}

bool DefReadFromCacheFont(void *&theReadPtr, Font **theFont)
{
	int aLen;
	SMemR(theReadPtr, &aLen, sizeof(int));		 // 读取字体标签字符数组的长度
	char *aFontName = (char *)_alloca(aLen + 1); // 在栈上分配字体标签字符数组的内存空间
	SMemR(theReadPtr, aFontName, aLen);			 // 读取字体标签字符数组
	aFontName[aLen] = '\0';

	*theFont = nullptr;
	return aFontName[0] == '\0' || DefinitionLoadFont(theFont, aFontName);
}

bool DefMapReadFromCache(void *&theReadPtr, DefMap *theDefMap, void *theDefinition)
{
	// 分别确认每一个成员变量，并修复其中的指针类型和标志类型的变量
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		bool aSucceed = true;
		void *aDest = (void *)((uintptr_t)theDefinition + aField->mFieldOffset); // 指向该成员变量的指针
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_ENUM:
		case DefFieldType::DT_INT:
			aSucceed = DefReadFromCacheInt(theReadPtr, (int *)aDest);
			break;
		case DefFieldType::DT_FLOAT:
			aSucceed = DefReadFromCacheFloat(theReadPtr, (float *)aDest);
			break;
		case DefFieldType::DT_FLAGS:
			aSucceed = DefReadFromCacheFlag(theReadPtr, (uint32_t *)aDest);
			break;
		case DefFieldType::DT_STRING:
			aSucceed = DefReadFromCacheString(theReadPtr, (char **)aDest);
			break;
		case DefFieldType::DT_VECTOR2:
			aSucceed = DefReadFromCacheVector2(theReadPtr, (SexyVector2 *)aDest);
			break;
		case DefFieldType::DT_ARRAY:
			aSucceed = DefReadFromCacheArray(theReadPtr, (DefinitionArrayDef *)aDest, (DefMap *)aField->mExtraData);
			break;
		case DefFieldType::DT_IMAGE:
			aSucceed = DefReadFromCacheImage(theReadPtr, (Image **)aDest);
			break;
		case DefFieldType::DT_FONT:
			aSucceed = DefReadFromCacheFont(theReadPtr, (Font **)aDest);
			break;
		case DefFieldType::DT_TRACK_FLOAT:
			aSucceed = DefReadFromCacheFloatTrack(theReadPtr, (FloatParameterTrack *)aDest);
			break;
		}

		if (!aSucceed)
			return false;
	}
	return true;
}

uint32_t DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol *theSymbolMap)
{
	while (theSymbolMap->mSymbolName != nullptr)
	{
		aSchemaHash = crc32(aSchemaHash, (const Bytef *)theSymbolMap->mSymbolName, strlen(theSymbolMap->mSymbolName));
		aSchemaHash = crc32(aSchemaHash, (const Bytef *)&theSymbolMap->mSymbolValue, sizeof(int));
		theSymbolMap++;
	}
	return aSchemaHash;
}

uint32_t DefinitionCalcHashDefMap(int aSchemaHash, DefMap *theDefMap, TodList<DefMap *> &theProgressMaps)
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
			aSchemaHash = DefinitionCalcHashSymbolMap(aSchemaHash, (DefSymbol *)aField->mExtraData);
			break;
		case DefFieldType::DT_ARRAY:
			aSchemaHash = DefinitionCalcHashDefMap(aSchemaHash, (DefMap *)aField->mExtraData, theProgressMaps);
			break;
		}
	}
	return aSchemaHash;
}

uint32_t DefinitionCalcHash(DefMap *theDefMap)
{
	TodList<DefMap *> aProgressMaps;
	uint32_t aResult = DefinitionCalcHashDefMap(crc32(0L, (Bytef *)Z_NULL, NULL) + 1, theDefMap, aProgressMaps);
	return aResult;
}

void *DefinitionUncompressCompiledBuffer(const CompiledDefinitionHeader *aHeader,
										 void *theCompressedBuffer,
										 size_t theCompressedBufferSize,
										 const SexyString &theCompiledFilePath)
{
	auto sz = theCompressedBufferSize;
	
	if (theCompressedBufferSize < 8)
	{
		TodTrace("Compile def too small", theCompiledFilePath.c_str());
		return nullptr;
	}

	Bytef *aUncompressedBuffer = (Bytef *)DefinitionAlloc(aHeader->mUncompressedSize);
	theCompressedBufferSize = aHeader->mUncompressedSize;
	int aResult = uncompress(aUncompressedBuffer,
							 (uLongf *)&theCompressedBufferSize,
							 (Bytef *)theCompressedBuffer,
							 sz);
	TOD_ASSERT(aResult == Z_OK);
	TOD_ASSERT(theCompressedBufferSize == aHeader->mUncompressedSize);
	return aUncompressedBuffer;
}

bool DefinitionReadCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition)
{
	CompiledDefinitionFile aCompiledFile;

	PerfTimer aTimer;
	aTimer.Start();
	FILE *pFile = fopen(theCompiledFilePath.c_str(), "rb");
	if (pFile)
	{
		fseek(pFile, 0, 2);
		size_t aFileSize = ftell(pFile);
		fseek(pFile, 0, 0);
		aCompiledFile.mData.resize(aFileSize);
		bool aReadCompressedFailed = fread(aCompiledFile.mData.data(), sizeof(uint8_t), aFileSize, pFile) != aFileSize;
		fclose(pFile);
		if (aReadCompressedFailed)
		{
			TodTrace("Failed to read compiled file: %s\n", theCompiledFilePath.c_str());
			return false;
		}

		if (aCompiledFile.isValid())
		{
			const CompiledDefinitionHeader *aHeader = aCompiledFile.GetHeader();
			if (aHeader->mDataOffset > aFileSize)
			{
				TodTrace("Data Offset is larger then file size: %s\n", theCompiledFilePath.c_str());
				return false;
			}
			size_t aCompressedSize = aFileSize - aHeader->mDataOffset;

			void *aDataPtr = (void *)aCompiledFile.GetCompressedData();
			void *anUncompressedData =
				DefinitionUncompressCompiledBuffer(aHeader, aDataPtr, aCompressedSize, theCompiledFilePath);

			if (anUncompressedData == nullptr)
			{
				TodTrace("Failed to uncompress: %s\n", theCompiledFilePath.c_str());
				return false;
			}

			bool aResult = DefMapReadFromCache(anUncompressedData, theDefMap, theDefinition);

			return aResult;
		}
		else
		{
#if SEXY_IS_X86
			return LegacyDefinition::DefinitionReadCompiledFile(theCompiledFilePath, theDefMap, theDefinition);
#else
			return false;
#endif
		}
		
	}

	return false;
}

SexyString DefinitionGetCompiledFilePathFromXMLFilePath(const SexyString &theXMLFilePath)
{
	return "compiled/" + theXMLFilePath + ".compiled";
}

bool IsFileInPakFile(const SexyString &theFilePath)
{
	PFILE *pFile = p_fopen(theFilePath.c_str(), "rb");
	bool aIsInPak =
		pFile && !pFile->mFP; // The file found and opened by mPakRecordMap.find has a null pointer mFP (because it was not opened from an actual file).
	if (pFile)
	{
		p_fclose(pFile);
	}
	return aIsInPak;
}

bool DefinitionIsCompiled(const SexyString &theXMLFilePath)
{
	SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
	PFILE *pFile = p_fopen(aCompiledFilePath.c_str(), "rb");
	if (pFile)
	{
		p_fclose(pFile);
		return true;
	}
	return false;
}

void DefinitionFillWithDefaults(DefMap *theDefMap, void *theDefinition)
{
	memset(theDefinition, NULL, theDefMap->mDefSize); // Initialize theDefinition to 0.
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++) // Iterate through each member variable of theDefinition
		if (aField->mFieldType == DefFieldType::DT_STRING) 
			*(char **)((uintptr_t)theDefinition + aField->mFieldOffset) = ""; // Assign all char* member variables to pointers to empty character arrays.
}

void DefinitionXmlError(XMLParser *theXmlParser, const char *theFormat, ...)
{
	va_list argList;
	va_start(argList, theFormat);
	std::string aFormattedMessage = SexyStringToString(vformat(theFormat, argList));
	va_end(argList);

	int aLine = theXmlParser->GetCurrentLineNum();
	std::string aFileName = theXmlParser->GetFileName();
	TodTraceAndLog("%s(%d): XML Definition Error: %s\n", aFileName.c_str(), aLine, aFormattedMessage.c_str());
}

bool DefinitionReadXMLString(XMLParser *theXmlParser, SexyString &theValue)
{
	XMLElement aXMLElement;
	if (!theXmlParser->NextElement(&aXMLElement)) // Read the next XML element
	{
		DefinitionXmlError(theXmlParser, "Missing element value");
		return false;
	}
	if (aXMLElement.mType == XMLElement::TYPE_END) // Processing ends when the end tag is read.
		return true;
	else if (aXMLElement.mType != XMLElement::TYPE_ELEMENT) // Apart from the closing tag, under normal circumstances, the content read here should be the defined main text.
	{
		DefinitionXmlError(theXmlParser, "unknown element type");
		return false;
	}

	theValue = aXMLElement.mValue; // Assigning values ​​to output parameters

	if (!theXmlParser->NextElement(&aXMLElement)) // Continue reading the next XML element
	{
		DefinitionXmlError(theXmlParser, "Can't read element end");
		return false;
	}
	if (aXMLElement.mType != XMLElement::TYPE_END) // Normally, what should be read here is the end tag.
	{
		DefinitionXmlError(theXmlParser, "Missing element end");
		return false;
	}
	return true;
}

bool DefSymbolValueFromString(DefSymbol *theSymbolMap, const char *theName, int *theResultValue)
{
	while (theSymbolMap->mSymbolName != nullptr)
	{
		if (stricmp(theName, theSymbolMap->mSymbolName) == 0)
		{
			*theResultValue = theSymbolMap->mSymbolValue;
			return true;
		}
		theSymbolMap++;
	}
	return false;
}

bool DefinitionReadIntField(XMLParser *theXmlParser, int *theValue)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (sexysscanf(aStringValue.c_str(), "%d", theValue) == 1)
		return true;

	DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
	return false;
}

bool DefinitionReadFloatField(XMLParser *theXmlParser, float *theValue)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (sexysscanf(aStringValue.c_str(), "%f", theValue) == 1)
		return true;

	DefinitionXmlError(theXmlParser, "Can't parse float value '%s'", aStringValue.c_str());
	return false;
}

bool DefinitionReadStringField(XMLParser *theXmlParser, char **theValue)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	*theValue = (char *)DefinitionAlloc(aStringValue.size() + 1);
	strcpy(*theValue, aStringValue.c_str());
	return true;
}

bool DefinitionReadEnumField(XMLParser *theXmlParser, int *theValue, DefSymbol *theSymbolMap)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (DefSymbolValueFromString(theSymbolMap, aStringValue.c_str(), theValue))
		return true;

	DefinitionXmlError(theXmlParser, "Can't parse enum value '%s'", aStringValue.c_str());
	return false;
}

bool DefinitionReadVector2Field(XMLParser *theXmlParser, SexyVector2 *theValue)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (sexysscanf(aStringValue.c_str(), "%f %f", theValue) == 1)
		return true;

	DefinitionXmlError(theXmlParser, "Can't parse vector2 value '%s'", aStringValue.c_str());
	return false;
}

bool DefinitionReadArrayField(XMLParser *theXmlParser, DefinitionArrayDef *theArray, DefField *theField)
{
	DefMap *aDefMap = (DefMap *)theField->mExtraData;
	
	if (theArray->mArrayCount == 0)
	{
		theArray->mArrayCount = 1;
		theArray->mArrayData = DefinitionAlloc(aDefMap->mDefSize);
	}
	else
	{
		// When theArray already contains elements, and the number of elements is a power of 2.
		if (theArray->mArrayCount >= 1 &&
			(theArray->mArrayCount == 1 || (theArray->mArrayCount & (theArray->mArrayCount - 1)) == 0))
		{

			void *anNewData = DefinitionAlloc(2 * theArray->mArrayCount * aDefMap->mDefSize);
			memcpy(anNewData, theArray->mArrayData, theArray->mArrayCount * aDefMap->mDefSize);
			DefinitionFree(theArray->mArrayData);
			theArray->mArrayData = anNewData;
		}
		theArray->mArrayCount++;
	}

	if (DefinitionLoadMap(theXmlParser,
						  aDefMap,
						  (unsigned char *)theArray->mArrayData + aDefMap->mDefSize * (theArray->mArrayCount - 1)))
		return true;

	DefinitionXmlError(theXmlParser, "failed to read sub def");
	return false;
}

DefSymbol gDefTrackEaseSymbols[] = {
	{TodCurves::CURVE_EASE_IN_OUT_WEAK, "EaseInOutWeak"},
	{TodCurves::CURVE_FAST_IN_OUT_WEAK, "FastInOutWeak"},
	{TodCurves::CURVE_EASE_IN_OUT, "EaseInOut"},
	{TodCurves::CURVE_FAST_IN_OUT, "FastInOut"},
	{TodCurves::CURVE_EASE_IN, "EaseIn"},
	{TodCurves::CURVE_EASE_OUT, "EaseOut"},
	{TodCurves::CURVE_EASE_SIN_WAVE, "EaseSinWave"},
	{TodCurves::CURVE_BOUNCE_FAST_MIDDLE, "BounceFastMiddle"},
	{TodCurves::CURVE_BOUNCE_SLOW_MIDDLE, "BounceSlowMiddle"},
	{TodCurves::CURVE_BOUNCE, "Bounce"},
	{TodCurves::CURVE_SIN_WAVE, "SinWave"},
	{TodCurves::CURVE_LINEAR, "Linear"},
};

// Read FloatTrackField from Definition XML
// Implementation originally by @Patoke, tweaked by @Electr0Gunner
bool DefinitionReadFloatTrackField(XMLParser *theXmlParser, FloatParameterTrack *theTrack)
{
	SexyString aStringValue;

	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	float aValue = 0;
	int aLen = 0;

	const char *aStringChars = aStringValue.c_str();
	size_t anIdx = 0;

	theTrack->mCountNodes = 0;

	std::vector<FloatParameterTrackNode> aFloatTrackVec = std::vector<FloatParameterTrackNode>();
	FloatParameterTrackNode aTrackNode = FloatParameterTrackNode();
	while (true)
	{
		if (anIdx >= aStringValue.length())
		{
			return false;
		}
		if (aStringChars[anIdx] == '\0')
			goto _m_break; // No empty strings allowed

		aTrackNode.mTime = -1;
		aTrackNode.mCurveType = TodCurves::CURVE_LINEAR;
		aTrackNode.mDistribution = TodCurves::CURVE_LINEAR;

		if (aStringChars[anIdx] == '[')
		{
			// <range>
			anIdx++;
			if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1)
				return false; // mLowValue
			anIdx += aLen;
			aTrackNode.mLowValue = aValue;
			aTrackNode.mHighValue = aValue;
			if (aStringChars[anIdx] != ']')
			{
				anIdx++; // space (' ')
				// <curve>
				for (size_t i = 0; i < sizeof(gDefTrackEaseSymbols) / sizeof(gDefTrackEaseSymbols[0]); ++i)
				{
					size_t aStrLen = strlen(gDefTrackEaseSymbols[i].mSymbolName);
					if (strncmp(gDefTrackEaseSymbols[i].mSymbolName, aStringChars + anIdx, aStrLen) ==
						0) // could be the distribution?
					{
						aTrackNode.mDistribution = (TodCurves)gDefTrackEaseSymbols[i].mSymbolValue;
						anIdx += aStrLen + 1; // Accounts for space (' '), expressions never end with a curve
						break;
					}
				}
				switch (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen)) // mHighValue
				{
				case 1: // Float read successfully
					anIdx += aLen;
					aTrackNode.mHighValue = aValue;
					break;
				case 0: // No float to read just continue
					break;
				default: // Something bad happened, panic!
					return false;
				}
			}
			if (aStringChars[anIdx] != ']')
				return false; // Invalid format
			anIdx++;
			if (aStringChars[anIdx] == '\0')
				goto _m_break; // Done!

			if (aStringChars[anIdx] == ',')
			{
				anIdx++;
				if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1)
					return false; // mTime
				anIdx += aLen;
				aTrackNode.mTime = aValue * 0.01;
			}
			if (aStringChars[anIdx] == '\0')
				goto _m_break; // Done!
			anIdx++;
		}
		else
		{
			// <norange>
			if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1)
				return false; // mLow/HighValue
			anIdx += aLen;
			aTrackNode.mLowValue = aValue;
			aTrackNode.mHighValue = aValue;

			if (aStringChars[anIdx] == '\0')
				goto _m_break; // Done!

			if (aStringChars[anIdx] == ',')
			{
				anIdx++;
				if (sexysscanf(aStringChars + anIdx, "%f%n", &aValue, &aLen) != 1)
					return false; // mTime
				anIdx += aLen;
				aTrackNode.mTime = aValue * 0.01;
			}
			if (aStringChars[anIdx] == '\0')
				goto _m_break; // Done!
			anIdx++;
			// <curve>
			for (size_t i = 0; i < sizeof(gDefTrackEaseSymbols) / sizeof(gDefTrackEaseSymbols[0]); ++i)
			{
				size_t aStrLen = strlen(gDefTrackEaseSymbols[i].mSymbolName);
				if (strncmp(gDefTrackEaseSymbols[i].mSymbolName, aStringChars + anIdx, aStrLen) == 0) // mCurveType
				{
					aTrackNode.mCurveType = (TodCurves)gDefTrackEaseSymbols[i].mSymbolValue;
					anIdx += aStrLen;
					if (aStringChars[anIdx] == '\0')
						goto _m_break; // Done!
					anIdx++;
					break;
				}
			}
		}

		aFloatTrackVec.push_back(aTrackNode);
	}
_m_break:
	aFloatTrackVec.push_back(aTrackNode);

	// Search forward for a timestamp:
	size_t aBaseIdx = 0;
	anIdx = 0;
	float high = 0.0;
	float low = 0.0;
	do
	{
		for (anIdx = aBaseIdx; anIdx < aFloatTrackVec.size(); ++anIdx)
		{
			if (aFloatTrackVec[anIdx].mTime >= 0.0)
			{
				// Found a timestamp!
				high = aFloatTrackVec[anIdx].mTime;
				goto _m_found; // Since we break out anIdx isn't incremented.
			}
		}
		// Didn't find another value, we're finished.
		// Since we did break, anIdx == aFloatTrackVec.size(), which means final value is set
		high = 1.0;
	_m_found:
		// Going backwards set previous timestamps
		for (size_t i = aBaseIdx; i < anIdx; ++i)
		{ // Iterate up to anIdx - 1
			float interp;
			if (((anIdx - 1) - aBaseIdx) != 0)
				interp = ((float)(i - aBaseIdx)) / ((float)((anIdx - 1) - aBaseIdx));
			else if (aBaseIdx == 0)
				interp = 0.0;
			else
				interp = 1.0;
			aFloatTrackVec[i].mTime = high * interp + low * (1 - interp);
		}
		// Start again
		aBaseIdx = anIdx + 1;
		low = high;
	} while (aBaseIdx < aFloatTrackVec.size());

	theTrack->mCountNodes = aFloatTrackVec.size();
	if (theTrack->mCountNodes > 0)
	{
		theTrack->mNodes = new FloatParameterTrackNode[theTrack->mCountNodes];
		std::memcpy(theTrack->mNodes, aFloatTrackVec.data(), theTrack->mCountNodes * sizeof(FloatParameterTrackNode));
	}
	else
	{
		theTrack->mNodes = nullptr;
	}

	return true;
}

bool DefinitionReadFlagField(XMLParser *theXmlParser,
							 const SexyString &theElementName,
							 uintptr_t *theResultValue,
							 DefSymbol *theSymbolMap)
{
	int aValue;
	if (!DefSymbolValueFromString(theSymbolMap, theElementName.c_str(), &aValue))
		return false;

	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	int aFlag;
	if (sexysscanf(aStringValue.c_str(), "%f %f", &aFlag) != 1)
	{
		DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
		return false;
	}

	if (aFlag)
	{
		*theResultValue |= 1 << aValue;
	}
	else
	{
		*theResultValue &= ~(1 << aValue);
	}
	return true;
}

bool DefinitionReadImageField(XMLParser *theXmlParser, Image **theImage)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (DefinitionLoadImage(theImage, aStringValue))
		return true;

	std::string aMessgae = StrFormat(
		"Failed to find image '%s' in %s", aStringValue.c_str(), theXmlParser->GetFileName().c_str());
	TodErrorMessageBox(aMessgae.c_str(), "Missing image");
}

bool DefinitionReadFontField(XMLParser *theXmlParser, Font **theFont)
{
	SexyString aStringValue;
	if (!DefinitionReadXMLString(theXmlParser, aStringValue))
		return false;

	if (DefinitionLoadFont(theFont, aStringValue))
		return true;

	std::string aMessgae = StrFormat("Failed to find font '%s' in %s", aStringValue.c_str(), theXmlParser->GetFileName().c_str());
	TodErrorMessageBox(aMessgae.c_str(), "Missing font");
}

bool DefinitionReadField(XMLParser *theXmlParser, DefMap *theDefMap, void *theDefinition, bool *theDone)
{
	if (theXmlParser->HasFailed())
		return false;

	XMLElement aXMLElement;

	if (!theXmlParser->NextElement(&aXMLElement))
	{
		*theDone = true; //We finished the file
		return true;
	}
	if (aXMLElement.mType == XMLElement::TYPE_END)
	{
		*theDone = true; //We reached the end of the field
		return true;
	}
	if (aXMLElement.mType !=
		XMLElement::TYPE_START) // Normally, the start tag should be read here, while other content is read in subsequent corresponding functions.
	{
		DefinitionXmlError(theXmlParser, "Missing element start");
		return false;
	}

	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		void *pVar = (void *)((uintptr_t)theDefinition + aField->mFieldOffset);
		if (aField->mFieldType == DefFieldType::DT_FLAGS &&
			DefinitionReadFlagField(
				theXmlParser, aXMLElement.mValue, (uintptr_t *)pVar, (DefSymbol *)aField->mExtraData))
			return true;

		if (stricmp(aXMLElement.mValue.c_str(), aField->mFieldName) == 0) // Determine if aXMLElement is defined as the member variable.
		{
			bool aSuccess = false;
			switch (aField->mFieldType)
			{
			case DefFieldType::DT_INT:
				aSuccess = DefinitionReadIntField(theXmlParser, (int *)pVar);
				break;
			case DefFieldType::DT_FLOAT:
				aSuccess = DefinitionReadFloatField(theXmlParser, (float *)pVar);
				break;
			case DefFieldType::DT_STRING:
				aSuccess = DefinitionReadStringField(theXmlParser, (char **)pVar);
				break;
			case DefFieldType::DT_ENUM:
				aSuccess = DefinitionReadEnumField(theXmlParser, (int *)pVar, (DefSymbol *)aField->mExtraData);
				break;
			case DefFieldType::DT_VECTOR2:
				aSuccess = DefinitionReadVector2Field(theXmlParser, (SexyVector2 *)pVar);
				break;
			case DefFieldType::DT_ARRAY:
				aSuccess = DefinitionReadArrayField(theXmlParser, (DefinitionArrayDef *)pVar, aField);
				break;
			case DefFieldType::DT_TRACK_FLOAT:
				aSuccess = DefinitionReadFloatTrackField(theXmlParser, (FloatParameterTrack *)pVar);
				break;
			case DefFieldType::DT_IMAGE:
				aSuccess = DefinitionReadImageField(theXmlParser, (Image **)pVar);
				break;
			case DefFieldType::DT_FONT:
				aSuccess = DefinitionReadFontField(theXmlParser, (Font **)pVar);
				break;
			default:
				TOD_ASSERT(false);
				break;
			}
			if (aSuccess)
				return true;

			DefinitionXmlError(theXmlParser, "Failed to read '%s' field", aXMLElement.mValue.c_str());
			return false;
		}
	}
	DefinitionXmlError(
		theXmlParser, "Ignoring unknown element '%s'", aXMLElement.mValue.c_str()); // When no member variables are defined in aXMLElement
	return false;
}

bool DefinitionLoadMap(XMLParser *theXmlParser, DefMap *theDefMap, void *theDefinition)
{
	if (theDefMap->mConstructorFunc)
		theDefMap->mConstructorFunc(theDefinition); // Construct theDefinition using the constructor
	else
		DefinitionFillWithDefaults(theDefMap, theDefinition); // TheDefinition is populated with the default value.

	bool aDone = false;
	while (!aDone)
		if (!DefinitionReadField(theXmlParser, theDefMap, theDefinition, &aDone))
			return false;
	return true;
}

void DefWriteToCacheFlag(DefinitionCompiler *theWritePtr, uint32_t *theValue)
{
	if (theValue == nullptr)
	{
		theWritePtr->Write(0);
		return;
	}
	
}

void DefWriteToCacheString(DefinitionCompiler *theWritePtr, const char *theValue)
{
	if (theValue == nullptr)
	{
		theWritePtr->Write(0);
		return;
	}
	uint32_t aLen = (uint32_t)strlen(theValue);
	theWritePtr->Write(aLen);
	if (aLen > 0)
		theWritePtr->Write(theValue, aLen);
}

void DefWriteToCacheImage(DefinitionCompiler *theWritePtr, Image **theValue)
{
	std::string aImageName{};
	if ((*theValue) != nullptr)
		TodFindImagePath(*theValue, &aImageName);

	uint32_t aImageSize = aImageName.length();
	theWritePtr->Write(aImageSize);
	if (aImageSize > 0)
		theWritePtr->Write(aImageName.data(), aImageSize);
}

void DefWriteToCacheFont(DefinitionCompiler *theWritePtr, Font **theValue)
{
	std::string aFontName{};
	if ((*theValue) != nullptr)
		TodFindFontPath(*theValue, &aFontName);

	uint32_t aFontSize = aFontName.length();
	theWritePtr->Write(aFontSize);
	if (aFontSize > 0)
		theWritePtr->Write(aFontName.data(), aFontSize);
}

void DefWriteToCacheArray(DefinitionCompiler *theWritePtr, DefinitionArrayDef *theValue, DefMap *theDefMap)
{
	if (theValue == nullptr)
	{
		theWritePtr->Write(0);
		return;
	}
	theWritePtr->Write(theValue->mArrayCount);
	for (uint32_t i = 0; i < theValue->mArrayCount; i++)
	{
		void *elem = (uint8_t *)theValue->mArrayData + i * theDefMap->mDefSize;
		DefMapWriteToCache(theWritePtr, theDefMap, elem);
	}

}

void DefWriteToCacheFloatTrack(DefinitionCompiler *theWritePtr, FloatParameterTrack *theValue)
{
	if (theValue == nullptr)
	{
		theWritePtr->Write(0);
		return;
	}
	theWritePtr->Write(theValue->mCountNodes);
	if (theValue->mCountNodes > 0)
	{
		theWritePtr->Write(theValue->mNodes, theValue->mCountNodes * sizeof(FloatParameterTrackNode));
	}
}

void DefWriteToCacheVector2(DefinitionCompiler *theWritePtr, SexyVector2 *theValue)
{
	if (theValue == nullptr)
	{
		theWritePtr->Write(SexyVector2(0, 0));
		return;
	}
	theWritePtr->Write(theValue);
}

void DefMapWriteToCache(DefinitionCompiler *theWritePtr, DefMap *theDefMap, void *theDefinition)
{
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		void *aSrc = (void *)((uintptr_t)theDefinition + aField->mFieldOffset);
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_ENUM:
		case DefFieldType::DT_INT:
		{
			int value = *(int *)aSrc;
			theWritePtr->Write(value);
			break;
		}
		case DefFieldType::DT_FLOAT:
		{
			float value = *(float *)aSrc;
			theWritePtr->Write(value);
			break;
		}
		case DefFieldType::DT_FLAGS:
			theWritePtr->Write(*(uint32_t *)aSrc);
			break;
		case DefFieldType::DT_VECTOR2:
			DefWriteToCacheVector2(theWritePtr, (SexyVector2 *)aSrc);
			break;

		case DefFieldType::DT_STRING:
			DefWriteToCacheString(theWritePtr, *(char **)aSrc);
			break;

		case DefFieldType::DT_ARRAY:
			DefWriteToCacheArray(theWritePtr, (DefinitionArrayDef *)aSrc, (DefMap *)aField->mExtraData);
			break;

		case DefFieldType::DT_TRACK_FLOAT:
			DefWriteToCacheFloatTrack(theWritePtr, (FloatParameterTrack *)aSrc);
			break;

		case DefFieldType::DT_IMAGE:
			DefWriteToCacheImage(theWritePtr, (Image **)aSrc);
			break;

		case DefFieldType::DT_FONT:
			DefWriteToCacheFont(theWritePtr, (Font **)aSrc);
			break;

		default:
			break;
		}
	}
}

bool DefinitionWriteCompiledFile(const SexyString &theCompiledFilePath, DefMap *theDefMap, void *theDefinition)
{
	CompiledDefinitionHeader aHeader;
	aHeader.mCookie = COMPILED_DEFINITION_MAGIC;
	DefinitionCompiler aCompiler;
	DefMapWriteToCache(&aCompiler, theDefMap, theDefinition);
	aHeader.mUncompressedSize = (uint32_t)aCompiler.mBuffer.size();
	aHeader.mDataOffset = sizeof(CompiledDefinitionHeader);


	uLongf aCompressedSize = compressBound(aCompiler.mBuffer.size());
	void *aCompressedData = DefinitionAlloc(aCompressedSize);
	int res = compress(
		(Bytef *)aCompressedData, &aCompressedSize, (Bytef *)aCompiler.mBuffer.data(), aHeader.mUncompressedSize);

	if (res != Z_OK)
	{
		TodTrace("Failed to compress file: %s\nZLib Error: %d", theCompiledFilePath.c_str(), res);
		DefinitionFree(aCompressedData);
		return false;
	}
	std::filesystem::path path(theCompiledFilePath.c_str());
	std::filesystem::create_directories(path.parent_path());
	std::ofstream anOut(path.c_str(), std::ios::binary);
	anOut.write((char *)&aHeader, sizeof(aHeader));
	anOut.write((char *)aCompressedData, aCompressedSize);

	DefinitionFree(aCompressedData);
	return true;
}

bool DefinitionCompileFile(const SexyString theXMLFilePath,
						   const SexyString &theCompiledFilePath,
						   DefMap *theDefMap,
						   void *theDefinition)
{
	XMLParser aXMLParser;
	if (!aXMLParser.OpenFile(theXMLFilePath))
	{
		TodTrace("XML file not found: %s\n", theXMLFilePath.c_str());
		return false;
	}
	else if (!DefinitionLoadMap(&aXMLParser, theDefMap, theDefinition))
		return false;

	DefinitionWriteCompiledFile(theCompiledFilePath, theDefMap, theDefinition);
	return true;
}

bool DefinitionCompileAndLoad(const SexyString &theXMLFilePath, DefMap *theDefMap, void *theDefinition)
{
	SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
	TodHesitationTrace("predef");
	
	try
	{
		if (DefinitionReadCompiledFile(aCompiledFilePath, theDefMap, theDefinition))
		{
			TodHesitationTrace("loaded %s", aCompiledFilePath.c_str());
			return true;
		}
	}
	catch (int anErrorCode) {}

	PerfTimer aTimer;
	aTimer.Start();
	bool aResult = DefinitionCompileFile(theXMLFilePath, "fresh_" + aCompiledFilePath, theDefMap, theDefinition); //write to fresh_compiled to not overwrite on game re-compile by accident
	TodTrace("compile %d ms:'%s'", (int)aTimer.GetDuration(), aCompiledFilePath.c_str());
	TodHesitationTrace("compiled %s", aCompiledFilePath.c_str());
	if (aResult)
		return aResult;

	TodErrorMessageBox(StrFormat("missing resource %s", aCompiledFilePath.c_str()).c_str(), "Error");
	exit(0);
}

float FloatTrackEvaluate(FloatParameterTrack &theTrack, float theTimeValue, float theInterp)
{
	if (theTrack.mCountNodes == 0)
		return 0.0f;

	if (theTimeValue < theTrack.mNodes[0].mTime) // If the current time is less than the start time of the first node
		return TodCurveEvaluate(
			theInterp, theTrack.mNodes[0].mLowValue, theTrack.mNodes[0].mHighValue, theTrack.mNodes[0].mDistribution);

	for (int i = 1; i < theTrack.mCountNodes; i++)
	{
		FloatParameterTrackNode *aNodeNxt = &theTrack.mNodes[i];
		if (theTimeValue <= aNodeNxt->mTime) // Find the first node whose start time is greater than the current time.
		{
			FloatParameterTrackNode *aNodeCur = &theTrack.mNodes[i - 1];
			// Calculate the progress from the current node to the next node at the current time.
			float aTimeFraction = (theTimeValue - aNodeCur->mTime) / (aNodeNxt->mTime - aNodeCur->mTime);
			float aLeftValue =
				TodCurveEvaluate(theInterp, aNodeCur->mLowValue, aNodeCur->mHighValue, aNodeCur->mDistribution);
			float aRightValue =
				TodCurveEvaluate(theInterp, aNodeNxt->mLowValue, aNodeNxt->mHighValue, aNodeNxt->mDistribution);
			return TodCurveEvaluate(aTimeFraction, aLeftValue, aRightValue, aNodeCur->mCurveType);
		}
	}

	FloatParameterTrackNode *aLastNode =
		&theTrack.mNodes[theTrack.mCountNodes - 1]; // If the current time is greater than the start time of the last node
	return TodCurveEvaluate(theInterp, aLastNode->mLowValue, aLastNode->mHighValue, aLastNode->mDistribution);
}

void FloatTrackSetDefault(FloatParameterTrack &theTrack, float theValue)
{
	if (theTrack.mNodes == nullptr && theValue != 0.0f) // Ensure that this parameter has no nodes (has not been assigned a value) and that the given default value is not 0.
	{
		theTrack.mCountNodes = 1; // The default parameter track has exactly one node.
		FloatParameterTrackNode *aNode = (FloatParameterTrackNode *)DefinitionAlloc(sizeof(FloatParameterTrackNode));
		theTrack.mNodes = aNode;
		aNode->mTime = 0.0f;
		aNode->mLowValue = theValue;
		aNode->mHighValue = theValue;
		aNode->mCurveType = TodCurves::CURVE_CONSTANT;
		aNode->mDistribution = TodCurves::CURVE_LINEAR;
	}
}

bool FloatTrackIsSet(const FloatParameterTrack &theTrack)
{
	return theTrack.mCountNodes != 0 && theTrack.mNodes[0].mCurveType != TodCurves::CURVE_CONSTANT;
}

bool FloatTrackIsConstantZero(FloatParameterTrack &theTrack)
{
	// When a track has no nodes, or has only one node whose maximum and minimum values ​​are both 0, the values ​​on that track are considered to be always zero.
	return theTrack.mCountNodes == 0 ||
		   (theTrack.mCountNodes == 1 && theTrack.mNodes[0].mLowValue == 0.0f && theTrack.mNodes[0].mHighValue == 0.0f);
}

float FloatTrackEvaluateFromLastTime(FloatParameterTrack &theTrack, float theTimeValue, float theInterp)
{
	return theTimeValue < 0.0f ? 0.0f : FloatTrackEvaluate(theTrack, theTimeValue, theInterp);
}

void DefinitionFreeArrayField(DefinitionArrayDef *theArray, DefMap *theDefMap)
{
	for (int i = 0; i < theArray->mArrayCount; i++)
		DefinitionFreeMap(theDefMap,
			(void *)((uintptr_t)theArray->mArrayData + theDefMap->mDefSize * i)); // The last parameter represents pData[i].
	DefinitionFree(theArray->mArrayData);
}

void DefinitionFreeMap(DefMap *theDefMap, void *theDefinition)
{
	// Iterate through each member variable of theDefinition using theDefMap.
	for (DefField *aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
	{
		void *aVar = (void *)((uintptr_t)theDefinition + aField->mFieldOffset); // pointer to this member variable
		switch (aField->mFieldType)
		{
		case DefFieldType::DT_STRING:
		{
			char *aStr = *(char **)aVar;
			if (aStr == nullptr)
				continue;
			if (*aStr != '\0')
				delete[] aStr; // Release character array

			*(char **)aVar = nullptr;
			break;
		}
		case DefFieldType::DT_ARRAY:
			DefinitionFreeArrayField((DefinitionArrayDef *)aVar, (DefMap *)aField->mExtraData);
			break;
		case DefFieldType::DT_TRACK_FLOAT:
		{
			FloatParameterTrack* aCastedField = (FloatParameterTrack *)aVar;
			if (aCastedField->mCountNodes != 0)
				delete[] aCastedField->mNodes; // Release the node of the floating-point parameter track
			aCastedField->mNodes = nullptr;
			break;
		}
		}
	}
}