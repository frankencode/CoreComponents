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
class string;
typedef List<string> StringList;

namespace syntax { class Definition; }
typedef syntax::Definition SyntaxDefinition;

class File;

class ByteArray: public Sequence<char, int>
{
public:
	typedef int Index;
	typedef char Item;

	inline static hook<ByteArray> create(int size = 0) { return new ByteArray(size); }
	inline static hook<ByteArray> create(int size, char zero) { return new ByteArray(size, zero); }
	inline static hook<ByteArray> create(const char *data, int size = -1) { return new ByteArray(data, size); }
	inline static hook<ByteArray> create(ByteArray *parent, int size) { return new ByteArray(parent, size); }
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

	inline hook<ByteArray> copy() const { return new ByteArray(*this); }

	inline hook<ByteArray> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}

	template<class Range>
	inline hook<ByteArray> copy(Range *range) const {
		return copy(range->i0(), range->i1());
	}

	inline hook<ByteArray> head(int n) const { return copy(0, n); }
	inline hook<ByteArray> tail(int n) const { return copy(size_ - n, size_); }

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
	int find(string pattern, int i = 0) const;
	int find(SyntaxDefinition *pattern, int i = 0) const;

	inline bool contains(const char *pattern) const { return find(pattern) != size_; }
	bool contains(string pattern) const;

	static hook<ByteArray> join(const StringList *parts, const char *sep = "");
	static hook<ByteArray> join(const StringList *parts, char sep);
	static hook<ByteArray> join(const StringList *parts, string sep);
	hook<StringList> split(char sep) const;
	hook<StringList> split(const char *sep) const;
	hook<StringList> split(SyntaxDefinition *pattern) const;

	void replaceInsitu(const char *pattern, const char *replacement);
	hook<ByteArray> replace(const char *pattern, const char *replacement) const;
	hook<ByteArray> replace(string pattern, string replacement) const;

	int toInt(bool *ok = 0) const;
	double toFloat(bool *ok = 0) const;
	int64_t toInt64(bool *ok = 0) const;
	uint64_t toUInt64(bool *ok = 0) const;
	float64_t toFloat64(bool *ok = 0) const;

	ByteArray *toLowerInsitu();
	ByteArray *toUpperInsitu();
	inline hook<ByteArray> toLower() const { return copy()->toLowerInsitu(); }
	inline hook<ByteArray> toUpper() const { return copy()->toUpperInsitu(); }

	ByteArray *expandInsitu();
	inline hook<ByteArray> expand() const { return copy()->expandInsitu(); }

	hook<ByteArray> stripLeadingSpace() const;
	hook<ByteArray> stripTrailingSpace() const;
	hook<ByteArray> trimmed() const;
	hook<ByteArray> stripTags() const;
	hook<ByteArray> simplified() const;
	hook<ByteArray> normalized(bool nameCase = true) const;

	bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;
	bool linePosToOffset(int line, int pos, int *offset = 0) const;

	void checkUtf8() const;

	static hook<ByteArray> fromUtf16(const void *data, int size = -1, int endian = localEndian());
	bool toUtf16(void *buf, int *size);
	hook<ByteArray> toUtf16(int endian = localEndian());

	hook<ByteArray> md5() const;
	hook<ByteArray> hex() const;
	hook<ByteArray> base64() const;

	bool isRootPath() const;
	bool isRelativePath() const;
	bool isAbsolutePath() const;

	hook<ByteArray> absolutePathRelativeTo(string currentDir) const;
	hook<ByteArray> absolutePath() const;
	hook<ByteArray> fileName() const;
	hook<ByteArray> completeBaseName() const;
	hook<ByteArray> baseName() const;
	hook<ByteArray> reducePath() const;
	hook<ByteArray> expandPath(string component) const;
	hook<ByteArray> canonicalPath() const;

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
	mutable hook<Character> chars_;
	hook<ByteArray> parent_;

	size_t mapSize_;
};

} // namespace ftl

#endif // FTL_BYTEARRAY_HPP
