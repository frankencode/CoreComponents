/*
 * strings.cpp -- helper functions for string processing
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "strings.hpp"

namespace ftl
{

char* str::dup(const char* s)
{
	char* s2 = 0;
	if (s) {
		int len = str::len(s);
		s2 = new char[len + 1];
		s2[len] = 0;
		mem::cpy(s2, s, len);
	}
	return s2;
}

char* str::cat(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6, const char* s7)
{
	int len = 0;
	const char* s[] = { s0, s1, s2, s3, s4, s5, s6, s7 };
	const int n = sizeof(s) / sizeof(const char*);
	for (int i = 0; i < n; ++i) {
		if (s[i])
			len += str::len(s[i]);
	}
	char* c = new char[len + 1];
	c[len] = 0;
	int j = 0;
	for (int i = 0; i < n; ++i) {
		if (s[i]) {
			const char* si = s[i];
			int k = 0;
			while (si[k] != 0)
				c[j++] = si[k++];
		}
	}
	check(j == len);
	return c;
}

char* intToStr(int value)
{
	int n = (value <= 0) + 1;
	for (int v = value; v != 0; v /= 10) ++n;
	
	char* buf = new char[n];
	int i = 0;
	if (value < 0)
		buf[0] = '-';
	
	i = n - 1;
	buf[i--] = 0;
	for (int v = value; i >= 0; v /= 10)
		buf[i--] = '0' + (v % 10);
	
	return buf;
}

int strToInt(const char* s, int i0, int i1, int base)
{
	if (!s) return 0;
	int n = 0;
	while (s[n]) ++n;
	if (i0 < 0) i0 = 0;
	else if (i0 > n) i0 = n;
	if (i1 > n) i1 = n;
	n = i1 - i0;
	if (n == 0) return 0;
	int sign = 1 - 2 * (s[0] == '-');
	if (base < 2) base = 2;
	else if (base > 16) base = 16;
	int value = 0;
	for (int i = i0; i < i1; ++i) {
		int digit = s[i];
		if (('0' <= digit) && (digit <= '9')) digit = digit - '0';
		else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
		else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
		else continue;
		value = value * base + digit;
	}
	return sign * value;
}

} // namespace ftl
