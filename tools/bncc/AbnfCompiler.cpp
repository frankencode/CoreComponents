/*
 * AbnfCompiler.cpp -- compiles syntax definitions according to RFC5434 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "AbnfCompiler.hpp"

namespace bncc
{

AbnfCompiler::AbnfCompiler(Ref<Scope> scope, const char* name)
	: Syntax<StringMedia>::Definition(scope, name),
	  depthSaved_(-1)
{
	OPTION("caseSensitive", false);
	
	DEFINE_VOID("ALPHA",
		CHOICE(
			RANGE('a', 'z'), // 0x61 - 0x7A
			RANGE('A', 'Z')  // 0x41 - 0x5A
		)
	);
	
	DEFINE_VOID("DIGIT",
		RANGE('0', '9') // 0x30 - 0x39
	);
	
	DEFINE_VOID("HEXDIG",
		CHOICE(
			INLINE("DIGIT"),
			CHOICE(
				RANGE('a', 'f'), // 0x61 - 0x66
				RANGE('A', 'F')  // 0x41 - 0x46
			)
		)
	);
	
	DEFINE_VOID("BIT", RANGE("01"));  // 0x30 or 0x31
	DEFINE_VOID("WSP", RANGE(" \t")); // 0x20 or 0x09, whitespace
	
	DEFINE_VOID("CHAR",  RANGE(0x01, 0x7F)); // any 7-bit US-ASCII character
	DEFINE_VOID("VCHAR", RANGE(0x21, 0x7E)); // visible (printing) characters
	
	DEFINE_VOID("CR",     CHAR('\r')); // 0x0D
	DEFINE_VOID("LF",     CHAR('\n')); // 0x0A
	DEFINE_VOID("HTAB",   CHAR('\t')); // 0x09
	DEFINE_VOID("SP",     CHAR(' '));  // 0x20
	DEFINE_VOID("DQUOTE", CHAR('"'));  // 0x22
	
	DEFINE_VOID("CRLF",
		GLUE(
			REPEAT(0, 1, INLINE("CR")), // minor deviation from RFC5234
			INLINE("LF")
		)
	);
	
	DEFINE_VOID("CTL",
		CHOICE(
			RANGE(0x00, 0x1F),
			CHAR(0x7F)
		)
	);
	
	DEFINE_VOID("LWSP",
		REPEAT(
			CHOICE(
				INLINE("WSP"),
				GLUE(
					INLINE("CRLF"),
					INLINE("WSP")
				)
			)
		)
	);
	
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
			KEYWORD("= =/")
		);
	
	rule_ =
		DEFINE("rule",
			GLUE(
				REF("rulename"),
				REPEAT(INLINE("c-wsp")),
				REF("defined-as"),
				REPEAT(INLINE("c-wsp")),
				REF("alternation"),         // correction of RFC5234, redundant rule "elements" substituted
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

void AbnfCompiler::compile(Ref<StringMedia> text)
{
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> rootToken = 0;
	if (!match(text, i0, &i1, &rootToken))
		check(false);
	print("OPTION(\"caseSensitive\", false);\n\n");
	compileRuleList(text, rootToken);
}

void AbnfCompiler::compileRuleList(Ref<StringMedia> text, Ref<Token> ruleList)
{
	check(ruleList->type() == rulelist_);
	Ref<Token> rule = ruleList->firstChild();
	while (rule) {
		check(rule->type() == rule_);
		
		Ref<Token> ruleName = rule->firstChild();
		Ref<Token> definedAs = ruleName->nextSibling();
		Ref<Token> alternation = definedAs->nextSibling();
		
		check(ruleName->type() == ruleName_);
		check(definedAs->rule() == definedAs_);
		check(alternation->type() == alternation_);
		check(definedAs->length() == 1); // HACK, disallow "=/" for starters (would require REDEFINE)
		
		printOpening("DEFINE", 0/*depth*/, Format("\"%%\"") << String(text, ruleName->index(), ruleName->length()));
		compileAlternation(text, alternation, 1);
		printClosing(rule, 0/*depth*/);
		print(";\n");
		depthSaved_ = -1;
		
		rule = rule->nextSibling();
		if (rule) print("\n");
	}
	
	{
		Ref<Token> rule = ruleList->firstChild();
		check(rule);
		check(rule->type() == rule_);
		Ref<Token> ruleName = rule->firstChild();
		check(ruleName->type() == ruleName_);
		
		print("\nENTRY(\"%%\");\n", String(text, ruleName->index(), ruleName->length()));
	}
}

