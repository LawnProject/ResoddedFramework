#include "FrameworkResources.h"
#include "SexyAppFramework/ResourceManager.h"
using namespace Sexy;

#pragma warning(disable:4311 4312)

static bool gNeedRecalcVariableToIdMap = false;

bool Sexy::ResoddedFrameworkExtractResourcesByName(ResourceManager * theManager, const char* theName)
{
	if (strcmp(theName,"LoadingImages")==0) return ResoddedFrameworkExtractLoadingImagesResources(theManager);
	return false;
}

Sexy::ResoddedFrameworkResourceId Sexy::ResoddedFrameworkGetIdByStringId(const char *theStringId)
{
	typedef std::map<std::string,int> MyMap;
	static MyMap aMap;
	if(aMap.empty())
	{
		for(int i=0; i<ResoddedFrameworkRESOURCE_ID_MAX; i++)
			aMap[ResoddedFrameworkGetStringIdById(i)] = i;
	}

	MyMap::iterator anItr = aMap.find(theStringId);
	if (anItr == aMap.end())
		return ResoddedFrameworkRESOURCE_ID_MAX;
	else
		return (ResoddedFrameworkResourceId) anItr->second;
}

// LoadingImages Resources
Image* Sexy::IMAGE_SEED_SELECTOR;
Image* Sexy::IMAGE_GAMEPAD_CURSOR_FRAME;
Image* Sexy::IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW;
Image* Sexy::IMAGE_GAMEPAD_SELECTOR_SHADOW;
Image* Sexy::IMAGE_GAMEPAD_IMAGE_P1;

bool Sexy::ResoddedFrameworkExtractLoadingImagesResources(ResourceManager *theManager)
{
	gNeedRecalcVariableToIdMap = true;

	ResourceManager &aMgr = *theManager;
	try
	{
		IMAGE_SEED_SELECTOR = aMgr.GetImageThrow("IMAGE_SEED_SELECTOR");
		IMAGE_GAMEPAD_CURSOR_FRAME = aMgr.GetImageThrow("IMAGE_GAMEPAD_CURSOR_FRAME");
		IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW = aMgr.GetImageThrow("IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW");
		IMAGE_GAMEPAD_SELECTOR_SHADOW = aMgr.GetImageThrow("IMAGE_GAMEPAD_SELECTOR_SHADOW");
		IMAGE_GAMEPAD_IMAGE_P1 = aMgr.GetImageThrow("IMAGE_GAMEPAD_IMAGE_P1");
	}
	catch(ResourceManagerException&)
	{
		return false;
	}
	return true;
}

static void* gResources[] =
{
	&IMAGE_SEED_SELECTOR,
	&IMAGE_GAMEPAD_CURSOR_FRAME,
	&IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW,
	&IMAGE_GAMEPAD_SELECTOR_SHADOW,
	&IMAGE_GAMEPAD_IMAGE_P1,
	nullptr
};

Image* Sexy::ResoddedFrameworkLoadImageById(ResourceManager *theManager, int theId)
{
	return (*((Image**)gResources[theId]) = theManager->LoadImage(ResoddedFrameworkGetStringIdById(theId)));
}

void Sexy::ResoddedFrameworkReplaceImageById(ResourceManager *theManager, int theId, Image *theImage)
{
	theManager->ReplaceImage(ResoddedFrameworkGetStringIdById(theId),theImage);
	 * (Image**)gResources[theId] = theImage; 
}

Image* Sexy::ResoddedFrameworkGetImageById(int theId)
{
	return *(Image**)gResources[theId];
}

Font* Sexy::ResoddedFrameworkGetFontById(int theId)
{
	return *(Font**)gResources[theId];
}

int Sexy::ResoddedFrameworkGetSoundById(int theId)
{
	return *(int*)gResources[theId];
}

Image*& Sexy::ResoddedFrameworkGetImageRefById(int theId)
{
	return *(Image**)gResources[theId];
}

Font*& Sexy::ResoddedFrameworkGetFontRefById(int theId)
{
	return *(Font**)gResources[theId];
}

int& Sexy::ResoddedFrameworkGetSoundRefById(int theId)
{
	return *(int*)gResources[theId];
}

static Sexy::ResoddedFrameworkResourceId ResoddedFrameworkGetIdByVariable(const void *theVariable)
{
	typedef std::map<int,int> MyMap;	static MyMap aMap;
	if(gNeedRecalcVariableToIdMap)
	{
		gNeedRecalcVariableToIdMap = false;
		aMap.clear();
		for(int i=0; i<ResoddedFrameworkRESOURCE_ID_MAX; i++)
			aMap[*(int*)gResources[i]] = i;
	}

	MyMap::iterator anItr = aMap.find((int)theVariable);
	if (anItr == aMap.end())
		return ResoddedFrameworkRESOURCE_ID_MAX;
	else
		return (ResoddedFrameworkResourceId) anItr->second;
}

Sexy::ResoddedFrameworkResourceId Sexy::ResoddedFrameworkGetIdByImage(Image *theImage)
{
	return ResoddedFrameworkGetIdByVariable(theImage);
}

Sexy::ResoddedFrameworkResourceId Sexy::ResoddedFrameworkGetIdByFont(Font *theFont)
{
	return ResoddedFrameworkGetIdByVariable(theFont);
}

Sexy::ResoddedFrameworkResourceId Sexy::ResoddedFrameworkGetIdBySound(int theSound)
{
	return ResoddedFrameworkGetIdByVariable((void*)theSound);
}

const char* Sexy::ResoddedFrameworkGetStringIdById(int theId)
{
	switch(theId)
	{
		case IMAGE_SEED_SELECTOR_ID: return "IMAGE_SEED_SELECTOR";
		case IMAGE_GAMEPAD_CURSOR_FRAME_ID: return "IMAGE_GAMEPAD_CURSOR_FRAME";
		case IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW_ID: return "IMAGE_GAMEPAD_CURSOR_FRAME_SHADOW";
		case IMAGE_GAMEPAD_SELECTOR_SHADOW_ID: return "IMAGE_GAMEPAD_SELECTOR_SHADOW";
		case IMAGE_GAMEPAD_IMAGE_P1_ID: return "IMAGE_GAMEPAD_IMAGE_P1";
		default: return "";
	}
}


