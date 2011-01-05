/*
 * SymbolicLink.hpp -- symbolic links
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SYMBOLICLINK_HPP
#define FTL_SYMBOLICLINK_HPP

#include "String.hpp"

namespace ftl
{

class SymbolicLink
{
public:
	SymbolicLink(String path);
	String path() const;
	String read() const;
	String resolve() const;
	
private:
	String path_;
};

} // namespace ftl

#endif // FTL_SYMBOLICLINK_HPP
