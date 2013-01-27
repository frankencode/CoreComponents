 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_BYTEARRAY_HPP
#define FTL_BYTEARRAY_HPP

#include "generics.hpp"
#include "strings.hpp"
#include "Default.hpp"

namespace ftl
{

template<class T> class List;

class Character;
class String;
typedef List<String> StringList;

namespace syntax { class Definition; }
typedef syntax::Definition SyntaxDefinition;

class File;

class ByteArray: public Sequence<char, int>
{
public:
	typedef int Index;
	typedef char Item;

	inline static O<ByteArray> create(int size = 0) { return new ByteArray(size); }
	inline static O<ByteArray> create(int size, char zero) { return new ByteArray(size, zero); }
	inline static O<ByteArray> create(const char *data, int size = -1) { return new ByteArray(data, size); }
	inline static O<ByteArray> create(ByteArray *parent, int size) { return new ByteArray(parent, size); }
	~ByteArray();

	inline static ByteArray *empty() { return Default<ByteArray>::instance(); }

	ByteArray &operator=(const ByteArray &b);

	void clear(char zero = '\0');
	void reset(int newSize = 0);
	void truncate(int newSize);

	Character *chars() const;

	inline int size() const { return size_; }
	inline int length() const { return size_; }
	inline bool isEmpty() const { return size_ == 0; }

	inline int first() const { return 0; }
	inline int last() const { return size_ - 1; }

	static inline int ill() { return -1; }
	static inline bool ill(int i) { return i < 0; }

	inline bool has(int i) const {
		if (i < 0) i += size_;
		return (0 <= i) && (i < size_);
	}

	inline char get(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}

	inline void set(int i, const char &item) {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		data_[i] = item;
	}

	inline char *pointerAt(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_ + i;
	}

	inline char &at(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}
	inline uint8_t &byteAt(int i) const { return reinterpret_cast<uint8_t&>(at(i)); }

	inline char *data() const { return data_; }
	inline const char *constData() const { return data_; }
	inline operator char*() const { return data_; }
	inline operator bool() const { return !isEmpty(); }

	inline void read(int i, char *data, int size) {
		if (size == 0) return;
		FTL_ASSERT(has(i));
		FTL_ASSERT(has(i + size - 1));
		mem::cpy(data, data_ + i, size);
	}

	inline void write(int i, const char *data, int size) {
		if (size == 0) return;
		FTL_ASSERT(has(i));
		FTL_ASSERT(has(i + size - 1));
		mem::cpy(data_ + i, data, size);
	}

	inline O<ByteArray> copy() const { return new ByteArray(*this); }

	inline O<ByteArray> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}

	template<class Range>
	inline O<ByteArray> copy(Range *range) const {
		return copy(range->i0(), range->i1());
	}

	inline O<ByteArray> head(int n) const { return copy(0, n); }
	inline O<ByteArray> tail(int n) const { return copy(size_ - n, size_); }

	inline int find(char ch) const { return find(0, ch); }
	inline int find(int i, char ch) const {
		if (i < 0) i = 0;
		while (i < size_) {
			if (data_[i] == ch) break;
			++i;
		}
		return i;
	}

	inline bool contains(char item) const { return find(item) < size_; }

	inline int replace(char oldItem, char newItem) {
		int n = 0;
		for (int i = 0; i < size_; ++i) {
			if (data_[i] == oldItem) {
				data_[i] = newItem;
				++n;
			}
		}
		return n;
	}

	int find(const char *pattern, int i = 0) const;
	int find(String pattern, int i = 0) const;
	int find(SyntaxDefinition *pattern, int i = 0) const;

	inline bool contains(const char *pattern) const { return find(pattern) != size_; }
	bool contains(String pattern) const;

	static O<ByteArray> join(const StringList *parts, const char *sep = "");
	static O<ByteArray> join(const StringList *parts, char sep);
	static O<ByteArray> join(const StringList *parts, String sep);
	O<StringList> split(char sep) const;
	O<StringList> split(const char *sep) const;
	O<StringList> split(SyntaxDefinition *pattern) const;

	void replaceInsitu(const char *pattern, const char *replacement);
	O<ByteArray> replace(const char *pattern, const char *replacement) const;
	O<ByteArray> replace(String pattern, String replacement) const;

	int toInt(bool *ok = 0) const;
	double toFloat(bool *ok = 0) const;
	int64_t toInt64(bool *ok = 0) const;
	uint64_t toUInt64(bool *ok = 0) const;
	float64_t toFloat64(bool *ok = 0) const;

	ByteArray *toLowerInsitu();
	ByteArray *toUpperInsitu();
	inline O<ByteArray> toLower() const { return copy()->toLowerInsitu(); }
	inline O<ByteArray> toUpper() const { return copy()->toUpperInsitu(); }

	ByteArray *expandInsitu();
	inline O<ByteArray> expand() const { return copy()->expandInsitu(); }

	O<ByteArray> stripLeadingSpace() const;
	O<ByteArray> stripTrailingSpace() const;
	O<ByteArray> trimmed() const;
	O<ByteArray> stripTags() const;
	O<ByteArray> simplified() const;
	O<ByteArray> normalized(bool nameCase = true) const;

	bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;
	bool linePosToOffset(int line, int pos, int *offset = 0) const;

	void checkUtf8() const;

	static O<ByteArray> fromUtf16(const void *data, int size = -1, int endian = localEndian());
	bool toUtf16(void *buf, int *size);
	O<ByteArray> toUtf16(int endian = localEndian());

	O<ByteArray> md5() const;
	O<ByteArray> hex() const;
	O<ByteArray> base64() const;

	bool isRootPath() const;
	bool isRelativePath() const;
	bool isAbsolutePath() const;

	O<ByteArray> absolutePathRelativeTo(String currentDir) const;
	O<ByteArray> absolutePath() const;
	O<ByteArray> fileName() const;
	O<ByteArray> completeBaseName() const;
	O<ByteArray> baseName() const;
	O<ByteArray> reducePath() const;
	O<ByteArray> expandPath(String component) const;
	O<ByteArray> canonicalPath() const;

private:
	friend class Singleton<ByteArray>;
	friend class File;

	explicit ByteArray(int size = 0);
	ByteArray(int size, char zero);
	ByteArray(const char *data, int size);
	ByteArray(char *data, int size, size_t mapSize);
	ByteArray(const ByteArray &b);
	ByteArray(ByteArray *b, int size);

	int size_;
	char *data_;
	mutable O<Character> chars_;
	O<ByteArray> parent_;

	size_t mapSize_;
};

} // namespace ftl

#endif // FTL_BYTEARRAY_HPP