void AbnfCompiler::compileAlternation(Ref<StringMedia> text, Ref<Token> alternation, int depth)
{
	Ref<Token> concatenation = alternation->firstChild();
	check(concatenation);
	if (concatenation == alternation->lastChild()) {
		compileConcatenation(text, concatenation, depth);
		return;
	}
	
	printOpening("CHOICE", depth);
	
	while (concatenation) {
		compileConcatenation(text, concatenation, depth + 1);
		concatenation = concatenation->nextSibling();
	}
	
	printClosing(alternation, depth);
}

void AbnfCompiler::compileConcatenation(Ref<StringMedia> text, Ref<Token> concatenation, int depth)
{
	Ref<Token> repetition = concatenation->firstChild();
	check(repetition);
	if (repetition == concatenation->lastChild()) {
		compileRepetition(text, repetition, depth);
		return;
	}
	
	printOpening("GLUE", depth);
	
	while (repetition) {
		compileRepetition(text, repetition, depth + 1);
		repetition = repetition->nextSibling();
	}
	
	printClosing(concatenation, depth);
}

void AbnfCompiler::compileRepetition(Ref<StringMedia> text, Ref<Token> repetition, int depth)
{
	Ref<Token> token = repetition->firstChild();
	
	if (token->type() == repeat_) {
		String s(text, token->index(), token->length());
		Ref<StringList, Owner> parts = s.split("*");
		Format arguments;
		if (parts->length() == 2) {
			if (parts->at(0) != "")
				arguments << String(Format("%%") << parts->at(0).toInt());
			if (parts->at(1) != "") {
				if (parts->at(0) == "")
					arguments << "0";
				arguments << String(Format(", %%") << parts->at(1).toInt());
			}
		}
		else if (parts->length() == 1) {
			int n = parts->at(0).toInt();
			arguments << String(Format("%%, %%") << n << n);
		}
		printOpening("REPEAT", depth, arguments);
		
		compileElement(text, token->nextSibling(), depth + 1);
		
		printClosing(repetition, depth);
	}
	else if (token->type() == element_) {
		compileElement(text, token, depth);
	}
	else if (token->type() == option_) {
		compileOption(text, token, depth);
	}
	else {
		check(false);
	}
}

void AbnfCompiler::compileOption(Ref<StringMedia> text, Ref<Token> option, int depth)
{
	check(option->type() == option_);
	Ref<Token> alternation = option->firstChild();
	check(alternation);
	printOpening("REPEAT", depth, "0, 1");
	compileAlternation(text, alternation, depth + 1);
	printClosing(option, depth);
}

void AbnfCompiler::compileElement(Ref<StringMedia> text, Ref<Token> element, int depth)
{
	check(element->type() == element_);
	Ref<Token> token = element->firstChild();
	if (token->type() == ruleName_) {
		printLeaf("REF", depth, Format("\"%%\"") << String(text, token->index(), token->length()));
	}
	else if (token->type() == group_) {
		compileAlternation(text, token->firstChild(), depth);
	}
	else if (token->type() == numVal_) {
		compileNumVal(text, token, depth);
	}
	else if (token->type() == charVal_) {
		compileCharVal(text, token, depth);
	}
	else if (token->type() == proseVal_) {
		compileProseVal(text, token, depth);
	}
	else {
		check(false);
	}
}

