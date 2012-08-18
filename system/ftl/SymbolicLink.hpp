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
	static String read(String linkPath);
	static String resolve(String linkPath);
	static void create(String origPath, String aliasPath);
};

} // namespace ftl

#endif // FTL_SYMBOLICLINK_HPP
