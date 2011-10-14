/*
 * BlueIdentifier.hpp -- Blue identifier
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BLUEIDENTIFIER_HPP
#define FTL_BLUEIDENTIFIER_HPP

#include "Crc32.hpp"

namespace ftl
{

class BlueIdentifier
{
public:
	BlueIdentifier()
		: id_(Crc32().sum())
	{}
	
	BlueIdentifier(const char* name)
		: id_(crc32(name))
	{}
	
	inline operator uint32_t() const { return id_; }
	
private:
	uint32_t id_;
};

} // namespace ftl

#endif // FTL_BLUEIDENTIFIER_HPP
