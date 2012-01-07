/*
 * SymbolicLink.hpp -- symbolic links
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
