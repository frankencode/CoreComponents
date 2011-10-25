/*
 * NameHash.hpp -- hash of an object name
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_NAMEHASH_HPP
#define FTL_NAMEHASH_HPP

#include "Crc32.hpp"

namespace ftl
{

class NameHash
{
public:
	Namehash()
		: id_(Crc32().sum())
	{}
	
	NameHash(const char* name)
		: id_(crc32(name))
	{}
	
	inline operator uint32_t() const { return id_; }
	
private:
	uint32_t id_;
};

} // namespace ftl

#endif // FTL_NAMEHASH_HPP
