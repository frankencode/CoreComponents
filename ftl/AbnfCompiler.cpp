/*
 * AbnfCompiler.cpp -- ABNF compiler according to RFC5234 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "SyntaxDebugger.hpp"
#include "AbnfSyntax.hpp"
#include "AbnfCompiler.hpp"

namespace ftl
{

Ref<AbnfCompiler::Definition, Owner> AbnfCompiler::compile(Ref<ByteArray> text, Ref<Debugger> debugger)
{
	Ref<Token, Owner> ruleList = AbnfSyntax::match(text);
	FTL_CHECK(ruleList);
	
	Ref<Definition, Owner> definition = new AbnfCoreSyntax(debugger);
	definition->OPTION("caseSensitive", false);
	
	compileRuleList(text, ruleList, definition);
	compileEntry(text, ruleList, definition);
	
	definition->LINK();
	
	return definition;
}

Ref<AbnfCompiler::Node> AbnfCompiler::ignoreDebug(Ref<Node> node)
{
	Ref<Debugger::DebugNode> debugNode = node;
	return (debugNode) ? debugNode->entry() : node;
}

void AbnfCompiler::compileRuleList(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition)
{
	FTL_CHECK(ruleList->rule() == AbnfSyntax::rulelist_);
	
	Ref<Token> rule = ruleList->firstChild();
	while (rule) {
		FTL_CHECK(rule->rule() == AbnfSyntax::rule_);
		
		Ref<Token> ruleName = rule->firstChild();
		Ref<Token> definedAs = ruleName->nextSibling();
		Ref<Token> alternation = definedAs->nextSibling();
		
		FTL_CHECK(ruleName->rule() == AbnfSyntax::ruleName_);
		FTL_CHECK(definedAs->rule() == AbnfSyntax::definedAs_);
		FTL_CHECK(alternation->rule() == AbnfSyntax::alternation_);
		
		if (text->at(definedAs->i0()) == '=')
			definition->DEFINE(str(text, ruleName), compileAlternation(text, alternation, definition));
		else if (text->at(definedAs->i0()) == '~')
			definition->DEFINE_VOID(str(text, ruleName), compileAlternation(text, alternation, definition));
		else
			FTL_CHECK(false);
		
		rule = rule->nextSibling();
	}
}

void AbnfCompiler::compileEntry(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition)
{
	Ref<Token> rule = ruleList->firstChild();
	FTL_CHECK(rule);
	FTL_CHECK(rule->rule() == AbnfSyntax::rule_);
	Ref<Token> ruleName = rule->firstChild();
	FTL_CHECK(ruleName->rule() == AbnfSyntax::ruleName_);
	
	definition->ENTRY(str(text, ruleName));
}

AbnfCompiler::NODE AbnfCompiler::compileAlternation(Ref<ByteArray> text, Ref<Token> alternation, Ref<Definition> definition)
{
	FTL_CHECK(alternation->rule() == AbnfSyntax::alternation_);
	if (alternation->firstChild() == alternation->lastChild())
		return compileConcatenation(text, alternation->firstChild(), definition);
	NODE node = definition->CHOICE();
	Ref<Token> token = alternation->firstChild();
	while (token) {
		ignoreDebug(node)->appendChild(compileConcatenation(text, token, definition));
		token = token->nextSibling();
	}
	return optimizeChoice(node, definition);
}

AbnfCompiler::NODE AbnfCompiler::compileConcatenation(Ref<ByteArray> text, Ref<Token> concatenation, Ref<Definition> definition)
{
	FTL_CHECK(concatenation->rule() == AbnfSyntax::concatenation_);
	if (concatenation->firstChild() == concatenation->lastChild())
		return compileRepetition(text, concatenation->firstChild(), definition);
	NODE node = definition->GLUE();
	Ref<Token> token = concatenation->firstChild();
	while (token) {
		ignoreDebug(node)->appendChild(compileRepetition(text, token, definition));
		token = token->nextSibling();
	}
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileRepetition(Ref<ByteArray> text, Ref<Token> repetition, Ref<Definition> definition)
{
	NODE node = 0;
	Ref<Token> token = repetition->firstChild();
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
		FTL_CHECK(false);
	}
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileOption(Ref<ByteArray> text, Ref<Token> option, Ref<Definition> definition)
{
	FTL_CHECK(option->rule() == AbnfSyntax::option_);
	Ref<Token> alternation = option->firstChild();
	FTL_CHECK(alternation);
	return definition->REPEAT(0, 1, compileAlternation(text, alternation, definition));
}

AbnfCompiler::NODE AbnfCompiler::compileElement(Ref<ByteArray> text, Ref<Token> element, Ref<Definition> definition)
{
	NODE node = 0;
	FTL_CHECK(element->rule() == AbnfSyntax::element_);
	Ref<Token> token = element->firstChild();
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
		FTL_CHECK(false);
	}
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileNumVal(Ref<ByteArray> text, Ref<Token> numVal, Ref<Definition> definition)
{
	NODE node = 0;
	
	FTL_CHECK(numVal->rule() == AbnfSyntax::numVal_);
	FTL_CHECK(text->at(numVal->i0()) == '%');
	char prefix = toLower(text->at(numVal->i0() + 1));
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
			ByteArray s(n);
			int i0 = numVal->i0() + 2;
			int i = i0;
			int j = 0;
			while (i < numVal->i1()) {
				if (text->at(i) == '.') {
					s.set(j, strToInt(*text, i0, i, base));
					++j;
					i0 = i + 1;
				}
				++i;
			}
			FTL_CHECK(j == s.size() - 1);
			s.set(j, strToInt(*text, i0, i, base));
			node = (s.size() > 1) ? definition->STRING(s.constData()) : definition->CHAR(s.at(0));
		}
		else {
			FTL_CHECK(false);
		}
	}
	else {
		Char ch = strToInt(*text, numVal->i0() + 2, numVal->i1(), base);
		node = definition->CHAR(ch);
	}
	
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileCharVal(Ref<ByteArray> text, Ref<Token> charVal, Ref<Definition> definition)
{
	return
		(charVal->length() - 2 > 1) ?
			definition->STRING(text->copy(charVal->i0() + 1, charVal->i1() - 1)->constData()) :
			definition->CHAR(text->at(charVal->i0() + 1));
}

AbnfCompiler::NODE AbnfCompiler::compileProseVal(Ref<ByteArray> text, Ref<Token> proseVal, Ref<Definition> definition)
{
	return compileCharVal(text, proseVal, definition);
}

AbnfCompiler::NODE AbnfCompiler::optimizeChoice(Ref<Node> node, Ref<Definition> definition)
{
	NODE optimizedChoice = node;
	
	int numChars = 0;
	bool isRangeExplicit = true;
	{
		Ref<Node> child = ignoreDebug(node)->firstChild();
		while ((child) && isRangeExplicit) {
			isRangeExplicit = Ref<CharNode>(ignoreDebug(child));
			child = child->nextSibling();
			++numChars;
		}
	}
	
	if (isRangeExplicit) {
		ByteArray s(numChars);
		int i = 0;
		Ref<Node> child = ignoreDebug(node)->firstChild();
		while (child) {
			Ref<CharNode> charNode = ignoreDebug(child);
			s.set(i, charNode->ch());
			++i;
			child = child->nextSibling();
		}
		
		optimizedChoice = definition->RANGE(s.constData());
	}
	else {
		deepOptimizeChoice(node, definition);
	}
	
	return optimizedChoice;
}

void AbnfCompiler::deepOptimizeChoice(Ref<Node> node, Ref<Definition> definition)
{
	int numChars = 0;
	Ref<Node> child = ignoreDebug(node)->firstChild();
	while (child) {
		if (Ref<CharNode>(ignoreDebug(child)))
			++numChars;
		else
			deepOptimizeChoice(node, child, numChars, definition);
		child = child->nextSibling();
	}
	deepOptimizeChoice(node, 0, numChars, definition);
}

void AbnfCompiler::deepOptimizeChoice(Ref<Node> node, Ref<Node> fin, int numChars, Ref<Definition> definition)
{
	if (numChars > 1) {
		ByteArray s(numChars);
		int i = numChars - 1;
		while (i >= 0) {
			Ref<Node> charNode = (fin) ? fin->previousSibling() : ignoreDebug(node)->lastChild();
			s.set(i, Ref<CharNode>(ignoreDebug(charNode))->ch());
			charNode->unlink();
			--i;
		}
		NODE range = definition->RANGE(s.constData());
		if (fin)
			ignoreDebug(node)->insertChild(range, fin->previousSibling());
		else
			ignoreDebug(node)->appendChild(range);
	}
	numChars = 0;
}

} // namespace ftl