void AbnfCompiler::compileNumVal(Ref<StringMedia> text, Ref<Token> numVal, int depth)
{
	check(numVal->type() == numVal_);
	String prefix;
	char base = toLower(text->at(numVal->index() + 1));
	if (base == 'd');
	else if (base == 'x')
		prefix = "0x";
	else if (base == 'b')
		prefix = "b";
	String s(text, numVal->index()+2, numVal->length()-2);
	if (s->contains('-')) {
		Ref<StringList, Owner> parts = s.split("-");
		// print("parts->at(0) = \"%%\"\n", parts->at(0));
		// print("parts->at(1) = \"%%\"\n", parts->at(1));
		char a = String(Format() << prefix << parts->at(0)).toInt(); // HACK, breaking down to ASCII
		char b = String(Format() << prefix << parts->at(1)).toInt();
		printLeaf("RANGE", depth, Format("'%%', '%%'") << a << b);
	}
	else if (s->contains('.')) {
		Ref<StringList, Owner> parts = s.split(".");
		String s2(parts->length());
		int j = 0;
		for (StringList::Iterator i = parts->iterator(); i.hasNext();) {
			char ch = String(Format() << prefix << i.next()).toInt(); // HACK, breaking down to ASCII
			s2->set(j++, ch);
		}
		printLeaf("STRING", depth, Format("\"%%\"") << s2);
	}
	else {
		char ch = String(Format() << prefix << s).toInt();
		printLeaf("CHAR", depth, Format("'%%'") << ch);
	}
}

void AbnfCompiler::compileCharVal(Ref<StringMedia> text, Ref<Token> charVal, int depth)
{
	check(charVal->type() == charVal_);
	String s(text, charVal->index() + 1, charVal->length() - 2);
	if (s->size() == 1) {
		char ch = s->at(0);
		bool isAlpha = ( (('a' <= ch) && (ch <= 'z')) ||
		                 (('A' <= ch) && (ch <= 'Z')) );
		if (!isAlpha) {
			printLeaf("CHAR", depth,  Format("'%%'") << ch);
			return;
		}
	}
	printLeaf("STRING", depth, Format("\"%%\"") << s);
}

void AbnfCompiler::compileProseVal(Ref<StringMedia> text, Ref<Token> proseVal, int depth)
{
	check(proseVal->type() == proseVal_);
	String s(text, proseVal->index() + 1, proseVal->length() - 2);
	s = s.replace("\"", "\\\"");
	printLeaf("STRING", depth, String(Format("\"%%\"") << s));
}

void AbnfCompiler::printLeaf(const char* nodeName, int depth, String arguments)
{
	printOpening(nodeName, depth, arguments, true);
}

void AbnfCompiler::printOpening(const char* nodeName, int depth, String arguments, bool closeSameLine)
{
	if (depth == depthSaved_)
		print(",\n");
	else if (depthSaved_ != -1)
		print("\n");
	depthSaved_ = depth;
	print("%%%%(%%", indent(depth), nodeName, arguments);
	if ((arguments != "") && (!closeSameLine)) print(",");
	if (closeSameLine)
		print(")");
}

void AbnfCompiler::printClosing(Ref<Token> token, int depth)
{
	depthSaved_ = depth;
	print("\n%%)", indent(depth));
}

String AbnfCompiler::indent(int depth)
{
	return String(4 * depth, ' ');
}

int AbnfCompiler::syntaxError(StringMedia* text, int index, State* state)
{
	int line = 1, pos = 1;
	getLineAndPosFromIndex(text, index, &line, &pos);
	char* lineStr = pona::intToStr(line);
	char* posStr = pona::intToStr(pos);
	char* indexStr = pona::intToStr(index);
	char* msg = pona::strcat("Syntax error in ABNF definition at ", lineStr, ":", posStr, "(", indexStr, ")");
	delete[] indexStr;
	delete[] posStr;
	delete[] lineStr;
	PONA_THROW(DebugException, msg);
	return -1;
}

} // namespace bncc
