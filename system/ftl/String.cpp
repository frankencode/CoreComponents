/*
 * String.cpp -- UTF-8 strings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Format.hpp"
#include "Variant.hpp"
#include "Path.hpp"
#include "String.hpp"

namespace ftl
{

String::String(const Variant& b)
	: Super(b.toInstance<ByteArray>())
{}

String::String(const Format& b)
{
	*this = *ByteArray::join(b);
}

String::String(const Path& b)
	: Super(b.toString().Super::get())
{}

} // namespace ftl
