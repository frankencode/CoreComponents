/*
 * strings.hpp -- helper functions for string processing
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRINGS_HPP
#define PONA_STRINGS_HPP

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

template<class T>
char* strdup(const T* s)
{
	char* s2 = 0;
	if (s) {
		int len = pona::strlen(s);
		s2 = new char[len + 1];
		s2[len] = 0;
		pona::memcpy(s2, s, len);
	}
	return s2;
}

template<class T>
inline int strcmp(const T* a, const T* b)
{
	int ret = 0;
	while ((*a) && (*b) && (!ret)) {
		if (*a < *b) ret = -1;
		else if (*b < *a) ret = 1;
		++a;
		++b;
	}
	if (!ret)
		ret = (*b == 0) - (*a == 0);
	return ret;
}

template<class T>
T* strcat(const T* s0, const T* s1 = 0, const T* s2 = 0, const T* s3 = 0, const T* s4 = 0, const T* s5 = 0, const T* s6 = 0, const T* s7 = 0)
{
	int len = 0;
	const T* s[] = { s0, s1, s2, s3, s4, s5, s6, s7 };
	const int n = sizeof(s) / sizeof(const T*);
	for (int i = 0; i < n; ++i) {
		if (s[i])
			len += pona::strlen(s[i]);
	}
	T* c = new T[len + 1];
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

} // namespace pona

#endif // PONA_STRINGS_HPP
