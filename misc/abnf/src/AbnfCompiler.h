/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDebugger>
#include <cc/abnf/AbnfSyntax>

namespace cc {
namespace abnf {

using namespace cc::syntax;

class StringTrap;

class AbnfCompiler: public AbnfSyntax
{
public:
    inline static Ref<AbnfCompiler> create() { return new AbnfCompiler; }
    Ref<SyntaxDefinition> compile(ByteArray *text, SyntaxDebugger *debugger = 0);

private:
    const char *str(ByteArray *text, Token *token);
    Ref<StringTrap> trap_;

    AbnfCompiler();
    SyntaxNode *ignoreDebug(SyntaxNode *node);

    void compileRuleList(ByteArray *text, Token *ruleList, SyntaxDefinition *definition);
    void compileEntry(ByteArray *text, Token *ruleList, SyntaxDefinition *definition);
    NODE compileAlternation(ByteArray *text, Token *alternation, SyntaxDefinition *definition);
    NODE compileConcatenation(ByteArray *text, Token *concatenation, SyntaxDefinition *definition);
    NODE compileRepetition(ByteArray *text, Token *repetition, SyntaxDefinition *definition);
    NODE compileOption(ByteArray *text, Token *option, SyntaxDefinition *definition);
    NODE compileElement(ByteArray *text, Token *element, SyntaxDefinition *definition);
    NODE compileNumVal(ByteArray *text, Token *numVal, SyntaxDefinition *definition);
    NODE compileCharVal(ByteArray *text, Token *charVal, SyntaxDefinition *definition);
    NODE compileProseVal(ByteArray *text, Token *proseVal, SyntaxDefinition *definition);

    NODE optimizeChoice(SyntaxNode *choice, SyntaxDefinition *definition);
    void deepOptimizeChoice(SyntaxNode *node, SyntaxDefinition *definition);
    void deepOptimizeChoice(SyntaxNode *node, SyntaxNode *fin, int numChars, SyntaxDefinition *definition);
};

}} // namespace cc::abnf

