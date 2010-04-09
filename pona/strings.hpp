/*
 * strings.hpp -- helper functions for string processing
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRINGS_HPP
#define PONA_STRINGS_HPP

#include "types.hpp"
#include "Exception.hpp"

namespace pona
{

template<class T>
inline void bzero(T* s)
{
	char* sc = reinterpret_cast<char*>(s);
	for (unsigned i = 0; i < sizeof(T); ++i) sc[i] = 0;
}

inline void bzero(void* p, int len)
{
	char* sc = reinterpret_cast<char*>(p);
	for (int i = 0; i < len; ++i) sc[i] = 0;
}

template<class T>
inline void memset(T* s, T ch, int len)
{
	for (int i = 0; i < len; ++i) s[i] = ch;
}

template<class T, class S>
inline void memcpy(T* t, const S* s, int n)
{
	for (int i = 0; i < n; ++i)
		t[i] = s[i];
}

template<class T>
inline int strlen(const T* s)
{
	int len = 0;
	if (s)
		while (*(s + len) != 0) ++len;
	return len;
}

inline int strcmp(const char* a, const char* b)
{
	int ret = 0;
	char ca, cb;
	while ((ca = *a) && (cb = *b) && (!ret)) {
		if (ca < cb) ret = -1;
		else if (cb < ca) ret = 1;
		++a;
		++b;
	}
	if (!ret)
		ret = (*b == 0) - (*a == 0);
	return ret;
}

inline int strcasecmp(const char* a, const char* b)
{
	int ret = 0;
	char ca, cb;
	while ((ca = *a) && (cb = *b) && (!ret)) {
		ca = toLower(ca);
		cb = toLower(cb);
		if (ca < cb) ret = -1;
		else if (cb < ca) ret = 1;
		++a;
		++b;
	}
	if (!ret)
		ret = (*b == 0) - (*a == 0);
	return ret;
}

char* strdup(const char* s);
char* strcat(const char* s0, const char* s1 = 0, const char* s2 = 0, const char* s3 = 0, const char* s4 = 0, const char* s5 = 0, const char* s6 = 0, const char* s7 = 0);

char* intToStr(int value);
int strToInt(const char* s, int i0 = 0, int i1 = intMax, int base = 10);

} // namespace pona

#endif // PONA_STRINGS_HPP
