#include <windows.h>
#include "ImageLib.h"
#include <math.h>
#include <tchar.h>
#include "..\PakLib\PakInterface.h"
#include <vector>
#include <cstdint>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

//#include "jpeg2000/jasper.h"

using namespace ImageLib;

Image::Image()
{
	mWidth = 0;
	mHeight = 0;
	mNumChannels = 0;
	mBits = NULL;
}

Image::~Image()
{
	delete[] mBits;
}

int Image::GetWidth()
{
	return mWidth;
}

int Image::GetHeight()
{
	return mHeight;
}

unsigned long *Image::GetBits()
{
	return mBits;
}

bool ImageLib::WriteImage(const std::string &theFileName,
						  Image *theImage,
						  const std::string &theExtension,
						  bool add_extension)
{
	uint8_t *data = new uint8_t[theImage->mWidth * theImage->mHeight * theImage->mNumChannels];

	unsigned long *aBits = theImage->GetBits();
	int index = 0;

	for (int j = 0; j < theImage->mHeight; ++j)
	{
		for (int i = 0; i < theImage->mWidth; ++i)
		{
			unsigned long px = aBits[j * theImage->mWidth + i];
			data[index++] = (px >> 16) & 0xFF; // R
			data[index++] = (px >> 8) & 0xFF;  // G
			data[index++] = (px) & 0xFF;	   // B
			if (theImage->mNumChannels == 4)
				data[index++] = (px >> 24) & 0xFF; // A
		}
	}

	std::string aFilename = add_extension ? theFileName + theExtension : theFileName;

	if (stricmp(theExtension.c_str(), ".jpg") == 0 || stricmp(theExtension.c_str(), ".jpeg") == 0)
		stbi_write_jpg(aFilename.c_str(), theImage->mWidth, theImage->mHeight, 3, data, 100);
	else if (stricmp(theExtension.c_str(), ".png") == 0)
		stbi_write_png(aFilename.c_str(),
					   theImage->mWidth,
					   theImage->mHeight,
					   theImage->mNumChannels,
					   data,
					   theImage->mWidth * theImage->mNumChannels);
	else if (stricmp(theExtension.c_str(), ".bmp") == 0)
		stbi_write_bmp(aFilename.c_str(), theImage->mWidth, theImage->mHeight, theImage->mNumChannels, data);
	else if (stricmp(theExtension.c_str(), ".tga") == 0)
		stbi_write_tga(aFilename.c_str(), theImage->mWidth, theImage->mHeight, theImage->mNumChannels, data);
	else
	{
		delete[] data;
		return false;
	}

	delete[] data;

	return true;
}

int ImageLib::gAlphaComposeColor = 0xFFFFFF;
bool ImageLib::gAutoLoadAlpha = true;

Image *ImageLib::GetImageBackend(const std::string &theFileName, const std::string &theExtension)
{
	PFILE *fp;
	if ((fp = p_fopen((theFileName + theExtension).c_str(), "rb")) == nullptr)
	{
		return nullptr;
	}

	p_fseek(fp, 0, SEEK_END);
	size_t fileSize = p_ftell(fp);
	p_fseek(fp, 0, SEEK_SET);
	std::vector<uint8_t> data(fileSize);
	p_fread(data.data(), 1, fileSize, fp);
	p_fclose(fp);

	int width, height, num_channels, frame_count;
	int *delays;
	unsigned char *stb_image;
	if (theExtension != ".gif")
	{
		stb_image = stbi_load_from_memory(data.data(), fileSize, &width, &height, &num_channels, 0);
		frame_count = 0;
	}
	else
	{
		stb_image =
			stbi_load_gif_from_memory(data.data(), fileSize, &delays, &width, &height, &frame_count, &num_channels, 0);

		if (delays)
			free(delays);
	}

	unsigned long *aBits = new unsigned long[width * height];
	for (int i = 0; i < width * height; ++i)
	{
		unsigned char *pixel = &stb_image[i * num_channels];

		unsigned char r = pixel[0];
		unsigned char g = pixel[1];
		unsigned char b = pixel[2];
		unsigned char a = (num_channels == 4) ? pixel[3] : 0xFF;

		aBits[i] = (a << 24) | (r << 16) | (g << 8) | b;
	}

	Image *anImage = new Image();
	anImage->mWidth = width;
	anImage->mHeight = height;
	anImage->mBits = aBits;
	anImage->mNumChannels = num_channels;

	stbi_image_free(stb_image);

	return anImage;
}

Image *ImageLib::GetImage(const std::string &theFilename, bool lookForAlphaImage)
{
	if (!gAutoLoadAlpha)
		lookForAlphaImage = false;

	int aLastDotPos = theFilename.rfind('.');
	int aLastSlashPos = max((int)theFilename.rfind('\\'), (int)theFilename.rfind('/'));

	std::string anExt;
	std::string aFilename;

	if (aLastDotPos > aLastSlashPos)
	{
		anExt = theFilename.substr(aLastDotPos, theFilename.length() - aLastDotPos);
		aFilename = theFilename.substr(0, aLastDotPos);
	}
	else
		aFilename = theFilename;

	Image *anImage = NULL;

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".tga") == 0) || (anExt.length() == 0)))
		anImage = GetImageBackend(aFilename, ".tga");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".jpg") == 0) || (anExt.length() == 0)))
		anImage = GetImageBackend(aFilename, ".jpg");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".png") == 0) || (anExt.length() == 0)))
		anImage = GetImageBackend(aFilename, ".png");

	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".gif") == 0) || (anExt.length() == 0)))
		anImage = GetImageBackend(aFilename, ".gif");

	// Check for alpha images
	Image* anAlphaImage = NULL;
	if(lookForAlphaImage)
	{
		// Check _ImageName
		anAlphaImage = GetImage(theFilename.substr(0, aLastSlashPos+1) + "_" +
			theFilename.substr(aLastSlashPos+1, theFilename.length() - aLastSlashPos - 1), false);

		// Check ImageName_
		if(anAlphaImage==NULL)
			anAlphaImage = GetImage(theFilename + "_", false);
	}



	// Compose alpha channel with image
	if (anAlphaImage != NULL) 
	{
		if (anImage != NULL)
		{
			if ((anImage->mWidth == anAlphaImage->mWidth) &&
				(anImage->mHeight == anAlphaImage->mHeight))
			{
				unsigned long* aBits1 = anImage->mBits;
				unsigned long* aBits2 = anAlphaImage->mBits;
				int aSize = anImage->mWidth*anImage->mHeight;

				for (int i = 0; i < aSize; i++)
				{
					*aBits1 = (*aBits1 & 0x00FFFFFF) | ((*aBits2 & 0xFF) << 24);
					++aBits1;
					++aBits2;
				}
			}

			delete anAlphaImage;
		}
		else if (gAlphaComposeColor==0xFFFFFF)
		{
			anImage = anAlphaImage;

			unsigned long* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = (0x00FFFFFF) | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
		else
		{
			const int aColor = gAlphaComposeColor;
			anImage = anAlphaImage;

			unsigned long* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = aColor | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
	}

	return anImage;
}