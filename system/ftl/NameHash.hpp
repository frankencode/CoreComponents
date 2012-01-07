/*
 * NameHash.hpp -- hash of an object name
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
