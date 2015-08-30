/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/RegExpSyntax>
#include <flux/RegExp>

namespace flux {

RegExp::RegExp() { *this = String(); }
RegExp::RegExp(const char *text) { *this = String(text); }
RegExp::RegExp(const String &text) { *this = text; }
RegExp::RegExp(const Ref<ByteArray> &text) { *this = String(text); }
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
        regExpSyntax()->compile(text_, *this);
    }
    return *this;
}

const RegExp &RegExp::operator=(const Variant &text)
{
    return *this = String(text);
}

} // namespace flux
