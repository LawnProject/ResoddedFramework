#pragma once

#include <vector>
#include <stdint.h>

#define COMPILED_DEFINITION_MAGIC 0x43444631

// ====================================================================================================
// Compress definition data header
// ----------------------------------------------------------------------------------------------------
// Header defining the important data of a compiled definition.
// ====================================================================================================
class CompiledDefinitionHeader
{
  public:
	uint32_t mCookie;	// Cache values ​​used for compression verification
	uint32_t mUncompressedSize; // Length of uncompressed data
	uint32_t mDataOffset;		// Where the data starts
};

class CompiledDefinitionFile
{
  public:
	std::vector<uint8_t> mData;

	bool isValid() const
	{
		if (mData.data() == nullptr || mData.size() < sizeof(CompiledDefinitionHeader))
			return false;

		auto *aHeader = GetHeader();

		if (aHeader->mCookie != COMPILED_DEFINITION_MAGIC)
			return false;

		return true;
	}

	const CompiledDefinitionHeader* GetHeader() const
	{
		return reinterpret_cast<const CompiledDefinitionHeader *>(mData.data());
	}

	uint8_t* GetCompressedData()
	{
		return mData.data() + GetHeader()->mDataOffset;
	}
};


class DefinitionCompiler
{
  public:
	std::vector<uint8_t> mBuffer;

	void Write(const void* theSrc, size_t theSize)
	{
		size_t anOffset = mBuffer.size();
		mBuffer.resize(anOffset + theSize);
		memcpy(mBuffer.data() + anOffset, theSrc, theSize);
	}

	template <typename T> 
	void Write(const T &value)
	{
		static_assert(std::is_trivially_copyable_v<T>, "Type cannot be copied");
		Write(&value, sizeof(T));
	}
};