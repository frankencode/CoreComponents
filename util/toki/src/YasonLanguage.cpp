/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Registry"
#include "YasonSyntax"

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
