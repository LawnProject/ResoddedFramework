#pragma once

#include <utf8cpp/utf8.h>
#include <iterator>
#include <cstddef>
#include <ostream>

typedef char32_t SexyChar;

class SexyString
{
  private:
	std::string internal;

  public:
	using value_type = SexyChar;
	using byte_type = char;
	static constexpr size_t npos = std::string::npos;

	struct sexystring_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = SexyChar;
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using reference = SexyChar;

		std::string::const_iterator begin;
		std::string::const_iterator current;
		std::string::const_iterator end;
		sexystring_iterator(std::string::const_iterator begin, std::string::const_iterator current, std::string::const_iterator end) : current(current), end(end)
		{
		}

		SexyChar operator*() const
		{
			auto temp = current;
			return utf8::next(temp, end);
		}

		sexystring_iterator &operator++()
		{
			utf8::unchecked::advance(current, 1);
			return *this;
		}

		sexystring_iterator operator++(int)
		{
			sexystring_iterator previous = *this;
			++(*this);
			return previous;
		}

		sexystring_iterator &operator--()
		{
			utf8::prior(current, begin);
			return *this;
		}

		sexystring_iterator operator--(int)
		{
			sexystring_iterator previous = *this;
			--(*this);
			return previous;
		}

		bool operator==(const sexystring_iterator &other) const
		{
			return current == other.current;
		}

