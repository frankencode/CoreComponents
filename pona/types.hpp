/*
 * types.hpp -- built-in types and conventions
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TYPES_HPP
#define PONA_TYPES_HPP

#include <sys/types.h> // ssize_t, etc.
#include <assert.h> // design HACK, pona should support its own assertions
#include <stdint.h> // (u)int8_t .. (u)int64_t

typedef float float32_t;
typedef double float64_t;

namespace pona
{

enum Endian { BigEndian = 0, LittleEndian = 1 };

template<class B, class A>
inline B union_cast(A a)
{
	union Bimorph { A a; B b; };
	assert(sizeof(A) == sizeof(B));
	Bimorph morph;
	morph.a = a;
	return morph.b;
}

const double nan = union_cast<double>(uint64_t(0xFFF8) << 48);
const double inf = union_cast<double>(uint64_t(0x7FF) << 52);

const unsigned unsignedMax = unsigned(-1);
const int intMax = unsignedMax >> 1;

template<class T> class IsAtomic { public: enum { value = 0 }; };
template<> class IsAtomic<const int8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const bool> { public: enum { value = 1 }; };
template<class T> class IsAtomic<const T*> { public: enum { value = 1 }; };

#define PONA_IS_ATOMIC(T) (IsAtomic<const T>::value == 1)

struct None {};

// taken from Andrei Alexandrescu's book "Modern C++ Design"
template<class T, class U>
class ConversionFromTo
{
private:
	typedef char Small;
	class Big { char dummy[2]; };
	static Small test(U);
	static Big test(...);
	static T makeT();
public:
	enum {
		Exists = (sizeof(test(makeT())) == sizeof(Small)),
		SameType = 0
	};
};

template<class T>
class ConversionFromTo<T, T> {
public:
	enum { Exists = 1, SameType = 1 };
};

template<class T, class U, int ConversionPossible = 0>
class DynamicCastHelper {};

template<class T, class U>
class DynamicCastHelper<T, U, 1> {
public:
	inline static U* cast(T* p) { return dynamic_cast<U*>(p); }
};

template<class T, class U, int ConversionExists = -1>
class CastHelper {};

template<class T, class U>
class CastHelper<T, U, 0> {
public:
	inline static U* cast(T* p) { return DynamicCastHelper<T, U, ConversionFromTo<const U*, const T*>::Exists>::cast(p); }
};

template<class T, class U>
class CastHelper<T, U, 1> {
public:
	inline static U* cast(T* p) { return static_cast<U*>(p); }
};

#define PONA_CAST_FROM_TO(T, U, p) CastHelper<T, U, ConversionFromTo<const T*, const U*>::Exists>::cast(p)

class Char
{
public:
	Char() {}
	Char(uint32_t ch): ch_(ch) {}
	inline const Char& operator=(uint32_t ch) { ch_ = ch; return *this; }
	inline operator uint32_t() const { return ch_; }
private:
	uint32_t ch_;
};

template<class T>
inline int strlen(const T* s)
{
	int len = 0;
	if (s)
		while (*(s + len) != 0) ++len;
	return len;
}

template<class T>
inline char* strdup(const T* s)
{
	int len = pona::strlen(s);
	char* buf = new char[len + 1];
	buf[len] = 0;
	for (int i = 0; i < len; ++i) buf[i] = s[i];
	return buf;
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
	assert(j == len);
	return c;
}

inline void* malloc(int size) { return new char[size]; }
inline void free(void* buf) { delete[] reinterpret_cast<char*>(buf); }

} // namespace pona

#endif // PONA_TYPES_HPP
