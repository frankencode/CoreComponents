/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXABNF_ABNFSYNTAX_H
#define FLUXABNF_ABNFSYNTAX_H

#include <flux/abnf/AbnfSyntaxDefinition>

namespace flux {
namespace abnf {

class AbnfSyntax: public AbnfSyntaxDefinition
{
protected:
    AbnfSyntax();
    NODE defineValue(const char *digitRule);

    int numVal_;
    int charVal_;
    int proseVal_;
    int group_;
    int option_;
    int element_;
    int repeat_;
    int repetition_;
    int concatenation_;
    int alternation_;
    int ruleName_;
    int definedAs_;
    int rule_;
    int rulelist_;
};

}} // namespace flux::abnf

#endif // FLUXABNF_ABNFSYNTAX_H
