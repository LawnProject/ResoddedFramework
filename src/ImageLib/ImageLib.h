#ifndef __IMAGELIB_H__
#define __IMAGELIB_H__

#include <string>

namespace ImageLib
{

class Image
{
  public:
	int mWidth;
	int mHeight;
	int mNumChannels;
	uint32_t *mBits;

  public:
	Image();
	virtual ~Image();

	int GetWidth();
	int GetHeight();
	uint32_t *GetBits();
};

bool WriteImage(const std::string &theFileName,
				Image *theImage,
				const std::string &theExtension,
				bool add_extension = true);
extern int gAlphaComposeColor;
extern bool gAutoLoadAlpha;

Image *GetImageBackend(const std::string &theFileName, const std::string &theExtension);
Image *GetImage(const std::string &theFileName, bool lookForAlphaImage = true);

} // namespace ImageLib

#endif //__IMAGELIB_H__