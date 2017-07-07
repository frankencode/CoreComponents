/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/abnf/AbnfSyntaxDefinition>

namespace cc {
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

}} // namespace cc::abnf

