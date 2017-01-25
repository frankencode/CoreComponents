/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h> // ssize_t, etc.
#include <stdint.h> // (u)int8_t .. (u)int64_t
#include <cc/assert>

typedef float float32_t;
typedef double float64_t;

namespace cc {

/** Byte order, also known as "Endianess"
  */
enum Endian {
    LittleEndian = 1, ///< Low order byte precedes the high order byte
    BigEndian = 0 ///< High order byte precedes the low order byte
};

inline Endian localEndian() {
    const unsigned y = 1;
    return Endian(*((uint8_t *)&y));
}

//! swap endianess, if local endian is unequal channel endian
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
    CC_ASSERT(sizeof(A) == sizeof(B));
    Bimorph morph;
    morph.a = a;
    return morph.b;
}

inline float64_t nan() { return union_cast<float64_t>(uint64_t(0xFFF8) << 48); }
inline float64_t inf() { return union_cast<float64_t>(uint64_t(0x7FF) << 52); }

const unsigned unsignedMax = unsigned(-1);
const int intMax = unsignedMax >> 1;

inline void *addressMax()
{
    void *p = 0;
    unsigned char *u = (unsigned char *)&p;
    for (unsigned i = 0; i < sizeof(void *) / sizeof(unsigned char); ++i)
        u[i] = 0xFF;
    return p;
}

template<class T> class IsAtomic { public: enum { value = 0 }; };
template<> class IsAtomic<const int8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const int64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint8_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint16_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const uint64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const float32_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const float64_t> { public: enum { value = 1 }; };
template<> class IsAtomic<const bool> { public: enum { value = 1 }; };
template<class T> class IsAtomic<const T*> { public: enum { value = 1 }; };

#define CC_IS_ATOMIC(T) (IsAtomic<const T>::value == 1)

template<class T> class IsPointer { public: enum { value = 0 }; };
template<class T> class IsPointer<T *> { public: enum { value = 1 }; };

#define CC_IS_POINTER(T) (IsPointer<T>::value == 1)

template<class T> class DerefPointerType { public: typedef T DerefType; };
template<class T> class DerefPointerType<T *> { public: typedef T DerefType; };

#define CC_DEREF(T) DerefPointerType<T>::DerefType

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

template<class T, class U, int ConversionPossible>
class DynamicCastHelper {
public:
    static_assert(ConversionPossible, "Impossible type conversion requested");
    inline static U *cast(T *p) { return dynamic_cast<U*>(p); }
};

template<class T, class U, int ConversionExists = -1>
class CastHelper {};

template<class T, class U>
class CastHelper<T, U, 0> {
public:
    inline static U *cast(T *p) { return DynamicCastHelper<T, U, ConversionFromTo<U*, T*>::Exists>::cast(p); }
};

template<class T, class U>
class CastHelper<T, U, 1> {
public:
    inline static U *cast(T *p) { return static_cast<U*>(p); }
};

template<class T>
class CastHelper<T, T, 1> {
public:
    inline static T *cast(T *p) { return p; }
};

template<class T>
class CastHelper<const T, T, 1> {
public:
    inline static T *cast(T *p) { return p; }
};

#define CC_CAST_FROM_TO(T, U, p) CastHelper<T, U, ConversionFromTo<T*, U*>::Exists>::cast(p)

/** %Unicode character type (stores an %Unicode code point)
  * \ingroup unicode
  */
typedef char32_t uchar_t;

template<class Char>
inline Char downcase(Char ch) {
    if (('A' <= ch) && (ch <= 'Z'))
        ch = ch + ('a' - 'A');
    return ch;
}

template<class Char>
inline Char upcase(Char ch) {
    if (('a' <= ch) && (ch <= 'z'))
        ch = ch - ('a' - 'A');
    return ch;
}

template<class Char>
inline bool isLower(Char ch) {
    return ('a' <= ch) && (ch <= 'z');
}

template<class Char>
inline bool isUpper(Char ch) {
    return ('A' <= ch) && (ch <= 'Z');
}

template<class Char>
inline bool isSpace(Char ch) {
    return (ch == ' ') || (ch == '\t');
}

template<class T>
class Identity {
public:
    inline static T map(const T &x) { return x; }
};

template<class T>
class ToLower {
public:
    inline static T map(const T &x) { return cc::downcase(x); }
};

template<class T>
class ToUpper {
public:
    inline static T map(const T &x) { return cc::upcase(x); }
};

template<class T>
class ToAscii {
public:
    inline static char map(const T &x) { return char(x); }
};

template<class T>
class ToUnicode {
public:
    inline static uchar_t map(const T &x) { return uchar_t(x); }
};

class NonCopyable
{
protected:
    NonCopyable() {}
    NonCopyable(const NonCopyable &b);
    const NonCopyable &operator=(const NonCopyable &b);
};

} // namespace cc
