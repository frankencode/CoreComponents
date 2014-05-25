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

namespace fluxtoki
{

class YasonSyntax: public flux::YasonSyntax
{
protected:
	friend class flux::Singleton<YasonSyntax>;

	YasonSyntax()
		: flux::YasonSyntax(GenerateComments|GenerateEscapedChars)
	{}
};

flux::YasonSyntax *yasonSyntax() { return flux::Singleton<fluxtoki::YasonSyntax>::instance(); }

} // namespace fluxtoki
