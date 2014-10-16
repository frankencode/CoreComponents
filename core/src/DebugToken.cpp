/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Format"
#include "DebugToken"

namespace flux
{

void DebugToken::printTo(Stream *stream, ByteArray *text, int depth, int defaultScope)
{
	if (defaultScope == -1) defaultScope = scope();

	String indent(depth * 2, ' ');
	String subIndent((depth + 1) * 2, ' ');

	{
		Format format(stream);
		format << indent;
		if (scope() != defaultScope) format << scopeName() << "::";
		format << ruleName() << "=" << "\"" << text->copy(this)->escape() << "\" (" << i0() << ":" << i1() - i0() << ")" << nl;
	}

	for (Token *token = firstChild(); token; token = token->nextSibling())
		cast<DebugToken>(token)->printTo(stream, text, depth + 1, defaultScope);

	// Format(stream) << indent << "]" << nl;
}

} // namespace flux
