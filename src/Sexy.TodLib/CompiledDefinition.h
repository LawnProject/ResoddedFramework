#pragma once

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
