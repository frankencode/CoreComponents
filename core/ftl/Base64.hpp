 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
