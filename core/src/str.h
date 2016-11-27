/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \file str.h
  * \brief Output formatting helper functions
  * \headerfile str.h <cc/str>
  */

#include <cc/String>
#include <cc/Variant>

namespace cc {

template<class T>
struct Sign { inline static int get(T x) { return x < 0; } };

template<>
struct Sign<unsigned char> { inline static int get(unsigned char) { return 0; } };

template<>
struct Sign<unsigned short> { inline static int get(unsigned short) { return 0; } };

template<>
struct Sign<unsigned int> { inline static int get(unsigned int) { return 0; } };

template<>
struct Sign<unsigned long> { inline static int get(unsigned long) { return 0; } };

template<>
struct Sign<unsigned long long> { inline static int get(unsigned long long) { return 0; } };

/** \addtogroup human_io
  * \{
  */

/** Convert an integer value to string
  * \tparam T integer type
  * \param x integer value
  * \param base number base (2..62)
  * \param n maximum number of digits
  * \return human-readable string
  */
template<class T>
String inum(T x, int base = 10, int n = -1)
{
    int sign = Sign<T>::get(x);
    if (sign) x = -x;
    int m = (x == 0);
    for (T y = x; y > 0; y /= base) ++m;
    if (sign) ++m;
    if (n > 0 && m < n) m = n;
    String s(m, '0');
    if (sign) s->at(0) = '-';
    for (int i = s->count() - 1; x > 0 && s->has(i); x /= base, --i) {
        const char *fig =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        s->at(i) = fig[x % base];
    }
    return s;
}

String fnum(float64_t x, int precision = 16, int base = 10, int screen = 6);
String fixed(float64_t x, int nf);

template<class T>
inline String dec(T x, int n = -1) { return inum(x, 10, n); }

template<class T>
inline String hex(T x, int n = -1) { return inum(x, 16, n); }

template<class T>
inline String oct(T x, int n = -1) { return inum(x, 8, n); }

template<class T>
inline String bin(T x, int n = -1) { return inum(x, 2, n); }

inline String sci(float32_t x, int p = 8) { return fnum(x, p, 10, 0); }
inline String sci(float64_t x, int p = 17) { return fnum(x, p, 10, 0); }

inline String dec(float32_t x, int p = 8) { return fnum(x, p, 10, 6); }
inline String dec(float64_t x, int p = 17) { return fnum(x, p, 10, 6); }

String dec(const Variant &x, int n = -1);

inline String str(const String &s) { return s; }
inline String str(ByteArray *s) { return String(s); }
inline String str(const char *s) { return String(s); }
inline String str(char *s) { return String(s); }
inline String str(char ch) { return String(&ch, 1); }
String str(uchar_t ch);

inline String str(bool x) { return x ? "true" : "false"; }

String str(void *x);

template<class T>
inline String str(const Ref<T> &x) { return x->toString(); }

template<class T>
inline String str(const T *x) { return x ? x->toString() : String("null"); }

inline String str(unsigned char x) { return dec(x); }
inline String str(unsigned short x) { return dec(x); }
inline String str(unsigned int x) { return dec(x); }
inline String str(unsigned long x) { return dec(x); }
inline String str(unsigned long long x) { return dec(x); }
inline String str(short x) { return dec(x); }
inline String str(int x) { return dec(x); }
inline String str(long x) { return dec(x); }
inline String str(long long x) { return dec(x); }
inline String str(float x) { return dec(x); }
inline String str(double x) { return dec(x); }

/** Align a string to the left
  * \param s input string
  * \param w width of display field
  * \param blank fill character
  * \return left aligned string
  */
String left(const String &s, int w, char blank = ' ');

/** Align a string to the right
  * \param s input string
  * \param w width of display field
  * \param blank fill character
  * \return right aligned string
  */
String right(const String &s, int w, char blank = ' ');

/** \}
  */

} // namespace cc
