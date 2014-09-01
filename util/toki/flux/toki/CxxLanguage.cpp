/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
