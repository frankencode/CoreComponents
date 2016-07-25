/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/syntax/DebugToken>

namespace cc {
namespace syntax {

void DebugToken::printTo(Stream *stream, ByteArray *text, int depth, int defaultScope)
{
    if (defaultScope == -1) defaultScope = scope();

    String indent(depth * 2, ' ');
    String subIndent((depth + 1) * 2, ' ');

    {
        Format format(stream);
        format << indent;
        if (scope() != defaultScope) format << scopeName() << "::";
        format << ruleName() << "=" << "\"" << text->copyRange(this)->escape() << "\" (" << i0() << ":" << i1() - i0() << ")" << nl;
    }

    for (Token *token = firstChild(); token; token = token->nextSibling())
        cast<DebugToken>(token)->printTo(stream, text, depth + 1, defaultScope);

    // Format(stream) << indent << "]" << nl;
}

}} // namespace cc::syntax
