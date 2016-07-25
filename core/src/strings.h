/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \brief Low-level C-string helper functions
  * \file strings
  */

#include <string.h> // memset, memcpy, strcmp
#include <cc/types>

namespace cc {

inline void *malloc(size_t size) { return (void* )new char[size]; }
inline void free(void *p) { delete[](char *)p; }

inline void memclr(void *buf, size_t bufSize) { memset(buf, 0, bufSize); }

/*template<class T>
inline int strlen(const T *s)
{
    int i = 0;
    if (s) while (s[i]) ++i;
    return i;
}*/

template<class T, class S>
inline void strcpy(T *t, const S *s, int n = -1)
{
    if (n == -1) n = len(s);
    for (int i = 0; i < n; ++i) t[i] = s[i];
}

inline int strcasecmp(const char *a, const char *b)
{
    int ret = 0;
    char ca, cb;
    while ((ca = *a) && (cb = *b) && (!ret)) {
        ca = downcase(ca);
        cb = downcase(cb);
        if (ca < cb) ret = -1;
        else if (cb < ca) ret = 1;
        ++a;
        ++b;
    }
    if (!ret)
        ret = (*b == 0) - (*a == 0);
    return ret;
}

char *strdup(const char *s);
char *strcat(const char *s0, const char *s1 = 0, const char *s2 = 0, const char *s3 = 0, const char *s4 = 0, const char *s5 = 0, const char *s6 = 0, const char *s7 = 0);

char *intToStr(int value);
int strToInt(const char *s, int i0 = 0, int i1 = intMax, int base = 10);

} // namespace cc

