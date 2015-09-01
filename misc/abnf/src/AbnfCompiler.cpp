/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/assert>
#include <flux/syntax/SyntaxDebugger>
#include <flux/abnf/AbnfSyntax>
#include "StringTrap.h"
#include <flux/abnf/AbnfCompiler>

namespace flux {
namespace abnf {

AbnfCompiler::AbnfCompiler()
    : trap_(StringTrap::create())
{}

Ref<AbnfCompiler::SyntaxDefinition> AbnfCompiler::compile(ByteArray *text, SyntaxDebugger *debugger)
{
    Ref<Token> ruleList = AbnfSyntax::match(text)->rootToken();
    FLUX_ASSERT(ruleList);

    Ref<SyntaxDefinition> definition = AbnfSyntaxDefinition::create(debugger);
    definition->OPTION("caseSensitive", false);

    compileRuleList(text, ruleList, definition);
    compileEntry(text, ruleList, definition);

    definition->LINK();

    return definition;
}

const char *AbnfCompiler::str(ByteArray *text, Token *token)
{
    return trap_->capture(text, token->i0(), token->i1());
}

SyntaxNode *AbnfCompiler::ignoreDebug(SyntaxNode *node)
{
    SyntaxDebugNode *debugNode = cast<SyntaxDebugNode>(node);
    return (debugNode) ? debugNode->entry() : node;
}

void AbnfCompiler::compileRuleList(ByteArray *text, Token *ruleList, SyntaxDefinition *definition)
{
    FLUX_ASSERT(ruleList->rule() == AbnfSyntax::rulelist_);

    Token *rule = ruleList->firstChild();
    while (rule) {
        FLUX_ASSERT(rule->rule() == AbnfSyntax::rule_);

        Token *ruleName = rule->firstChild();
        Token *definedAs = ruleName->nextSibling();
        Token *alternation = definedAs->nextSibling();

        FLUX_ASSERT(ruleName->rule() == AbnfSyntax::ruleName_);
        FLUX_ASSERT(definedAs->rule() == AbnfSyntax::definedAs_);
        FLUX_ASSERT(alternation->rule() == AbnfSyntax::alternation_);

        if (text->at(definedAs->i0()) == '=')
            definition->DEFINE(str(text, ruleName), compileAlternation(text, alternation, definition));
        else if (text->at(definedAs->i0()) == '~')
            definition->DEFINE_VOID(str(text, ruleName), compileAlternation(text, alternation, definition));
        else
            FLUX_ASSERT(false);

        rule = rule->nextSibling();
    }
}

void AbnfCompiler::compileEntry(ByteArray *text, Token *ruleList, SyntaxDefinition *definition)
{
    Token *rule = ruleList->firstChild();
    FLUX_ASSERT(rule);
    FLUX_ASSERT(rule->rule() == AbnfSyntax::rule_);
    Token *ruleName = rule->firstChild();
    FLUX_ASSERT(ruleName->rule() == AbnfSyntax::ruleName_);

    definition->ENTRY(str(text, ruleName));
}

AbnfCompiler::NODE AbnfCompiler::compileAlternation(ByteArray *text, Token *alternation, SyntaxDefinition *definition)
{
    FLUX_ASSERT(alternation->rule() == AbnfSyntax::alternation_);
    if (alternation->firstChild() == alternation->lastChild())
        return compileConcatenation(text, alternation->firstChild(), definition);
    NODE node = definition->CHOICE();
    Token *token = alternation->firstChild();
    while (token) {
        ignoreDebug(node)->appendChild(compileConcatenation(text, token, definition));
        token = token->nextSibling();
    }
    return optimizeChoice(node, definition);
}

AbnfCompiler::NODE AbnfCompiler::compileConcatenation(ByteArray *text, Token *concatenation, SyntaxDefinition *definition)
{
    FLUX_ASSERT(concatenation->rule() == AbnfSyntax::concatenation_);
    if (concatenation->firstChild() == concatenation->lastChild())
        return compileRepetition(text, concatenation->firstChild(), definition);
    NODE node = definition->GLUE();
    Token *token = concatenation->firstChild();
    while (token) {
        ignoreDebug(node)->appendChild(compileRepetition(text, token, definition));
        token = token->nextSibling();
    }
    return node;
}

AbnfCompiler::NODE AbnfCompiler::compileRepetition(ByteArray *text, Token *repetition, SyntaxDefinition *definition)
{
    NODE node = 0;
    Token *token = repetition->firstChild();
    if (token->rule() == AbnfSyntax::repeat_) {
        int i = token->i0();
        while (i < token->i1()) {
            if (text->at(i) == '*') break;
            ++i;
        }
        int repeatMin = 0;
        int repeatMax = intMax;
        if (i < token->i1()) {
            if (i > 0) repeatMin = strToInt(*text, token->i0(), i);
            if (i + 1 < token->i1()) repeatMax = strToInt(*text, i + 1, token->i1());
        }
        else {
            repeatMin = repeatMax = strToInt(*text, token->i0(), token->i1());
        }
        if ((repeatMin == 0) && (repeatMax == 0))
            node = definition->PASS();
        else
            node = definition->REPEAT(repeatMin, repeatMax, compileElement(text, token->nextSibling(), definition));
    }
    else if (token->rule() == AbnfSyntax::element_) {
        node = compileElement(text, token, definition);
    }
    else if (token->rule() == AbnfSyntax::option_) {
        node = compileOption(text, token, definition);
    }
    else {
        FLUX_ASSERT(false);
    }
    return node;
}

AbnfCompiler::NODE AbnfCompiler::compileOption(ByteArray *text, Token *option, SyntaxDefinition *definition)
{
    FLUX_ASSERT(option->rule() == AbnfSyntax::option_);
    Token *alternation = option->firstChild();
    FLUX_ASSERT(alternation);
    return definition->REPEAT(0, 1, compileAlternation(text, alternation, definition));
}

AbnfCompiler::NODE AbnfCompiler::compileElement(ByteArray *text, Token *element, SyntaxDefinition *definition)
{
    NODE node = 0;
    FLUX_ASSERT(element->rule() == AbnfSyntax::element_);
    Token *token = element->firstChild();
    if (token->rule() == AbnfSyntax::ruleName_) {
        node = definition->REF(str(text, token));
    }
    else if (token->rule() == AbnfSyntax::group_) {
        node = compileAlternation(text, token->firstChild(), definition);
    }
    else if (token->rule() == AbnfSyntax::numVal_) {
        node = compileNumVal(text, token, definition);
    }
    else if (token->rule() == AbnfSyntax::charVal_) {
        node = compileCharVal(text, token, definition);
    }
    else if (token->rule() == AbnfSyntax::proseVal_) {
        node = compileProseVal(text, token, definition);
    }
    else {
        FLUX_ASSERT(false);
    }
    return node;
}

AbnfCompiler::NODE AbnfCompiler::compileNumVal(ByteArray *text, Token *numVal, SyntaxDefinition *definition)
{
    NODE node = 0;

    FLUX_ASSERT(numVal->rule() == AbnfSyntax::numVal_);
    FLUX_ASSERT(text->at(numVal->i0()) == '%');
    char prefix = downcase(text->at(numVal->i0() + 1));
    int base;
    if (prefix == 'x')
        base = 16;
    else if (prefix == 'b')
        base = 2;
    else // if (prefix == 'd');
        base = 10;

    int i = numVal->i0() + 2;
    while ((i < numVal->i1()) && (text->at(i) != '-') && (text->at(i) != '.')) ++i;

    if (i < numVal->i1()) {
        if (text->at(i) == '-') {
            int a = strToInt(*text, numVal->i0() + 2, i, base);
            int b = strToInt(*text, i + 1, numVal->i1(), base);
            node = definition->RANGE(a, b);
        }
        else if (text->at(i) == '.') {
            int n = 1;
            for (int i = numVal->i0() + 2; i < numVal->i1(); ++i) {
                n += text->at(i) == '.';
                ++i;
            }
            Ref<ByteArray> s = ByteArray::create(n);
            int i0 = numVal->i0() + 2;
            int i = i0;
            int j = 0;
            while (i < numVal->i1()) {
                if (text->at(i) == '.') {
                    s->at(j) = strToInt(*text, i0, i, base);
                    ++j;
                    i0 = i + 1;
                }
                ++i;
            }
            FLUX_ASSERT(j == s->count() - 1);
            s->at(j) = strToInt(*text, i0, i, base);
            node = (s->count() > 1) ? definition->STRING(s->chars()) : definition->CHAR(s->at(0));
        }
        else {
            FLUX_ASSERT(false);
        }
    }
    else {
        char ch = strToInt(*text, numVal->i0() + 2, numVal->i1(), base);
        node = definition->CHAR(ch);
    }

    return node;
}

AbnfCompiler::NODE AbnfCompiler::compileCharVal(ByteArray *text, Token *charVal, SyntaxDefinition *definition)
{
    return
        (charVal->i1() - charVal->i0() - 2 > 1) ?
            definition->STRING(text->copy(charVal->i0() + 1, charVal->i1() - 1)->chars()) :
            definition->CHAR(text->at(charVal->i0() + 1));
}

AbnfCompiler::NODE AbnfCompiler::compileProseVal(ByteArray *text, Token *proseVal, SyntaxDefinition *definition)
{
    return compileCharVal(text, proseVal, definition);
}

AbnfCompiler::NODE AbnfCompiler::optimizeChoice(SyntaxNode *node, SyntaxDefinition *definition)
{
    NODE optimizedChoice = node;

    int numChars = 0;
    bool isRangeExplicit = true;
    {
        SyntaxNode *child = ignoreDebug(node)->firstChild();
        while ((child) && isRangeExplicit) {
            isRangeExplicit = cast<syntax::CharNode>(ignoreDebug(child));
            child = child->nextSibling();
            ++numChars;
        }
    }

    if (isRangeExplicit) {
        Ref<ByteArray> s = ByteArray::create(numChars);
        int i = 0;
        SyntaxNode *child = ignoreDebug(node)->firstChild();
        while (child) {
            Ref<syntax::CharNode> charNode = cast<syntax::CharNode>(ignoreDebug(child));
            s->at(i) = charNode->ch();
            ++i;
            child = child->nextSibling();
        }

        optimizedChoice = definition->RANGE(s->chars());
    }
    else {
        deepOptimizeChoice(node, definition);
    }

    return optimizedChoice;
}

void AbnfCompiler::deepOptimizeChoice(SyntaxNode *node, SyntaxDefinition *definition)
{
    int numChars = 0;
    SyntaxNode *child = ignoreDebug(node)->firstChild();
    while (child) {
        if (cast<syntax::CharNode>(ignoreDebug(child)))
            ++numChars;
        else
            deepOptimizeChoice(node, child, numChars, definition);
        child = child->nextSibling();
    }
    deepOptimizeChoice(node, 0, numChars, definition);
}

void AbnfCompiler::deepOptimizeChoice(SyntaxNode *node, SyntaxNode *fin, int numChars, SyntaxDefinition *definition)
{
    if (numChars > 1) {
        Ref<ByteArray> s = ByteArray::create(numChars);
        int i = numChars - 1;
        while (i >= 0) {
            SyntaxNode *charNode = (fin) ? fin->previousSibling() : ignoreDebug(node)->lastChild();
            s->at(i) = cast<syntax::CharNode>(ignoreDebug(charNode))->ch();
            charNode->unlink();
            --i;
        }
        NODE range = definition->RANGE(s->chars());
        if (fin)
            ignoreDebug(node)->insertChild(range, fin->previousSibling());
        else
            ignoreDebug(node)->appendChild(range);
    }
    numChars = 0;
}

}} // namespace flux::abnf
