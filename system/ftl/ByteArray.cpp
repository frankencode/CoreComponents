/*
 * ByteArray.cpp -- an array of bytes
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Utf8Decoder.hpp"
#include "Utf8Encoder.hpp"
#include "Utf16Decoder.hpp"
#include "Utf16Encoder.hpp"
#include "Character.hpp"
#include "Md5.hpp"
#include "Base64.hpp"
#include "ByteArray.hpp"

namespace ftl
{

ByteArray::ByteArray(int size)
	: size_(0),
	  data_(const_cast<char*>("")),
	  range_(false)
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
	  range_(false)
{
	if (size > 0) {
		size_ = size;
		data_ = new char[size + 1];
		mem::clr(data_, size_, zero);
		data_[size] = 0;
	}
}

ByteArray::ByteArray(const char* data, int size)
	: size_(0),
	  data_(const_cast<char*>("")),
	  range_(false)
{
	if (size < 0) size = str::len(data);
	if (size > 0) {
		size_ = size;
		data_ = new char[size + 1];
		mem::cpy(data_, data, size);
		data_[size] = 0;
	}
}

ByteArray::ByteArray(const ByteArray& b)
	: size_(0),
	  data_(const_cast<char*>("")),
	  range_(false)
{
	if (b.size_ > 0) {
		size_ = b.size_;
		data_ = new char[b.size_ + 1];
		mem::cpy(data_, b.data_, b.size_ + 1);
	}
}

ByteArray::ByteArray(ByteArray* b, int size)
	: size_(size),
	  data_(b->data_),
	  range_(true)
{}


ByteArray::~ByteArray() {
	if ((size_ > 0) && (!range_)) delete[] data_;
}

ByteArray& ByteArray::operator=(const ByteArray& b)
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

Ref<Character> ByteArray::chars() const
{
	if (!chars_) chars_ = new Character(data_);
	return chars_;
}

int ByteArray::find(int i, const char* pattern) const
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

Ref<ByteArray, Owner> ByteArray::join(Ref<StringList> parts, const char* sep)
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
		Ref<ByteArray, Owner> result = new ByteArray(size);
		char* p = result->data_;
		for (int i = 0; i < parts->length(); ++i) {
			Ref<ByteArray> part = parts->at(i);
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

Ref<StringList, Owner> ByteArray::split(char sep) const
{
	char sep2[2] = { sep, '\0' };
	return split(sep2);
}

Ref<StringList, Owner> ByteArray::split(const char* sep) const
{
	Ref<StringList, Owner> parts = new StringList;
	int i0 = 0;
	int sepLength = str::len(sep);
	while (i0 < size_) {
		int i1 = find(i0, sep);
		if (i1 == size_) break;
		parts->append(copy(i0, i1));
		i0 = i1 + sepLength;
	}
	if (i0 < size_)
		parts->append(copy(i0, size_));
	else
		parts->append(String());
	return parts;
}

void ByteArray::replaceInsitu(const char* pattern, const char* replacement)
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

Ref<ByteArray, Owner> ByteArray::replace(const char* pattern, const char* replacement) const
{
	return join(split(pattern));
}

int ByteArray::toInt(bool* ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == size_);
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double ByteArray::toFloat(bool* ok) const
{
	return toFloat64(ok);
}

int64_t ByteArray::toInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == size_);
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t ByteArray::toUInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == size_);
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t ByteArray::toFloat64(bool* ok) const
{
	float64_t value = 0.;
	int i1 = 0;
	if (floatLiteral()->match(this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == size_);
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

Ref<ByteArray> ByteArray::toLowerInsitu()
{
	for (int i = 0; i < size_; ++i)
		data_[i] = ftl::toLower(data_[i]);
	return this;
}

Ref<ByteArray> ByteArray::toUpperInsitu()
{
	for (int i = 0; i < size_; ++i)
		data_[i] = ftl::toUpper(data_[i]);
	return this;
}

Ref<ByteArray> ByteArray::expandInsitu()
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
					Utf8Encoder encoder(ec, 4);
					encoder.write(x);
					int el = encoder.byteEncoder()->numBytesWritten();
					for (int k = 0; k < el; ++k)
						data_[j++] = ec[k];
				}
			}
			else {
				hs = 0;
				if (ch == 'b') data_[j++] = 0x08;
				else if (ch == 't') data_[j++] = 0x09;
				else if (ch == 'n') data_[j++] = 0x0A;
				else if (ch == 'r') data_[j++] = 0x0D;
				else if (ch == 'f') data_[j++] = 0x0C;
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

Ref<ByteArray, Owner> ByteArray::stripLeadingSpace() const
{
	int i = 0;
	while (i < size_) {
		if (!ftl::isSpace(data_[i])) break;
		++i;
	}
	return copy(i, size_ - i);
}

Ref<ByteArray, Owner> ByteArray::stripTrailingSpace() const
{
	int n = size_;
	while (n > 0) {
		if (!ftl::isSpace(data_[n - 1])) break;
		--n;
	}
	return copy(0, n);
}

Ref<ByteArray, Owner> ByteArray::trimmed() const
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

Ref<ByteArray, Owner> ByteArray::stripTags() const
{
	StringList parts;
	char* o = data_;
	char* p = o;
	while (*p) {
		if (*p == '<') {
			if (o < p) parts << String(o, p-o);
			while ((*p) && (*p != '>')) ++p;
			p += (*p == '>');
			o = p;
		}
		else if (*p == '&') {
			if (o < p) parts << String(o, p-o);
			while ((*p) && (*p != ';')) ++p;
			p += (*p == ';');
			o = p;
		}
		else {
			++p;
		}
	}
	if (o < p) parts << String(o, p-o);
	return join(&parts);
}

Ref<ByteArray, Owner> ByteArray::simplified() const
{
	return normalized(false);
}

Ref<ByteArray, Owner> ByteArray::normalized(bool nameCase) const
{
	for (int i = 0; i < size_; ++i) {
		if ((0 <= data_[i]) && (data_[i] < 32))
			data_[i] = 32;
	}
	Ref<StringList, Owner> parts = split(" ");
	for (int i = 0; i < parts->length(); ++i) {
		String s = parts->at(i);
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
  * \arg line n-th line starting with 1
  * \arg pos position on line starting with 1 (in bytes)
  * \return true if successful
  */
