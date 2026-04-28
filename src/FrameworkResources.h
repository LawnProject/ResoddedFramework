#pragma once
namespace Sexy
{
	class ResourceManager;
	class Image;
	class Font;
	Image* ResoddedFrameworkLoadImageById(ResourceManager *theManager, int theId);	void ResoddedFrameworkReplaceImageById(ResourceManager *theManager, int theId, Image *theImage);
	bool ResoddedFrameworkExtractResourcesByName(ResourceManager *theManager, const char *theName);

	bool ResoddedFrameworkExtractResourcesByName(ResourceManager *theManager, const char *theName);

	// LoadingImages Resources
	bool ResoddedFrameworkExtractLoadingImagesResources(ResourceManager *theMgr);
	extern Image* IMAGE_SEED_SELECTOR;
	extern Image* IMAGE_GAMEPAD_CURSOR_FRAME;
	extern Image* IMAGE_GAMEPAD_CURSOR_ARROWS;
	extern Image* IMAGE_GAMEPAD_CURSOR_ARROWS_SHADOW;

	enum ResoddedFrameworkResourceId
	{
		IMAGE_SEED_SELECTOR_ID,
		IMAGE_GAMEPAD_CURSOR_FRAME_ID,
		IMAGE_GAMEPAD_CURSOR_ARROWS_ID,
		IMAGE_GAMEPAD_CURSOR_ARROWS_SHADOW_ID,
		ResoddedFrameworkRESOURCE_ID_MAX
	};

	Image* ResoddedFrameworkGetImageById(int theId);
	Font* ResoddedFrameworkGetFontById(int theId);
	int ResoddedFrameworkGetSoundById(int theId);

	Image*& ResoddedFrameworkGetImageRefById(int theId);
	Font*& ResoddedFrameworkGetFontRefById(int theId);
	int& ResoddedFrameworkGetSoundRefById(int theId);

	ResoddedFrameworkResourceId ResoddedFrameworkGetIdByImage(Image *theImage);
	ResoddedFrameworkResourceId ResoddedFrameworkGetIdByFont(Font *theFont);
	ResoddedFrameworkResourceId ResoddedFrameworkGetIdBySound(int theSound);

	const char* ResoddedFrameworkGetStringIdById(int theId);
	ResoddedFrameworkResourceId ResoddedFrameworkGetIdByStringId(const char *theStringId);

} // namespace Sexy

