/*
 * String.cpp -- UTF-8 strings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Utf8Decoder.hpp"
#include "Utf8Encoder.hpp"
#include "Utf16Decoder.hpp"
#include "Utf16Encoder.hpp"
#include "Crc32.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Format.hpp"
#include "Variant.hpp"
#include "Path.hpp"
#include "Stream.hpp"
#include "String.hpp"

namespace ftl
{

String::String(const Variant& b)
	: Super(b.toInstance<ByteArray>())
{}

String::String(const Format& b)
{
	*this = String::join(b);
}

String::String(const Path& b)
	: Super(b.toString().bytes())
{}

String String::fromUtf16(const void* data, int size, int endian)
{
	String s2;
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
		s2 = String::uninitialized(size2);
		Utf16Decoder source(data, size, endian);
		Utf8Encoder sink(s2.bytes()->data(), size2);
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
bool String::toUtf16(void* buf, int* size)
{
	uint16_t* buf2 = reinterpret_cast<uint16_t*>(buf);
	int j = 0, n = *size / 2;
	for (int i = 0; i < length(); ++i) {
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

Ref<ByteArray, Owner> String::toUtf16(int endian)
{
	int size2 = 0;
	for (int i = 0; i < length(); ++i)
		size2 += Utf16Encoder::encodedSize(get(i));
	Ref<ByteArray, Owner> s2 = new ByteArray(size2 + 2);
	(*s2)[size2] = 0;
	(*s2)[size2 + 1] = 0;
	if (size2 > 0) {
		Utf16Encoder sink(s2->data(), size2, endian);
		for (int i = 0; i < length(); ++i)
			sink.write(get(i));
	}
	return s2;
}

void String::validate(const char* data, int size)
{
	if (size < 0) size = str::len(data);
	if (size > 0) {
		Utf8Decoder source(data, size);
		for (uchar_t ch = 0; source.read(&ch););
	}
}

void String::assign(Ref<StringList> parts, const char* sep)
{
	int sepSize = str::len(sep);
	if (parts->length() == 0) {
		set(ByteArray::empty());
	}
	else {
		int size = 0;
		for (int i = 0; i < parts->length(); ++i)
			size += parts->at(i)->size();
		size += (parts->length() - 1) * sepSize;
		set(new ByteArray(size));
		char* p = bytes()->data();
		for (int i = 0; i < parts->length(); ++i) {
			String part = parts->at(i);
			mem::cpy(p, part->data(), part->size());
			p += part->size();
			if (i + 1 < parts->length()) {
				mem::cpy(p, sep, sepSize);
				p += sepSize;
			}
		}
		FTL_ASSERT(p == bytes()->data() + size);
	}
}

String String::copy() const
{
	String b;
	b.Super::set(new ByteArray(bytes()->data(), bytes()->size()));
	return b;
}

bool String::valid() const
{
	try { validate(); }
	catch (StreamIoException&) { return false; }
	return true;
}

int String::find(int i, const char* pattern) const
{
	if (!character()->has(i)) return i;
	const char* t = character()->byte(i); // text pos
	const char* m = pattern; // match pos
	while ((*t) && (*m)) {
		if (*t == *m)
			++m;
		else
			m = pattern;
		++t;
	}
	return (*m) ? length() : character()->index(t - (m - pattern));
}

Ref<StringList, Owner> String::split(const char* pattern) const
{
	Ref<StringList, Owner> parts = new StringList;
	int i0 = 0;
	int patternLength = Utf8Walker::length(pattern);
	while (i0 < length()) {
		int i1 = find(i0, pattern);
		if (i1 == length()) break;
		parts->append(copy(i0, i1));
		i0 = i1 + patternLength;
	}
	if (i0 < length())
		parts->append(copy(i0, length()));
	else
		parts->append(String());
	return parts;
}

void String::replaceInsitu(const char* pattern, const char* replacement)
{
	int patternLength = str::len(pattern);
	int replacementLength = str::len(replacement);
	if (patternLength < replacementLength) {
		*this = replace(pattern, replacement);
	}
	else if (patternLength > 0) {
		int i = 0, j = 0, k = 0, n = bytes()->size();
		while (i < n) {
			char ch = bytes()->at(i++);
			if (j < i) bytes()->set(j++, ch);
			if (ch == pattern[k]) {
				++k;
				if (k == patternLength) {
					j -= patternLength;
					for (k = 0; k < replacementLength; ++k)
						bytes()->set(j++, replacement[k]);
					k = 0;
				}
			}
			else {
				k = 0;
			}
		}
		if (j < i) bytes()->truncate(j);
	}
}

int String::toInt(bool* ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == bytes()->size());
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double String::toFloat(bool* ok) const
{
	return toFloat64(ok);
}

int64_t String::toInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == bytes()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t String::toUInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == bytes()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t String::toFloat64(bool* ok) const
{
	float64_t value = 0.;
	int i1 = 0;
	if (floatLiteral()->match(*this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == bytes()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

void String::toLowerInsitu()
{
	for (int i = 0, n = bytes()->size(); i < n; ++i)
		bytes()->set(i, ftl::toLower(bytes()->at(i)));
}

void String::toUpperInsitu()
{
	for (int i = 0, n = bytes()->size(); i < n; ++i)
		bytes()->set(i, ftl::toUpper(bytes()->at(i)));
}

String String::stripLeadingSpace() const
{
	int i = 0, n = bytes()->size();
	while (i < n) {
		if (!ftl::isSpace(bytes()->at(i))) break;
		++i;
	}
	return String(bytes()->data() + i, bytes()->size() - i);
}

String String::stripTrailingSpace() const
{
	int n = bytes()->size();
	while (n > 0) {
		if (!ftl::isSpace(bytes()->at(n - 1))) break;
		--n;
	}
	return String(bytes()->data(), n);
}

String String::trimmed() const
{
	int t = 0, l = 0, n = bytes()->size();
	while (t < n) {
		if (!ftl::isSpace(bytes()->at(-t-1))) break;
		++t;
	}
	n -= t;
	while (l < n) {
		if (!ftl::isSpace(bytes()->at(l))) break;
		++l;
	}
	return String(bytes()->data() + l, n);
}

String String::stripTags() const
{
	StringList parts;
	char* o = bytes()->data();
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
	return parts.join();
}

String String::simplified() const
{
	return normalized(false);
}

String String::normalized(bool nameCase) const
{
	for (int i = 0, n = bytes()->size(); i < n; ++i) {
		char ch = bytes()->at(i);
		if ((0 <= ch) && (ch < 32)) ch = 32;
		bytes()->set(i, ch);
	}
	Ref<StringList, Owner> parts = split(" ");
	for (int i = 0; i < parts->length(); ++i) {
		String s = parts->at(i);
		if (s.isEmpty()) {
			parts->remove(i);
		}
		else {
			if (nameCase) {
				s = s.toLower();
				s->set(0, ftl::toUpper(s->at(0)));
				parts->set(i, s);
			}
			++i;
		}
	}
	return String::join(parts, " ");
}

void String::expandInsitu()
{
	int j = 0;
	uint32_t hs = 0; // high surrogate, saved
	for (int i = 0, n = bytes()->size(); i < n;) {
		char ch = bytes()->at(i++);
		if ((ch == '\\') && (i < n)) {
			ch = bytes()->at(i++);
			if ((ch == 'u') && (i < n - 4)) {
				uint32_t x = 0;
				for (int k = 0; k < 4; ++k) {
					int digit = bytes()->at(i++);
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
						bytes()->set(j++, ec[k]);
				}
			}
			else {
				hs = 0;
				if (ch == 'b') bytes()->set(j++, 0x08);
				else if (ch == 't') bytes()->set(j++, 0x09);
				else if (ch == 'n') bytes()->set(j++, 0x0A);
				else if (ch == 'r') bytes()->set(j++, 0x0D);
				else if (ch == 'f') bytes()->set(j++, 0x0C);
			}
		}
		else if (j < i) {
			hs = 0;
			bytes()->set(j++, ch);
		}
		else {
			hs = 0;
			++j;
		}
	}
	bytes()->truncate(j);
}

String String::expand() const
{
	String s2 = copy();
	s2.expandInsitu();
	return s2;
}

} // namespace ftl
