/*
 * AbnfCompiler.cpp -- ABNF compiler according to RFC5234 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "SyntaxDebugger.hpp"
#include "AbnfCompiler.hpp"

namespace ftl
{

AbnfCompiler::AbnfCompiler()
{
	DEFINE_VOID("comment",
		GLUE(
			CHAR(';'),
			REPEAT(
				CHOICE(
					INLINE("WSP"),
					INLINE("VCHAR")
				)
			),
			INLINE("CRLF")
		)
	);
	
	DEFINE_VOID("c-nl",
		CHOICE(
			INLINE("comment"),
			INLINE("CRLF")
		)
	);
	
	DEFINE_VOID("c-wsp",
		GLUE(
			REPEAT(0, 1, INLINE("c-nl")),
			INLINE("WSP")
		)
	);
	
	numVal_ =
		DEFINE("num-val",
			GLUE(
				CHAR('%'),
				CHOICE(
					GLUE(RANGE("bB"), defineValue("BIT")),
					GLUE(RANGE("dD"), defineValue("DIGIT")),
					GLUE(RANGE("xX"), defineValue("HEXDIG"))
				)
			)
		);
	
	charVal_ =
		DEFINE("char-val",
			GLUE(
				CHAR('"'),
				REPEAT(OTHER('"')), // deviation from RFC5234
				CHAR('"')
			)
		);
	
	proseVal_ =
		DEFINE("prose-val",
			GLUE(
				CHAR('<'),
				REPEAT(OTHER('>')), // deviation from RFC5234
				CHAR('>')
			)
		);
	
	group_ =
		DEFINE("group",
			GLUE(
				CHAR('('),
				REPEAT(INLINE("c-wsp")),
				REF("alternation"),
				REPEAT(INLINE("c-wsp")),
				CHAR(')')
			)
		);
	
	option_ =
		DEFINE("option",
			GLUE(
				CHAR('['),
				REPEAT(INLINE("c-wsp")),
				REF("alternation"),
				REPEAT(INLINE("c-wsp")),
				CHAR(']')
			)
		);
	
	element_ =
		DEFINE("element",
			CHOICE(
				REF("rulename"),
				REF("group"),
				// REF("option"), // correction over RFC5234
				REF("num-val"),
				REF("char-val"),
				REF("prose-val")
			)
		);
	
	repeat_ =
		DEFINE("repeat",
			CHOICE(
				GLUE(
					REPEAT(INLINE("DIGIT")),
					CHAR('*'),
					REPEAT(INLINE("DIGIT"))
				),
				REPEAT(1, INLINE("DIGIT"))
			)
		);
	
	repetition_ =
		DEFINE("repetition",
			CHOICE(
				GLUE(
					REPEAT(0, 1, REF("repeat")),
					REF("element")
				),
				REF("option")
			)
		);
	
	concatenation_ =
		DEFINE("concatenation",
			GLUE(
				REF("repetition"),
				REPEAT(
					GLUE(
						REPEAT(1, INLINE("c-wsp")),
						REF("repetition")
					)
				)
			)
		);
	
	alternation_ =
		DEFINE("alternation",
			GLUE(
				REF("concatenation"),
				REPEAT(
					GLUE(
						REPEAT(INLINE("c-wsp")),
						CHAR('/'),
						REPEAT(INLINE("c-wsp")),
						REF("concatenation")
					)
				)
			)
		);
	
	ruleName_ =
		DEFINE("rulename",
			GLUE(
				INLINE("ALPHA"),
				REPEAT(
					CHOICE(
						INLINE("ALPHA"),
						INLINE("DIGIT"),
						CHAR('-')
					)
				)
			)
		);
	
	definedAs_ =
		DEFINE("defined-as",
			KEYWORD("= ~")
				// deviation from RFC5234, no support for redefinition ("=/")
				// and added differentiation between matching ('~') and production rules ('=')
		);
	
	rule_ =
		DEFINE("rule",
			GLUE(
				REF("rulename"),
				REPEAT(INLINE("c-wsp")),
				REF("defined-as"),
				REPEAT(INLINE("c-wsp")),
				REF("alternation"),
					// correction of RFC5234, redundant rule "elements" substituted
				REPEAT(INLINE("c-wsp")),
				INLINE("c-nl")
			)
		);
	
	rulelist_ =
		DEFINE("rulelist",
			REPEAT(1,
				GLUE(
					NOT(EOI()),
					CHOICE(
						REF("rule"),
						GLUE(
							REPEAT(INLINE("c-wsp")),
							INLINE("c-nl")
						),
						ERROR()
					)
				)
			)
		);
	
	ENTRY("rulelist");
	LINK();
}

AbnfCompiler::NODE AbnfCompiler::defineValue(const char* digitRule)
{
	return
		GLUE(
			REPEAT(1, INLINE(digitRule)),
			REPEAT(0, 1,
				CHOICE(
					REPEAT(1,
						GLUE(
							CHAR('.'),
							REPEAT(1, INLINE(digitRule))
						)
					),
					GLUE(
						CHAR('-'),
						REPEAT(1, INLINE(digitRule))
					)
				)
			)
		);
}

Ref<AbnfCompiler::Definition, Owner> AbnfCompiler::compile(Ref<ByteArray> text, Ref<Debugger> debugger)
{
	Ref<Token, Owner> ruleList = match(text);
	check(ruleList);
	
	Ref<Definition, Owner> definition = new Definition(debugger);
	definition->OPTION("caseSensitive", false);
	
	compileRuleList(text, ruleList, definition);
	compileEntry(text, ruleList, definition);
	
	definition->LINK();
	
	return definition;
}

Ref<AbnfCompiler::Node> AbnfCompiler::ignoreDebug(Ref<Node> node) {
	Ref<Debugger::DebugNode> debugNode = node;
	return (debugNode) ? debugNode->entry() : node;
}

void AbnfCompiler::compileRuleList(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition)
{
	check(ruleList->rule() == rulelist_);
	
	Ref<Token> rule = ruleList->firstChild();
	while (rule) {
		check(rule->rule() == rule_);
		
		Ref<Token> ruleName = rule->firstChild();
		Ref<Token> definedAs = ruleName->nextSibling();
		Ref<Token> alternation = definedAs->nextSibling();
		
		check(ruleName->rule() == ruleName_);
		check(definedAs->rule() == definedAs_);
		check(alternation->rule() == alternation_);
		
		if (text->at(definedAs->i0()) == '=')
			definition->DEFINE(str(text, ruleName), compileAlternation(text, alternation, definition));
		else if (text->at(definedAs->i0()) == '~')
			definition->DEFINE_VOID(str(text, ruleName), compileAlternation(text, alternation, definition));
		else
			check(false);
		
		rule = rule->nextSibling();
	}
}

void AbnfCompiler::compileEntry(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition)
{
	Ref<Token> rule = ruleList->firstChild();
	check(rule);
	check(rule->rule() == rule_);
	Ref<Token> ruleName = rule->firstChild();
	check(ruleName->rule() == ruleName_);
	
	definition->ENTRY(str(text, ruleName));
}

AbnfCompiler::NODE AbnfCompiler::compileAlternation(Ref<ByteArray> text, Ref<Token> alternation, Ref<Definition> definition)
{
	check(alternation->rule() == alternation_);
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
	check(concatenation->rule() == concatenation_);
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
	if (token->rule() == repeat_) {
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
	else if (token->rule() == element_) {
		node = compileElement(text, token, definition);
	}
	else if (token->rule() == option_) {
		node = compileOption(text, token, definition);
	}
	else {
		check(false);
	}
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileOption(Ref<ByteArray> text, Ref<Token> option, Ref<Definition> definition)
{
	check(option->rule() == option_);
	Ref<Token> alternation = option->firstChild();
	check(alternation);
	return definition->REPEAT(0, 1, compileAlternation(text, alternation, definition));
}

AbnfCompiler::NODE AbnfCompiler::compileElement(Ref<ByteArray> text, Ref<Token> element, Ref<Definition> definition)
{
	NODE node = 0;
	check(element->rule() == element_);
	Ref<Token> token = element->firstChild();
	if (token->rule() == ruleName_) {
		node = definition->REF(str(text, token));
	}
	else if (token->rule() == group_) {
		node = compileAlternation(text, token->firstChild(), definition);
	}
	else if (token->rule() == numVal_) {
		node = compileNumVal(text, token, definition);
	}
	else if (token->rule() == charVal_) {
		node = compileCharVal(text, token, definition);
	}
	else if (token->rule() == proseVal_) {
		node = compileProseVal(text, token, definition);
	}
	else {
		check(false);
	}
	return node;
}

AbnfCompiler::NODE AbnfCompiler::compileNumVal(Ref<ByteArray> text, Ref<Token> numVal, Ref<Definition> definition)
{
	NODE node = 0;
	
	check(numVal->rule() == numVal_);
	check(text->at(numVal->i0()) == '%');
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
			check(j == s.size() - 1);
			s.set(j, strToInt(*text, i0, i, base));
			node = (s.size() > 1) ? definition->STRING(s.constData()) : definition->CHAR(s.at(0));
		}
		else {
			check(false);
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
	
	if (ignoreDebug(node)->firstChild() == ignoreDebug(node)->lastChild())
		optimizedChoice = ignoreDebug(node)->firstChild();
	
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
