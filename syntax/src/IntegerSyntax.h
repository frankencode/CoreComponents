/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace syntax {

/** \brief Syntax of an integer literal
  * \see FloatSyntax
  */
class IntegerSyntax: public SyntaxDefinition
{
public:
    static const IntegerSyntax *instance();

    void read(uint64_t *value, int *sign, const ByteArray *text, Token *token) const;
    Ref<Token> read(uint64_t *value, int *sign, const ByteArray *text, int i = -1) const;

protected:
    friend class Singleton<IntegerSyntax>;

    IntegerSyntax();

    int sign_;
    int binNumber_;
    int octNumber_;
    int hexNumber_;
    int decNumber_;
    int literal_;
};

}} // namespace cc::syntax