bool ByteArray::offsetToLinePos(int offset, int* line, int* pos) const
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
	*line = y;
	*pos = x;
	return true;
}

Ref<ByteArray, Owner> ByteArray::fromUtf16(const void* data, int size, int endian)
{
	Ref<ByteArray, Owner> s2;
	if (size < 0) {
		size = 0;
		for (const uint16_t* p = reinterpret_cast<const uint16_t*>(data); *p; ++p) ++size;
		size *= 2;
	}
	if (size > 0) {
		int size2 = 0;
		{
			Utf16Decoder source(data, size, endian);
			for (uchar_t ch; source.read(&ch);)
				size2 += Utf8Encoder::encodedSize(ch);
		}
		s2 = new ByteArray(size2);
		Utf16Decoder source(data, size, endian);
		Utf8Encoder sink(s2->data_, size2);
		for (uchar_t ch; source.read(&ch);)
			sink.write(ch);
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
bool ByteArray::toUtf16(void* buf, int* size)
{
	uint16_t* buf2 = reinterpret_cast<uint16_t*>(buf);
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

Ref<ByteArray, Owner> ByteArray::toUtf16(int endian)
{
	int size2 = 0;
	for (int i = 0; i < chars()->length(); ++i)
		size2 += Utf16Encoder::encodedSize(get(i));
	Ref<ByteArray, Owner> s2 = new ByteArray(size2 + 2);
	(*s2)[size2] = 0;
	(*s2)[size2 + 1] = 0;
	if (size2 > 0) {
		Utf16Encoder sink(s2->data(), size2, endian);
		for (int i = 0; i < chars()->length(); ++i)
			sink.write(get(i));
	}
	return s2;
}

void ByteArray::checkUtf8() const
{
	Utf8Decoder source(data_, size_);
	for (uchar_t ch = 0; source.read(&ch););
}

Ref<ByteArray, Owner> ByteArray::md5() const
{
	Md5 md5;
	md5.feed(data_, size_);
	return md5.finish();
}

Ref<ByteArray, Owner> ByteArray::base64() const
{
	return Base64::encode(this);
}

} // namespace ftl
