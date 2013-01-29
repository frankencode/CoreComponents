 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "format.hpp"
#include "variant.hpp"
#include "string.hpp"

namespace ftl
{

string::string(const variant &b)
	: Super(cast<ByteArray>(b))
{
	if (!Super::get()) Super::set(ByteArray::empty());
}

string::string(const format &b)
{
	*this = *ByteArray::join(b);
}

string::string(hook<StringList> parts) { *this = join(parts); }

} // namespace ftl
