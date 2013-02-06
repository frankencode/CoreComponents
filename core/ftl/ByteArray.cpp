 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <sys/mman.h> // munmap
#include "List.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Utf8Decoder.hpp"
#include "Utf8Encoder.hpp"
#include "Utf16Decoder.hpp"
#include "Utf16Encoder.hpp"
#include "Character.hpp"
#include "Md5.hpp"
#include "Base64.hpp"
#include "format.hpp"
#include "Process.hpp"
#include "ByteArray.hpp"

namespace ftl
{

Ref<ByteArray> ByteArray::join(Parts *parts, const char *sep)
{
	int sepSize = str::len(sep);
	if (parts->length() == 0) {
		return ByteArray::empty();
	}
	else {
		int size = 0;
		for (int i = 0; i < parts->length(); ++i)
			size += parts->at(i)->size();
		size += (parts->length() - 1) * sepSize;
		Ref<ByteArray> result = ByteArray::create(size);
		char *p = result->data_;
		for (int i = 0; i < parts->length(); ++i) {
			ByteArray *part = parts->at(i);
			mem::cpy(p, part->data_, part->size_);
			p += part->size_;
			if (i + 1 < parts->length()) {
				mem::cpy(p, sep, sepSize);
				p += sepSize;
			}
		}
		FTL_ASSERT(p == result->data_ + result->size_);
		return result;
	}
}

ByteArray::ByteArray(int size)
	: size_(0),
	  data_(const_cast<char*>("")),
	  mapSize_(0)
{
	if (size > 0) {
		size_ = size;
		data_ = new char[size + 1];
		data_[size] = 0;
	}
}

ByteArray::ByteArray(int size, char zero)
	: size_(0),
	  data_(const_cast<char*>("")),
	  mapSize_(0)
{
	if (size > 0) {
		size_ = size;
		data_ = new char[size + 1];
		mem::clr(data_, size_, zero);
		data_[size] = 0;
	}
}

ByteArray::ByteArray(const char *data, int size)
	: size_(0),
	  data_(const_cast<char*>("")),
	  mapSize_(0)
{
	if (size < 0) size = str::len(data);
	if (size > 0) {
		size_ = size;
		data_ = new char[size + 1];
		mem::cpy(data_, data, size);
		data_[size] = 0;
	}
}

ByteArray::ByteArray(char *data, int size, size_t mapSize)
	: size_(size),
	  data_(data),
	  mapSize_(mapSize)
{}

ByteArray::ByteArray(const ByteArray &b)
	: size_(0),
	  data_(const_cast<char*>("")),
	  mapSize_(0)
{
	if (b.size_ > 0) {
		size_ = b.size_;
		data_ = new char[b.size_ + 1];
		mem::cpy(data_, b.data_, b.size_ + 1);
	}
}

ByteArray::ByteArray(ByteArray *parent, int size)
	: size_(size),
	  data_(parent->data_),
	  parent_(parent),
	  mapSize_(0)
{}


ByteArray::~ByteArray()
{
	if (mapSize_ > 0) {
		::munmap((void*)data_, mapSize_);
	}
	else if ((size_ > 0) && (!parent_)) {
		delete[] data_;
	}
}

ByteArray &ByteArray::operator=(const ByteArray &b)
{
	if (size_ != b.size_) {
		if (size_ > 0) delete[] data_;
		if (b.size_ > 0) {
			size_ = b.size_;
			data_ = new char[b.size_ + 1];
		}
		else {
			size_ = 0;
			data_ = const_cast<char*>("");
		}
	}
	if (b.size_ > 0)
		mem::cpy(data_, b.data_, b.size_ + 1);
	chars_ = 0;
	return *this;
}

void ByteArray::clear(char zero)
{
	mem::clr(data_, size_, zero);
}

void ByteArray::reset(int newSize)
{
	if (size_ != newSize) {
		if (size_ > 0) delete[] data_;
		if (newSize > 0) {
			size_ = newSize;
			data_ = new char[newSize + 1];
			data_[newSize] = 0;
		}
		else {
			size_ = 0;
			data_ = const_cast<char*>("");
		}
		chars_ = 0;
	}
}

void ByteArray::truncate(int newSize)
{
	if (newSize < 0) newSize = 0;
	if (newSize < size_) {
		size_ = newSize;
		data_[newSize] = 0;
		chars_ = 0;
	}
}

Character *ByteArray::chars() const
{
	if (!chars_) chars_ = new Character(data_);
	return chars_;
}

int ByteArray::find(const char *pattern, int i) const
{
	if (!has(i)) return size_;
	if (!pattern[0]) return size_;
	for (int j = i, k = 0; j < size_;) {
		if (data_[j++] == pattern[k]) {
			++k;
			if (!pattern[k])
				return j - k;
		}
		else {
			k = 0;
		}
	}
	return size_;
}

int ByteArray::find(string pattern, int i) const
{
	return find(pattern->constData(), i);
}

int ByteArray::find(SyntaxDefinition *pattern, int i) const
{
	Ref<Token> token = pattern->find(this, i);
	return (token) ? token->i0(): size_;
}

bool ByteArray::contains(string pattern) const
{
	return contains(pattern->constData());
}

Ref<ByteArray> ByteArray::join(const StringList *parts, const char *sep)
{
	int sepSize = str::len(sep);
	if (parts->length() == 0) {
		return ByteArray::empty();
	}
	else {
		int size = 0;
		for (int i = 0; i < parts->length(); ++i)
			size += parts->at(i)->size();
		size += (parts->length() - 1) * sepSize;
		Ref<ByteArray> result = ByteArray::create(size);
		char *p = result->data_;
		for (int i = 0; i < parts->length(); ++i) {
			ByteArray *part = parts->at(i);
			mem::cpy(p, part->data_, part->size_);
			p += part->size_;
			if (i + 1 < parts->length()) {
				mem::cpy(p, sep, sepSize);
				p += sepSize;
			}
		}
		FTL_ASSERT(p == result->data_ + result->size_);
		return result;
	}
}

Ref<ByteArray> ByteArray::join(const StringList *parts, char sep)
{
	char h[2];
	h[0] = sep;
	h[1] = 0;
	return join(parts, h);
}

Ref<ByteArray> ByteArray::join(const StringList *parts, string sep)
{
	return join(parts, sep->constData());
}

Ref<StringList> ByteArray::split(char sep) const
{
	char sep2[2] = { sep, '\0' };
	return split(sep2);
}

Ref<StringList> ByteArray::split(const char *sep) const
{
	Ref<StringList> parts = StringList::create();
	int i0 = 0;
	int sepLength = str::len(sep);
	while (i0 < size_) {
		int i1 = find(sep, i0);
		if (i1 == size_) break;
		parts->append(copy(i0, i1));
		i0 = i1 + sepLength;
	}
	if (i0 < size_)
		parts->append(copy(i0, size_));
	else
		parts->append(string());
	return parts;
}

Ref<StringList> ByteArray::split(SyntaxDefinition *pattern) const
{
	Ref<StringList> parts = StringList::create();
	for (int i = 0; i < size_;) {
		Ref<Token> token = pattern->find(this, i);
		parts->append(copy(i, token->i0()));
		i = token->i1();
	}
	return parts;
}

void ByteArray::replaceInsitu(const char *pattern, const char *replacement)
{
	int patternLength = str::len(pattern);
	int replacementLength = str::len(replacement);
	if (patternLength < replacementLength) {
		*this = *replace(pattern, replacement);
	}
	else if (patternLength > 0) {
		int i = 0, j = 0, k = 0, n = size_;
		while (i < n) {
			char ch = data_[i++];
			if (j < i) data_[j++] = ch;
			if (ch == pattern[k]) {
				++k;
				if (k == patternLength) {
					j -= patternLength;
					for (k = 0; k < replacementLength; ++k)
						data_[j++] = replacement[k];
					k = 0;
				}
			}
			else {
				k = 0;
			}
		}
		if (j < i) truncate(j);
	}
}

Ref<ByteArray> ByteArray::replace(const char *pattern, const char *replacement) const
{
	return join(split(pattern), replacement);
}

Ref<ByteArray> ByteArray::replace(string pattern, string replacement) const
{
	return replace(pattern->constData(), replacement->constData());
}

int ByteArray::toInt(bool *ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	if (integerLiteral()->read(&value, &sign, this, -1))
		 *ok = (value <= uint64_t(intMax));
	else
		*ok = false;
	return sign * int(value);
}

double ByteArray::toFloat(bool *ok) const
{
	return toFloat64(ok);
}

int64_t ByteArray::toInt64(bool *ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	if (integerLiteral()->read(&value, &sign, this, -1))
		*ok = ((value & (uint64_t(1) << 63)) != 0);
	else
		*ok = false;
	return sign * value;
}

uint64_t ByteArray::toUInt64(bool *ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	if (integerLiteral()->read(&value, &sign, this, -1))
		*ok = (sign == 1);
	else
		*ok = false;
	return value;
}

float64_t ByteArray::toFloat64(bool *ok) const
{
	bool h;
	if (!ok) ok = &h;
	float64_t value = 0.;
	*ok = floatLiteral()->read(&value, this, -1);
	return value;
}

ByteArray *ByteArray::toLowerInsitu()
{
	for (int i = 0; i < size_; ++i)
		data_[i] = ftl::toLower(data_[i]);
	return this;
}

ByteArray *ByteArray::toUpperInsitu()
{
	for (int i = 0; i < size_; ++i)
		data_[i] = ftl::toUpper(data_[i]);
	return this;
}

ByteArray *ByteArray::expandInsitu()
{
	int j = 0;
	uint32_t hs = 0; // high surrogate, saved
	for (int i = 0, n = size_; i < n;) {
		char ch = data_[i++];
		if ((ch == '\\') && (i < n)) {
			ch = data_[i++];
			if ((ch == 'u') && (i < n - 4)) {
				uint32_t x = 0;
				for (int k = 0; k < 4; ++k) {
					int digit = data_[i++];
					if (('0' <= digit) && (digit <= '9')) digit -= '0';
					else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
					else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
					x = (x * 16) + digit;
				}
				if ((0xB800 <= x) && (x < 0xDC00)) {
					// save the high surrogate, do not output anything
					hs = x;
				}
				else {
					if ((0xDC00 <= x) && (x < 0xE000) && (hs != 0)) {
						// combine the high and low surrogate
						x = ((hs - 0xD800) << 10) | (x - 0xDC00);
						x += 0x10000;
						hs = 0;
					}
					char ec[4];
					Ref<Utf8Encoder> encoder = Utf8Encoder::open(ec, 4);
					encoder->write(x);
					int el = encoder->byteEncoder()->numBytesWritten();
					for (int k = 0; k < el; ++k)
						data_[j++] = ec[k];
				}
			}
			else if ((ch == 'x') && (i < n - 2)) {
				uint8_t x = 0;
				for (int k = 0; k < 2; ++k) {
					int digit = data_[i++];
					if (('0' <= digit) && (digit <= '9')) digit -= '0';
					else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
					else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
					x = (x * 16) + digit;
				}
				data_[j++] = (char)x;
			}
			else {
				hs = 0;
				if (ch == 'b') data_[j++] = 0x08;
				else if (ch == 't') data_[j++] = 0x09;
				else if (ch == 'n') data_[j++] = 0x0A;
				else if (ch == 'r') data_[j++] = 0x0D;
				else if (ch == 'f') data_[j++] = 0x0C;
				else data_[j++] = ch;
			}
		}
		else if (j < i) {
			hs = 0;
			data_[j++] = ch;
		}
		else {
			hs = 0;
			++j;
		}
	}
	truncate(j);
	return this;
}

Ref<ByteArray> ByteArray::stripLeadingSpace() const
{
	int i = 0;
	while (i < size_) {
		if (!ftl::isSpace(data_[i])) break;
		++i;
	}
	return copy(i, size_ - i);
}

Ref<ByteArray> ByteArray::stripTrailingSpace() const
{
	int n = size_;
	while (n > 0) {
		if (!ftl::isSpace(data_[n - 1])) break;
		--n;
	}
	return copy(0, n);
}

Ref<ByteArray> ByteArray::trimmed() const
{
	int t = 0, i = 0, n = size_;
	while (t < n) {
		if (!ftl::isSpace(data_[-t-1])) break;
		++t;
	}
	n -= t;
	while (i < n) {
		if (!ftl::isSpace(data_[i])) break;
		++i;
	}
	return copy(i, n);
}

Ref<ByteArray> ByteArray::stripTags() const
{
	Ref<StringList> parts = StringList::create();
	char *o = data_;
	char *p = o;
	while (*p) {
		if (*p == '<') {
			if (o < p) parts->append(string(o, p-o));
			while ((*p) && (*p != '>')) ++p;
			p += (*p == '>');
			o = p;
		}
		else if (*p == '&') {
			if (o < p) parts->append(string(o, p-o));
			while ((*p) && (*p != ';')) ++p;
			p += (*p == ';');
			o = p;
		}
		else {
			++p;
		}
	}
	if (o < p) parts->append(string(o, p-o));
	return join(parts);
}

Ref<ByteArray> ByteArray::simplified() const
{
	return normalized(false);
}

Ref<ByteArray> ByteArray::normalized(bool nameCase) const
{
	for (int i = 0; i < size_; ++i) {
		if ((0 <= data_[i]) && (data_[i] < 32))
			data_[i] = 32;
	}
	Ref<StringList> parts = split(" ");
	for (int i = 0; i < parts->length(); ++i) {
		string s = parts->at(i);
		if (s->isEmpty()) {
			parts->remove(i);
		}
		else {
			if (nameCase) {
				s = s->toLower();
				s->set(0, ftl::toUpper(s->at(0)));
				parts->set(i, s);
			}
			++i;
		}
	}
	return join(parts, " ");
}

/** \brief Map a byte offset to editor coordinates.
  * \arg offset byte offset
  * \arg line n-th line starting with 1
  * \arg pos position on line starting with 1 (in bytes)
  * \return true if successful
  */
bool ByteArray::offsetToLinePos(int offset, int *line, int *pos) const
{
	if ((offset < 0) || (size() <= offset)) return false;
	int y = 1, x = 1;
	for (int i = 0; i < offset; ++i) {
		if (data_[i] == '\n') {
			++y;
			x = 1;
		}
		else {
			++x;
		}
	}
	if (line) *line = y;
	if (pos) *pos = x;
	return true;
}

/** \brief Map editor coordinates to a byte offset
  * \arg line n-th line starting with 1
  * \arg pos position on line starting with 1 (in bytes)
  * \arg offset byte offset
  * \return true if successful
  */
bool ByteArray::linePosToOffset(int line, int pos, int *offset) const
{
	if (line <= 0) return false;
	int i = 0;
	for (int y = 1; y < line && i < size_; ++i)
		if (data_[i] == '\n') ++y;
	if (i + pos >= size_)
		return false;
	if (offset) *offset = i + pos;
	return true;
}

Ref<ByteArray> ByteArray::fromUtf16(const void *data, int size, int endian)
{
	Ref<ByteArray> s2;
	if (size < 0) {
		size = 0;
		for (const uint16_t *p = reinterpret_cast<const uint16_t*>(data); *p; ++p) ++size;
		size *= 2;
	}
	if (size > 0) {
		int size2 = 0;
		{
			Ref<Utf16Decoder> source = Utf16Decoder::open(data, size, endian);
			for (uchar_t ch; source->read(&ch);)
				size2 += Utf8Encoder::encodedSize(ch);
		}
		s2 = ByteArray::create(size2);
		Ref<Utf16Decoder> source = Utf16Decoder::open(data, size, endian);
		Ref<Utf8Encoder> sink = Utf8Encoder::open(s2->data_, size2);
		for (uchar_t ch; source->read(&ch);)
			sink->write(ch);
	}
	return s2;
}

/** Convert this string to UTF-16 efficiently (local endian).
  * Returns true if the given buffer was suitable to hold the encoded string.
  * The number of bytes required to fully represent the string in UTF-16 is
  * returned with the 'size' argument. Passing a zero for 'size' allows to
  * determine the required buffer size. No zero termination is written or
  * or accounted for.
  */
bool ByteArray::toUtf16(void *buf, int *size)
{
	uint16_t *buf2 = reinterpret_cast<uint16_t*>(buf);
	int j = 0, n = *size / 2;
	for (int i = 0; i < chars()->length(); ++i) {
		uchar_t ch = get(i);
		if (ch < 0x10000) {
			if (j < n) buf2[j] = ch;
			++j;
		}
		else if (ch <= 0x10FFFF) {
			if (j + 1 < n) {
				buf2[j] = (ch >> 10) + 0xB800;
				buf2[j + 1] = (ch & 0x3FF) + 0xBC00;
			}
			j += 2;
		}
		else {
			if (j < n) buf2[j] = 0xFFFD/*replacement character*/;
			++j;
		}
	}
	*size = 2 * j;
	return (j <= n);
}

Ref<ByteArray> ByteArray::toUtf16(int endian)
{
	int size2 = 0;
	for (int i = 0; i < chars()->length(); ++i)
		size2 += Utf16Encoder::encodedSize(get(i));
	Ref<ByteArray> s2 = ByteArray::create(size2 + 2);
	(*s2)[size2] = 0;
	(*s2)[size2 + 1] = 0;
	if (size2 > 0) {
		Ref<Utf16Encoder> sink = Utf16Encoder::create(s2->data(), size2, endian);
		for (int i = 0; i < chars()->length(); ++i)
			sink->write(get(i));
	}
	return s2;
}

void ByteArray::checkUtf8() const
{
	Ref<Utf8Decoder> source = Utf8Decoder::create(data_, size_);
	for (uchar_t ch = 0; source->read(&ch););
}

Ref<ByteArray> ByteArray::md5() const
{
	Md5 md5;
	md5.feed(data_, size_);
	return md5.finish();
}

Ref<ByteArray> ByteArray::hex() const
{
	Ref<ByteArray> s2 = ByteArray::create(size_ * 2);
	int j = 0;
	for (int i = 0; i < size_; ++i) {
		unsigned char ch = (unsigned char)data_[i];
		int d0 = ch & 0xf;
		int d1 = (ch >> 4) & 0xf;
		if ((0 <= d0) && (d0 < 10)) s2->data_[j++] = d0 + '0';
		else s2->data_[j++] = (d0 - 10) + 'a';
		if ((0 <= d1) && (d1 < 10)) s2->data_[j++] = d1 + '0';
		else s2->data_[j++] = (d1 - 10) + 'a';
	}
	return s2;
}

Ref<ByteArray> ByteArray::base64() const
{
	return Base64::encode(this);
}

bool ByteArray::isRootPath() const
{
	return string(this) == "/";
}

bool ByteArray::isRelativePath() const
{
	return !isAbsolutePath();
}

bool ByteArray::isAbsolutePath() const
{
	return (length() > 0) ? (get(0) == '/') : false;
}

Ref<ByteArray> ByteArray::absolutePathRelativeTo(string currentDir) const
{
	if (isAbsolutePath() || (currentDir == "."))
		return const_cast<ByteArray *>(this);

	Ref<StringList> absoluteParts = StringList::create();
	Ref<StringList> parts = split("/");

	int upCount = 0;

	for (int i = 0; i < parts->length(); ++i)
	{
		string c = parts->at(i);
		if (c == ".")
		{}
		else if (c == "..") {
			if (absoluteParts->length() > 0)
				absoluteParts->popBack();
			else
				++upCount;
		}
		else {
			absoluteParts->append(c);
		}
	}

	string prefix;
	if (currentDir->length() > 0)
		prefix = currentDir->copy();
	else
		prefix = Process::cwd();

	while (upCount > 0) {
		prefix = prefix->reducePath();
		--upCount;
	}

	absoluteParts->pushFront(prefix);

	return absoluteParts->join("/");
}

Ref<ByteArray> ByteArray::absolutePath() const
{
	if (isAbsolutePath())
		return const_cast<ByteArray *>(this);
	return absolutePathRelativeTo(string());
}

Ref<ByteArray> ByteArray::fileName() const
{
	string name;
	Ref<StringList> parts = split("/");
	if (parts->length() > 0)
		name = parts->at(-1);
	return name;
}

Ref<ByteArray> ByteArray::completeBaseName() const
{
	Ref<StringList> parts = fileName()->split(".");
	parts->popBack();
	return parts->join(".");
}

Ref<ByteArray> ByteArray::baseName() const
{
	Ref<StringList> parts = fileName()->split(".");
	return parts->at(0);
}

Ref<ByteArray> ByteArray::reducePath() const
{
	Ref<StringList> parts = split("/");
	if (parts->length() > 0)
		parts->popBack();
	string resultPath = parts->join("/");
	if ((resultPath == "") && isAbsolutePath())
		resultPath = "/";
	return resultPath;
}

Ref<ByteArray> ByteArray::expandPath(string component) const
{
	return string(format() << string(this) << "/" << component);
}

Ref<ByteArray> ByteArray::canonicalPath() const
{
	Ref<StringList> parts = split("/");
	Ref<StringList> result = StringList::create();
	for (int i = 0; i < parts->length(); ++i) {
		string part = parts->at(i);
		if ((part == "") && (i > 0)) continue;
		if ((part == "") && (i == parts->length() - 1)) continue;
		if ((part == ".") && (i > 0)) continue;
		if ((part == "..") && (result->length() > 0)) {
			result->popBack();
			continue;
		}
		result->append(part);
	}
	return result->join("/");
}

} // namespace ftl
