/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ScriptDiscoverySyntax"

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
