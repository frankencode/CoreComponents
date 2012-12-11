/*
 * Base64.hpp -- base64 codec according to RFC4648
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_BASE64_HPP
#define FTL_BASE64_HPP

#include "String.hpp"

namespace ftl
{

class Base64: public Instance
{
public:
	static Ref<ByteArray, Owner> encode(Ref<ByteArray> source);
	static Ref<ByteArray, Owner> decode(Ref<ByteArray> source);
};

} // namespace ftl

#endif // FTL_BASE64_HPP
