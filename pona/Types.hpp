/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_TYPES_HPP
#define PONA_TYPES_HPP

#include <assert.h> // design HACK, pona should support its own assertions

#ifdef _MSC_VER
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

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

} // namespace pona

#endif // PONA_TYPES_HPP
