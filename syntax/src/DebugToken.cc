/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/syntax/DebugToken>

namespace cc {
namespace syntax {

void DebugToken::printTo(Stream *stream, CharArray *text, int depth, int defaultScope)
{
    if (defaultScope == -1) defaultScope = scope();

    string indent(depth * 2, ' ');
    string subIndent((depth + 1) * 2, ' ');

    {
        format format(stream);
        format << indent;
        if (scope() != defaultScope) format << scopeName() << "::";
        format << ruleName() << "=" << "\"" << text->copyRange(this)->escape() << "\" (" << i0() << ":" << i1() - i0() << ")" << nl;
    }

    for (Token *token = firstChild(); token; token = token->nextSibling())
        Object::cast<DebugToken *>(token)->printTo(stream, text, depth + 1, defaultScope);

    // format(stream) << indent << "]" << nl;
}

}} // namespace cc::syntax