		bool operator!=(const sexystring_iterator &other) const
		{
			return current != other.current;
		}
	};

	sexystring_iterator begin() const
	{
		return sexystring_iterator(internal.begin(), internal.begin(), internal.end());
	}
	sexystring_iterator end() const
	{
		return sexystring_iterator(internal.begin(), internal.end(), internal.end());
	}

	SexyString() = default;

	SexyString(const char *str) : internal(str)
	{
	}
	SexyString(std::string &str) : internal(str)
	{
	}
	SexyString(const std::string &str) : internal(str)
	{
	}
	SexyString(std::string &&str) : internal(std::move(str))
	{
	}
	SexyString(SexyChar chr)
	{
		utf8::append(chr, std::back_inserter(internal));
	}

	const std::string &bytes() const
	{
		return internal;
	}

	const char *c_str() const
	{
		return internal.c_str();
	}

	const size_t size() const
	{
		return internal.size();
	}

	const size_t length() const
	{
		return utf8::distance(internal.begin(), internal.end());
	}

	const size_t max_size() const
	{
		return internal.max_size();
	}

	const size_t capacity() const
	{
		return internal.capacity();
	}

	// Reserve n BYTES, for the string, not characters.
	void reserve(size_t n)
	{
		return internal.reserve(n);
	}

	const bool empty() const
	{
		return internal.empty();
	}

	const bool starts_with(const char *str) const
	{
		return internal.starts_with(str);
	}

	const bool ends_with(const char *str) const
	{
		return internal.ends_with(str);
	}

	// Resize to n BYTES, not characters.
	void resize(size_t n)
	{
		internal.resize(n);
	}

	// Resize to n BYTES, not characters.
	void resize(size_t n, char c)
	{
		internal.resize(n, c);
	}

	void clear()
	{
		internal.clear();
	}

	void shrink_to_fit()
	{
		internal.shrink_to_fit();
		auto it = internal.begin();
		auto last = internal.end();
		auto validEnd = it;

		try
		{
			while (it != last)
			{
				validEnd = it;
				utf8::next(it, last);
			}
		}
		catch (utf8::invalid_code_point err)
		{
			internal.erase(validEnd, internal.end());
		}
	}

	// Note: this uses bytes
	int compare(const SexyString &str) const
	{
		return internal.compare(str.bytes());
	}

	// Note: this uses bytes
	int compare(size_t pos, size_t len, const SexyString &str) const
	{
		return internal.compare(pos, len, str.bytes());
	}

	// Note: this uses bytes
	int compare(size_t pos, size_t len, const SexyString &str, size_t subpos, size_t sublen = npos) const
	{
		return internal.compare(pos, len, str.bytes(), subpos, sublen);
	}

	SexyString& erase(size_t pos = 0, size_t len = npos)
	{
		auto first = internal.begin();
		auto last = internal.end();

		utf8::advance(first, pos, last);

		if (len == npos)
		{
			internal.erase(first, last);
			return *this;
		}

		auto eraseEnd = first;
		utf8::advance(eraseEnd, len, last);

		internal.erase(first, eraseEnd);

		return *this;
	}

	SexyString &erase(sexystring_iterator first, sexystring_iterator last)
	{
		internal.erase(first.current, last.current);
		return *this;
	}

	size_t rfind(const SexyString& str, size_t pos = npos) const
	{
		return internal.rfind(str.bytes(), pos);
	}

	size_t find(const SexyString& str, size_t pos = 0) const
	{
		return internal.find(str.bytes(), pos);
	}

	SexyString substr(size_t pos = 0, size_t count = npos) const
	{
		return internal.substr(pos, count);
	}

	SexyString &replace(size_t pos, size_t len, const SexyString &str)
	{
		auto beginIt = internal.begin();
		auto endIt = internal.end();
		for (size_t i = 0; i < pos; i++)
			utf8::next(beginIt, endIt);
		auto lenIt = beginIt;
		for (size_t i = 0; i < len; i++)
			utf8::next(lenIt, endIt);

		internal.replace(beginIt, lenIt, str.bytes());

		return *this;
	}

	SexyString& append(const SexyString &str)
	{
		internal += str.bytes();
		return *this;
	}

	SexyString &append(const SexyString &str, size_t subpos, size_t sublen)
	{
		internal.append(str.bytes(), subpos, sublen);
		return *this;
	}

	SexyString &append(const char *s, size_t n)
	{
		internal.append(s, n);
		return *this;
	}

	SexyString &append(const char *s)
	{
		internal.append(s);
		return *this;
	}

	SexyString &append(size_t n, char c)
	{
		internal.append(n, c);
		return *this;
	}

	SexyString& operator+=(const SexyString &str)
	{
		internal += str.bytes();
		return *this;
	}

	SexyString& operator+=(const char *str)
	{
		internal += str;
		return *this;
	}

	SexyString &operator+=(SexyChar chr)
	{
		utf8::append(chr, std::back_inserter(internal));
		return *this;
	}

	SexyString &operator+=(const std::string &str)
	{
		internal += str;
		return *this;
	}

	SexyString operator+(SexyChar chr) const
	{
		SexyString result = *this;
		result += chr;
		return result;
	}

	SexyString operator+(const SexyString& str) const
	{
		SexyString result = *this;
		result.internal += str.bytes();
		return result;
	}

	friend SexyString operator+(const char *str, const SexyString &sstr)
	{
		SexyString result;
		result += str;
		result += sstr;
		return result;
	}

	friend SexyString operator+(const std::string &str, const SexyString &sstr)
	{
		SexyString result;
		result += str;
		result += sstr;
		return result;
	}

	friend SexyString operator+(SexyChar chr, const SexyString &str)
	{
		SexyString result;
		result += chr;
		result += str;
		return result;
	}

	friend std::ostream &operator<<(std::ostream &os, const SexyString &s)
	{
		return os << s.internal;
	}

	SexyChar operator[](int idx) const
	{
		return internal[idx];
	}

	SexyChar getCharacter(int idx) const
	{
		auto beginIt = internal.begin();
		auto endIt = internal.end();
		for (int i = 0; i < idx; i++)
			utf8::next(beginIt, endIt);
		return utf8::next(beginIt, endIt);
	}

	operator std::string() const
	{
		return internal;
	}

	const std::string &toString() const
	{
		return internal;
	}

	bool operator==(const SexyString &other) const
	{
		return bytes() == other.bytes();
	}

	bool operator!=(const SexyString &other) const
	{
		return bytes() != other.bytes();
	}

	bool operator<(const SexyString &other) const
	{
		return internal < other.internal;
	}
	
	//misc

	static inline bool isValid(SexyChar chr)
	{
		return isValid(SexyString(chr));
	}

	static inline bool isValid(const SexyString &str)
	{
		return utf8::is_valid(str.bytes());
	}
};
