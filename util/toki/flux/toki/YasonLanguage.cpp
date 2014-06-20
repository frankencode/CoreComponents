/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "YasonSyntax.h"
#include "YasonLanguage.h"

namespace flux {
namespace toki {

YasonLanguage::YasonLanguage():
	Language(
		"yason",
		Pattern("(*.(yason|json))|Recipe"),
		flux::toki::yasonSyntax()
	)
{}

}} // flux::toki
