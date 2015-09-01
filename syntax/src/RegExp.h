/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_REGEXP_H
#define FLUXSYNTAX_REGEXP_H

#include <flux/String>
#include <flux/syntax/exceptions>
#ifndef NDEBUG
#include <flux/syntax/SyntaxDebugger>
#endif
#include <flux/syntax/SyntaxDefinition>

namespace flux {

/** \brief Regular expression pattern
  * \see SyntaxDefinition
  */
class RegExp: public Ref<SyntaxDefinition>
{
public:
    RegExp();

    RegExp(const char *text);
    RegExp(const String &text);
    RegExp(const Ref<ByteArray> &text);
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

} // namespace flux

#endif // FLUXSYNTAX_REGEXP_H
