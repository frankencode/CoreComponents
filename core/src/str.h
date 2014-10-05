/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_STR_H
#define FLUX_STR_H

#include "String.h"
#include "Variant.h"

namespace flux
{

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
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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

inline String dec(float32_t x, int p = 8) { return fnum(x, p, 10); }
inline String dec(float64_t x, int p = 17) { return fnum(x, p, 10); }

String dec(const Variant &x, int n = -1);

inline String str(const String &s) { return s; }
inline String str(const Ref<ByteArray> &s) { return String(s); }
inline String str(ByteArray *s) { return String(s); }
inline String str(const char *s) { return String(s); }
inline String str(char *s) { return String(s); }
inline String str(char ch) { return String(&ch, 1); }

inline String str(bool x) { return x ? "true" : "false"; }

String str(void *x);

template<class T>
inline String str(const Ref<T> &x) { return x->toString(); }

template<class T>
inline String str(const T *x) { return x->toString(); }

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

String left(const String &s, int w, char blank = ' ');
String right(const String &s, int w, char blank = ' ');

} // namespace flux

#endif // FLUX_STR_H
