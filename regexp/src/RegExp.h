/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/syntax/exceptions>
#ifndef NDEBUG
#include <cc/syntax/SyntaxDebugger>
#endif
#include <cc/syntax/SyntaxDefinition>

namespace cc {
namespace regexp {

using namespace cc::syntax;

/** \brief Regular expression pattern
  * \see SyntaxDefinition
  */
class RegExp: public Ref<SyntaxDefinition>
{
public:
    RegExp();

    RegExp(const char *text);
    RegExp(const String &text);
    RegExp(const Variant &Variant);

    const RegExp &operator=(const char *text);
    const RegExp &operator=(const String &text);
    const RegExp &operator=(const Variant &text);

    inline operator String() const { return text_; }

private:
    friend String str(const RegExp &pattern);
    String text_;
};

inline String str(const RegExp &pattern) { return pattern.text_; }

}} // namespace cc::regexp
