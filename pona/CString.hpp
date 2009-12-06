/*
 * CString.hpp -- null-terminated strings
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_CSTRING_HPP
#define PONA_CSTRING_HPP

#include "atoms"
#include "Array.hpp"

namespace pona
{

class CString: public Ref<Array<char>, OwnerInstance>
{
public:
	CString() {}
	CString(int length): Ref<Array<char>, OwnerInstance>(new Array<char>(length + 1)) { get()->set(length, 0); }
	inline operator char*() const { return get()->at(0); }
	inline operator void*() const { return (void*)get()->at(0); }
	inline operator uint8_t*() const { return (uint8_t*)get()->at(0); }
};

} // namespace pona

#endif // PONA_CSTRING_HPP
