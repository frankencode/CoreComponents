/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/glob/PatternSyntax>
#include <cc/glob/Pattern>

namespace cc {
namespace glob {

Pattern::Pattern() { *this = String(); }
Pattern::Pattern(const char *text) { *this = String(text); }
Pattern::Pattern(const String &text) { *this = text; }
Pattern::Pattern(const Variant &text) { *this = String(text); }

const Pattern &Pattern::operator=(const char *text)
{
    return *this = String(text);
}

const Pattern &Pattern::operator=(const String &text)
{
    if (text_ != text || !get()) {
        text_ = text;
        set(
            SyntaxDefinition::create(
                #ifndef NDEBUG
                SyntaxDebugger::create()
                #endif
            )
        );
        PatternSyntax::instance()->compile(text_, *this);
    }
    return *this;
}

const Pattern &Pattern::operator=(const Variant &text)
{
    return *this = String(text);
}

}} // namespace cc::glob
