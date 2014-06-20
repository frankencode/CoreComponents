/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "Language.h"

namespace flux {
namespace toki {

Language::Language(
	String displayName,
	Pattern pathPattern,
	const SyntaxDefinition *highlightingSyntax,
	const SyntaxDefinition *discoverySyntax,
	const SyntaxDefinition *foldingSyntax
)
	: displayName_(displayName),
	  name_(highlightingSyntax->name()),
	  pathPattern_(pathPattern),
	  highlightingSyntax_(highlightingSyntax),
	  discoverySyntax_(discoverySyntax),
	  foldingSyntax_(foldingSyntax)
{
	registry()->registerLanguage(this);
}

}} // namespace flux::toki
