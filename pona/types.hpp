/*
 * types.hpp -- built-in types and conventions
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TYPES_HPP
#define PONA_TYPES_HPP

#include <sys/types.h> // ssize_t, etc.
#include <stdint.h> // (u)int8_t .. (u)int64_t
#include "Exception.hpp"

typedef float float32_t;
typedef double float64_t;

namespace pona
{

enum Endian { LittleEndian = 1, BigEndian = 0 };

inline int localEndian() {
	const unsigned y = 1;
	return *((uint8_t*)&y);
}

// swap endianess, if local endian is not channel endian
template<class UInt>
inline UInt endianGate(UInt x, const int channelEndian = BigEndian)
{
	if (localEndian() != channelEndian) {
		const int n = sizeof(x);
		UInt z = 0;
		for (int i = 0; i < n; ++i) {
			z <<= 8;
			z |= (x >> (i * 8)) & 0xFF;
		}
		x = z;
	}
	return x;
}

template<class B, class A>
inline B union_cast(A a)
{
	union Bimorph { A a; B b; };
	check(sizeof(A) == sizeof(B));
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
protected:
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

typedef uint32_t uchar_t; // could also be a signed integer (int32_t)

inline char toLower(char ch) {
	if (('A' <= ch) && (ch <= 'Z'))
		ch = ch + 'a' - 'A';
	return ch;
}

inline char toUpper(char ch) {
	if (('a' <= ch) && (ch <= 'z'))
		ch = ch + 'A' - 'a';
	return ch;
}

inline char isLower(char ch) {
	return ('a' <= ch) && (ch <= 'z');
}

inline char isUpper(char ch) {
	return ('A' <= ch) && (ch <= 'Z');
}

inline bool isSpace(char ch) {
	return (ch == ' ') || (ch == '\t');
}

inline void* malloc(int size) { return new char[size]; }
inline void free(void* buf) { delete[] reinterpret_cast<char*>(buf); }

template<class T>
class Return: public T
{
public:
	Return(const T& b): T(b) {}
	inline operator T() { return *this; }
};

/*
template<
	template<class> class CA,
	template<class> class CB,
	class T
>
inline CA<T>& operator<<(CA<T>& ca, CB<T>& cb)
{
	while (!cb.empty()) {
		T item;
		cb >> item;
		ca << item;
	}
	return ca;
}*/

template<
	template<class> class CA,
	template<class> class CB,
	class T
>
inline CA<T>& operator>>(CA<T>& ca, CB<T>& cb)
{
	cb << ca;
	return ca;
}

template<class T>
class Ascending {
public:
	static inline bool below(const T& a, const T& b) { return a < b; }
};

template<class T>
class Descending {
public:
	static inline bool below(const T& a, const T& b) { return b < a; }
};

} // namespace pona

#endif // PONA_TYPES_HPP
