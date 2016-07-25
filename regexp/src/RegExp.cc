/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/regexp/RegExpSyntax>
#include <cc/regexp/RegExp>

namespace cc {
namespace regexp {

RegExp::RegExp() { *this = String(); }
RegExp::RegExp(const char *text) { *this = String(text); }
RegExp::RegExp(const String &text) { *this = text; }
RegExp::RegExp(const Variant &text) { *this = String(text); }

const RegExp &RegExp::operator=(const char *text)
{
    return *this = String(text);
}

const RegExp &RegExp::operator=(const String &text)
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
        RegExpSyntax::instance()->compile(text_, *this);
    }
    return *this;
}

const RegExp &RegExp::operator=(const Variant &text)
{
    return *this = String(text);
}

}} // namespace cc::regexp
