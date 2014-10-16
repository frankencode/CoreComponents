/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include "YasonSyntax"

namespace flux {
namespace toki {

class YasonSyntax: public flux::YasonSyntax
{
protected:
	friend class flux::Singleton<YasonSyntax>;

	YasonSyntax()
		: flux::YasonSyntax(GenerateComments|GenerateEscapedChars)
	{}
};

flux::YasonSyntax *yasonSyntax() { return flux::Singleton<flux::toki::YasonSyntax>::instance(); }

}} // namespace flux::toki
