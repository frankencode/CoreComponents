/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "YasonSyntax.h"

namespace flux {
namespace toki {

class YasonSyntax: public flux::yason::YasonSyntax
{
protected:
	friend class flux::Singleton<YasonSyntax>;

	YasonSyntax()
		: flux::yason::YasonSyntax(GenerateComments|GenerateEscapedChars)
	{}
};

flux::yason::YasonSyntax *yasonSyntax() { return flux::Singleton<flux::toki::YasonSyntax>::instance(); }

}} // namespace flux::toki
