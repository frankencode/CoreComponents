/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_FLOATSYNTAX_H
#define FLUX_FLOATSYNTAX_H

#include <flux/SyntaxDefinition>

namespace flux {

template<class SubClass> class Singleton;

/** \brief Syntax of a floating point literal
  * \see IntegerSyntax
  */
class FloatSyntax: public SyntaxDefinition
{
public:
    void read(float64_t *value, const ByteArray *text, Token *token) const;
    Ref<Token> read(float64_t *value, const ByteArray *text, int i = -1) const;

    inline int literal() const { return literal_; }

private:
    friend class Singleton<FloatSyntax>;

    FloatSyntax();

    int sign_;
    int integerPart_;
    int fractionPart_;
    int exponentSign_;
    int exponent_;
    int nan_;
    int infinite_;
    int literal_;
};

const FloatSyntax *floatSyntax();

} // namespace flux

#endif // FLUX_FLOATSYNTAX_H
