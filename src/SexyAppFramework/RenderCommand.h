#pragma once

#include <array>
#include "Rect.h"
#include "Color.h"

namespace Sexy
{

class Image;

class RenderCommand
{
  public:
	Image *mImage{};
	int mDestX;
	int mDestY;
	Rect mSrc;
	int mMode = -1;
	Color mColor{};
	RenderCommand *mNext{};
};

class RenderCommandList
{
	static constexpr int LIST_SIZE = 256;

	class List
	{
	  public:
		RenderCommand *mHead{};
		RenderCommand *mTail{};
	};
	using ListArray = std::array<RenderCommandList::List, LIST_SIZE>;

	class iterator
	{
		friend class RenderCommandList;

	  public:
		RenderCommand &operator*() const
		{
			return *mCurrent;
		}
		RenderCommand *operator->() const
		{
			return mCurrent;
		}
		// pre increment
		iterator &operator++()
		{
			if (mOrder >= LIST_SIZE)
			{
				return *this; // reached end
			}
			if (mCurrent)
			{
				mCurrent = mCurrent->mNext;
			}
			while (!mCurrent)
			{
				if (mOrder < LIST_SIZE)
				{
					++mOrder;
				}
				if (mOrder >= LIST_SIZE)
				{
					return *this; // reached end
				}
				mCurrent = mLists[mOrder].mHead;
			}
			return *this;
		}
		// post increment
		iterator operator++(int)
		{
			iterator old = *this;
			++(*this);
			return old;
		}
		friend bool operator==(const iterator &a, const iterator &b)
		{
			return a.mOrder == b.mOrder && a.mCurrent == b.mCurrent;
		}
		friend bool operator!=(const iterator &a, const iterator &b)
		{
			return !(a == b);
		}

	  private:
		iterator(ListArray &lists, int order) : mLists(lists), mOrder(order)
		{
			if (order >= 0 && order < LIST_SIZE)
			{
				mCurrent = mLists[order].mHead;
			}
			else
			{
				mCurrent = nullptr;
			}
		}
		ListArray &mLists;
		int mOrder;
		RenderCommand *mCurrent;
	};

  public:
	void Clear()
	{
		mLists = {};
		mBufferNextIdx = 0;
	}
	RenderCommand *PushWithOrder(int order)
	{
		auto *command = Allocate();
		if (command)
		{
			int orderIdx = std::min(std::max(order + 128, 0), 255);
			auto &list = mLists[orderIdx];
			if (list.mTail)
			{
				list.mTail->mNext = command;
				list.mTail = command;
			}
			else
			{
				list.mHead = command;
				list.mTail = command;
			}
		}
		return command;
	}
	iterator begin()
	{
		iterator it(mLists, -1);
		++it; // advance to first element
		return it;
	}
	iterator end()
	{
		return { mLists, LIST_SIZE };
	}

  private:
	RenderCommand *Allocate()
	{
		if (mBufferNextIdx >= mBuffer.size())
		{
			return nullptr;
		}
		auto *command = &mBuffer[mBufferNextIdx++];
		new (command) RenderCommand;
		return command;
	}
	std::array<RenderCommand, 4096> mBuffer{};
	int mBufferNextIdx{};
	ListArray mLists{};
};

} // namespace Sexy
