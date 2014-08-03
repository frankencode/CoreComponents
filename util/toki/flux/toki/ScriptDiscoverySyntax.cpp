/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ScriptDiscoverySyntax.h"

namespace flux {
namespace toki {

Ref<ScriptDiscoverySyntax> ScriptDiscoverySyntax::create(const char *suffix)
{
	return new ScriptDiscoverySyntax(suffix);
}

ScriptDiscoverySyntax::ScriptDiscoverySyntax(const char *suffix)
{
	DEFINE("FirstLine",
		GLUE(
			STRING("#!"),
			LAZY_REPEAT(OTHER('\n')),
			STRING(suffix)
		)
	);

	ENTRY("FirstLine");
	LINK();
}

}} // namespace flux::toki
