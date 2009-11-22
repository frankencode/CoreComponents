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
template<> class IsAtomic<int8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<int16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<int32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<int64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<uint8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<uint16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<uint32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<uint64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<bool> { public: enum { value = 1 }; };
template<class T> class IsAtomic<T*> { public: enum { value = 1 }; };

#define PONA_IS_ATOMIC(T) (IsAtomic<T>::value == 1)


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

#define PONA_DISABLE_COPY(ClassName) \
ClassName(const ClassName& b); \
const ClassName& operator=(const ClassName& b);

template<class T>
inline int strlen(const T* s)
{
	int len = 0;
	if (s)
		while (*(s + len)) ++len;
	return len;
}

} // namespace pona

#endif // PONA_TYPES_HPP
