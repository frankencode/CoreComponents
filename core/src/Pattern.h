/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PATTERN_H
#define FLUX_PATTERN_H

#include <flux/String>
#include <flux/exceptions>
#ifndef NDEBUG
#include <flux/SyntaxDebugger>
#endif
#include <flux/SyntaxDefinition>

namespace flux {

class Pattern: public Ref<SyntaxDefinition>
{
public:
    Pattern();
    Pattern(const char *text);
    Pattern(const String &text);
    Pattern(const Ref<ByteArray> &text);
    Pattern(const Variant &Variant);

    const Pattern &operator=(const char *text);
    const Pattern &operator=(const String &text);
    const Pattern &operator=(const Variant &text);

    inline operator String() const { return text_; }

private:
    friend String str(const Pattern &pattern);
    String text_;
};

inline String str(const Pattern &pattern) { return pattern.text_; }

} // namespace flux

#endif // FLUX_PATTERN_H
