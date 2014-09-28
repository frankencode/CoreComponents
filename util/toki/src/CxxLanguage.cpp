/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Registry.h"
#include "CppDiscoverySyntax.h"
#include "CxxSyntax.h"

namespace flux {
namespace toki {

class CxxLanguage: public Language
{
private:
	friend class Registration<CxxLanguage>;

	CxxLanguage():
		Language(
			"C++",
			Pattern("*.((h|c){0..1:(pp|xx|++|h|c)}|(m|mm))"),
			cxxSyntax(),
			cppDiscoverySyntax()
		)
	{}
};

namespace { Registration<CxxLanguage> registration; }

}} // namespace flux::toki