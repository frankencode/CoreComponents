/*
 * RegExp.hpp -- regular expression
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_REGEXP_HPP
#define FTL_REGEXP_HPP

#include "Syntax.hpp"

namespace ftl
{

class RegExp: public Syntax<ByteArray>::Definition
{
public:
	RegExp(const String& pattern);
};

} // namespace ftl

#endif // FTL_REGEXP_HPP
