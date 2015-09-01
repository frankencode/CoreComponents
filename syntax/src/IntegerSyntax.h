/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_INTEGERSYNTAX_H
#define FLUXSYNTAX_INTEGERSYNTAX_H

#include <flux/syntax/SyntaxDefinition>

namespace flux {

/** \brief Syntax of an integer literal
  * \see FloatSyntax
  */
class IntegerSyntax: public SyntaxDefinition
{
public:
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

const IntegerSyntax *integerSyntax();

} // namespace flux

#endif // FLUXSYNTAX_INTEGERSYNTAX_H
