/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/syntax/SyntaxDefinition>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace regexp {

using namespace cc::syntax;

/** \brief Syntax of a regular expression text pattern
  */
class RegExpSyntax: public SyntaxDefinition
{
public:
    static const RegExpSyntax *instance();

protected:
    friend class Singleton<RegExpSyntax>;
    friend class RegExp;

    RegExpSyntax();

    void compile(const ByteArray *text, SyntaxDefinition *definition) const;
    NODE compileChoice(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileSequence(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileAhead(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileBehind(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileCapture(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileReference(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    char readChar(const ByteArray *text, Token *token) const;
    String readString(const ByteArray *text, Token *token) const;
    NODE compileRangeMinMax(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileRangeExplicit(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
    NODE compileRepeat(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;

    int gap_;
    int any_;
    int boi_;
    int eoi_;
    int char_;
    int string_;
    int rangeMinMax_;
    int rangeExplicit_;
    int minRepeat_;
    int maxRepeat_;
    int repeat_;
    int sequence_;
    int group_;
    int ahead_;
    int behind_;
    int identifier_;
    int capture_;
    int replay_;
    int choice_;
    int pattern_;
};

}} // namespace cc::regexp
