/*
 * ByteArray.hpp -- an array of bytes
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_BYTEARRAY_HPP
#define FTL_BYTEARRAY_HPP

#include "generics.hpp"
#include "strings.hpp"
#include "Default.hpp"
#include "Token.hpp"

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

	inline static Ref<ByteArray, Owner> create(int size = 0) { return new ByteArray(size); }
	inline static Ref<ByteArray, Owner> create(int size, char zero) { return new ByteArray(size, zero); }
	inline static Ref<ByteArray, Owner> create(const char *data, int size = -1) { return new ByteArray(data, size); }
	inline static Ref<ByteArray, Owner> create(Ref<ByteArray> parent, int size) { return new ByteArray(parent, size); }
	~ByteArray();

	inline static Ref<ByteArray> empty() { return Default<ByteArray>::instance(); }

	ByteArray &operator=(const ByteArray &b);

	void clear(char zero = '\0');
	void reset(int newSize = 0);
	void truncate(int newSize);

	Ref<Character> chars() const;

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

	inline Ref<ByteArray, Owner> copy() const { return new ByteArray(*this); }

	inline Ref<ByteArray, Owner> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}
	template<class Range>
	inline Ref<ByteArray, Owner> copy(Ref<Range> range) const {
		return copy(range->i0(), range->i1());
	}

	inline Ref<ByteArray, Owner> head(int n) const { return copy(0, n); }
	inline Ref<ByteArray, Owner> tail(int n) const { return copy(size_ - n, size_); }

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
	int find(Ref<SyntaxDefinition> pattern, int i = 0) const;

	inline bool contains(const char *pattern) const { return find(pattern) != size_; }
	bool contains(String pattern) const;

	static Ref<ByteArray, Owner> join(Ref<StringList> parts, const char *sep = "");
	static Ref<ByteArray, Owner> join(Ref<StringList> parts, char sep);
	static Ref<ByteArray, Owner> join(Ref<StringList> parts, String sep);
	Ref<StringList, Owner> split(char sep) const;
	Ref<StringList, Owner> split(const char *sep) const;
	Ref<StringList, Owner> split(Ref<SyntaxDefinition> pattern) const;

	void replaceInsitu(const char *pattern, const char *replacement);
	Ref<ByteArray, Owner> replace(const char *pattern, const char *replacement) const;
	Ref<ByteArray, Owner> replace(String pattern, String replacement) const;

	int toInt(bool *ok = 0) const;
	double toFloat(bool *ok = 0) const;
	int64_t toInt64(bool *ok = 0) const;
	uint64_t toUInt64(bool *ok = 0) const;
	float64_t toFloat64(bool *ok = 0) const;

	Ref<ByteArray> toLowerInsitu();
	Ref<ByteArray> toUpperInsitu();
	inline Ref<ByteArray, Owner> toLower() const { return copy()->toLowerInsitu(); }
	inline Ref<ByteArray, Owner> toUpper() const { return copy()->toUpperInsitu(); }

	Ref<ByteArray> expandInsitu();
	inline Ref<ByteArray, Owner> expand() const { return copy()->expandInsitu(); }

	Ref<ByteArray, Owner> stripLeadingSpace() const;
	Ref<ByteArray, Owner> stripTrailingSpace() const;
	Ref<ByteArray, Owner> trimmed() const;
	Ref<ByteArray, Owner> stripTags() const;
	Ref<ByteArray, Owner> simplified() const;
	Ref<ByteArray, Owner> normalized(bool nameCase = true) const;

	bool offsetToLinePos(int offset, int *line, int *pos) const;

	void checkUtf8() const;

	static Ref<ByteArray, Owner> fromUtf16(const void *data, int size = -1, int endian = localEndian());
	bool toUtf16(void *buf, int *size);
	Ref<ByteArray, Owner> toUtf16(int endian = localEndian());

	Ref<ByteArray, Owner> md5() const;
	Ref<ByteArray, Owner> hex() const;
	Ref<ByteArray, Owner> base64() const;

	bool isRootPath() const;
	bool isRelativePath() const;
	bool isAbsolutePath() const;

	Ref<ByteArray, Owner> absolutePathRelativeTo(String currentDir) const;
	Ref<ByteArray, Owner> absolutePath() const;
	Ref<ByteArray, Owner> fileName() const;
	Ref<ByteArray, Owner> completeBaseName() const;
	Ref<ByteArray, Owner> baseName() const;
	Ref<ByteArray, Owner> reducePath() const;
	Ref<ByteArray, Owner> expandPath(String component) const;
	Ref<ByteArray, Owner> canonicalPath() const;

private:
	friend class Singleton<ByteArray>;
	friend class File;

	explicit ByteArray(int size = 0);
	ByteArray(int size, char zero);
	ByteArray(const char *data, int size, size_t mapSize = 0);
	ByteArray(const ByteArray &b);
	ByteArray(ByteArray *b, int size);

	int size_;
	char *data_;
	mutable Ref<Character, Owner> chars_;
	Ref<ByteArray, Owner> parent_;

	size_t mapSize_;
};

} // namespace ftl

#endif // FTL_BYTEARRAY_HPP
