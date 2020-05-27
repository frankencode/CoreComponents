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

Pattern::Pattern() { *this = string{}; }
Pattern::Pattern(const char *text) { *this = string{text}; }
Pattern::Pattern(const string &text) { *this = text; }
Pattern::Pattern(const variant &text) { *this = string(text); }

const Pattern &Pattern::operator=(const char *text)
{
    return *this = string{text};
}

const Pattern &Pattern::operator=(const string &text)
{
    if (text_ != text || !get()) {
        text_ = text;
        auto definition =
            SyntaxDefinition::create(
                #ifndef NDEBUG
                SyntaxDebugger::create()
                #endif
            );
        PatternSyntax::instance()->compile(text_, definition);
        set(definition);
    }
    return *this;
}

const Pattern &Pattern::operator=(const variant &text)
{
    return *this = string{text};
}

string str(const Pattern &pattern)
{
    return pattern.text_;
}

}} // namespace cc::glob
