/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "YasonSyntax.h"

namespace flux {
namespace toki {

class YasonLanguage: public Language
{
private:
	friend class Registration<YasonLanguage>;

	YasonLanguage():
		Language(
			"Yason",
			Pattern("(*.(yason|json))|Recipe"),
			flux::toki::yasonSyntax()
		)
	{}
};

namespace { Registration<YasonLanguage> registration; }

}} // namespace flux::toki
