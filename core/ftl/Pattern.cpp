/*
 * Pattern.cpp -- regular expressions
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Singleton.hpp"
#ifndef NDEBUG
#include "SyntaxDebugger.hpp"
#endif
#include "syntax.hpp"
#include "Format.hpp"
#include "Pattern.hpp"

namespace ftl
{

typedef syntax::NODE NODE;

PatternException::PatternException(const String &error, int pos)
	: error_(error),
	  pos_(pos)
{
	message_ = Format("%%: %%") << pos << error;
}

PatternException::~PatternException() throw()
{}

const char *PatternException::what() const throw()
{
	return message_;
}

class PatternCompiler: public SyntaxDefinition, public Singleton<PatternCompiler>
{
protected:
	friend class Singleton<PatternCompiler>;
	friend class Pattern;

	PatternCompiler();

	void compile(Ref<ByteArray> text, Ref<SyntaxDefinition> definition);
	NODE compileChoice(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileSequence(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileAhead(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileBehind(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileCapture(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileReference(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	char readChar(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileRangeMinMax(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileRangeExplicit(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition);
	NODE compileRepeat(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition, NODE previous);

	int gap_;
	int any_;
	int boi_;
	int eoi_;
	int char_;
	int rangeMinMax_;
	int rangeExplicit_;
	int minRepeat_;
	int maxRepeat_;
	int repeat_;
	int sequence_;
	int ahead_;
	int behind_;
	int identifier_;
	int capture_;
	int reference_;
	int group_;
	int choice_;
};

PatternCompiler::PatternCompiler()
{
	any_ = DEFINE("Any", CHAR('#'));
	gap_ = DEFINE("Gap", CHAR('*'));
	boi_ = DEFINE("Boi", CHAR('^'));
	eoi_ = DEFINE("Eoi", CHAR('$'));

	char_ =
		DEFINE("Char",
			CHOICE(
				EXCEPT("#*\\[](){}|^$"),
				GLUE(
					CHAR('\\'),
					HINT("Illegal escape sequence"),
					CHOICE(
						RANGE(
							"#*\\[](){}|^$"
							"fnrt"
							"\"/"
						),
						GLUE(
							CHAR('x'),
							REPEAT(2, 2,
								CHOICE(
									RANGE('0', '9'),
									RANGE('a', 'f'),
									RANGE('A', 'F')
								)
							)
						)
					),
					DONE()
				)
			)
		);

	rangeMinMax_ =
		DEFINE("RangeMinMax",
			GLUE(
				CHAR('['),
				CHOICE(
					GLUE(
						REPEAT(0, 1, CHAR('^')),
						CHOICE(
							GLUE(
								STRING(".."),
								REF("Char")
							),
							GLUE(
								REF("Char"),
								STRING(".."),
								REF("Char")
							),
							GLUE(
								REF("Char"),
								STRING("..")
							)
						)
					),
					STRING("..")
				),
				CHAR(']')
			)
		);

	rangeExplicit_ =
		DEFINE("RangeExplicit",
			GLUE(
				CHAR('['),
				REPEAT(0, 1, CHAR('^')),
				REPEAT(1, REF("Char")),
				CHAR(']')
			)
		);

	DEFINE_VOID("Number",
		REPEAT(1, 20,
			RANGE('0', '9')
		)
	);

	minRepeat_ = DEFINE("MinRepeat", INLINE("Number"));
	maxRepeat_ = DEFINE("MaxRepeat", INLINE("Number"));

	repeat_ =
		DEFINE("Repeat",
			GLUE(
				CHOICE(
					PREVIOUS("Char"),
					PREVIOUS("Any"),
					PREVIOUS("RangeMinMax"),
					PREVIOUS("RangeExplicit"),
					PREVIOUS("Group")
				),
				CHAR('{'),
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 1, REF("MinRepeat")),
						CHAR(','),
						REPEAT(0, 1, REF("MaxRepeat"))
					)
				),
				REPEAT(0, 1, RANGE("~?")),
				CHAR('}')
			)
		);

	sequence_ =
		DEFINE("Sequence",
			REPEAT(
				CHOICE(
					REF("Repeat"),
					REF("Char"),
					REF("Any"),
					REF("Gap"),
					REF("RangeMinMax"),
					REF("RangeExplicit"),
					REF("Boi"),
					REF("Eoi"),
					REF("Ahead"),
					REF("Behind"),
					REF("Capture"),
					REF("Reference"),
					REF("Group")
				)
			)
		);

	ahead_ =
		DEFINE("Ahead",
			GLUE(
				STRING("(?"),
				RANGE("=!"),
				REF("Choice"),
				CHAR(')')
			)
		);

	behind_ =
		DEFINE("Behind",
			GLUE(
				STRING("(?<"),
				RANGE("=!"),
				REF("Choice"),
				CHAR(')')
			)
		);

	identifier_ =
		DEFINE("Identifier",
			REPEAT(1,
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					CHAR('_')
				)
			)
		);

	capture_ =
		DEFINE("Capture",
			GLUE(
				STRING("(?"),
				REF("Identifier"),
				CHAR(':'),
				REF("Choice"),
				CHAR(')')
			)
		);

	reference_ =
		DEFINE("Reference",
			GLUE(
				STRING("(?&"),
				REF("Identifier"),
				CHAR(')')
			)
		);

	group_ =
		DEFINE("Group",
			GLUE(
				CHAR('('),
				NOT(CHAR('?')),
				REF("Choice"),
				CHAR(')')
			)
		);

	choice_ =
		DEFINE("Choice",
			GLUE(
				REF("Sequence"),
				REPEAT(
					GLUE(
						CHAR('|'),
						REF("Sequence")
					)
				)
			)
		);

	ENTRY("Choice");
	LINK();
}

void PatternCompiler::compile(Ref<ByteArray> text, Ref<SyntaxDefinition> definition)
{
	Ref<SyntaxState, Owner> state = newState();
	Ref<Token, Owner> token = match(text, 0, state);
	if ((!token) || (token->length() < text->length())) {
		String reason = "Syntax error";
		int pos = token->length();
		if (state->hint()) {
			reason = state->hint();
			pos = state->hintOffset();
		}
		throw PatternException(reason, pos);
	}
	NODE entry;
	if (text->length() == 0) entry = definition->PASS();
	else entry = compileChoice(text, token, definition);
	definition->DEFINE("Expression", entry);
	definition->ENTRY("Expression");
	definition->LINK();
}

NODE PatternCompiler::compileChoice(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	if (token->countChildren() == 1)
		return compileSequence(text, token->firstChild(), definition);
	NODE node = new syntax::ChoiceNode;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling())
		node->appendChild(compileSequence(text, child, definition));
	return definition->debug(node, "Choice");
}

NODE PatternCompiler::compileSequence(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	NODE node = new syntax::GlueNode;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		if (child->rule() == char_) node->appendChild(definition->CHAR(readChar(text, child, definition)));
		else if (child->rule() == any_) node->appendChild(definition->ANY());
		else if (child->rule() == gap_) node->appendChild(definition->GREEDY_REPEAT(definition->ANY()));
		else if (child->rule() == rangeMinMax_) node->appendChild(compileRangeMinMax(text, child, definition));
		else if (child->rule() == rangeExplicit_) node->appendChild(compileRangeExplicit(text, child, definition));
		else if (child->rule() == repeat_) node->appendChild(compileRepeat(text, child, definition, node->lastChild()));
		else if (child->rule() == boi_) node->appendChild(definition->BOI());
		else if (child->rule() == eoi_) node->appendChild(definition->EOI());
		else if (child->rule() == group_) node->appendChild(compileChoice(text, child->firstChild(), definition));
		else if (child->rule() == ahead_) node->appendChild(compileAhead(text, child, definition));
		else if (child->rule() == behind_) node->appendChild(compileBehind(text, child, definition));
		else if (child->rule() == capture_) node->appendChild(compileCapture(text, child, definition));
		else if (child->rule() == reference_) node->appendChild(compileReference(text, child, definition));
	}
	if (node->firstChild() == node->lastChild()) {
		NODE child = node->firstChild();
		child->unlink();
		return child;
	}
	return definition->debug(node, "Glue");
}

NODE PatternCompiler::compileAhead(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	return
		(text->at(token->i0() + 2) == '=') ?
			definition->AHEAD(compileChoice(text, token->firstChild(), definition)) :
			definition->NOT(compileChoice(text, token->firstChild(), definition));
}

NODE PatternCompiler::compileBehind(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	return
		(text->at(token->i0() + 3) == '=') ?
			definition->BEHIND(compileChoice(text, token->firstChild(), definition)) :
			definition->NOT_BEHIND(compileChoice(text, token->firstChild(), definition));
}

NODE PatternCompiler::compileCapture(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	String name = text->copy(token->firstChild());
	return definition->CAPTURE(name, compileChoice(text, token->lastChild(), definition));
}

NODE PatternCompiler::compileReference(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	String name = text->copy(token->firstChild());
	return definition->REPLAY(name);
}

char PatternCompiler::readChar(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	return (token->i1() - token->i0() > 1) ?
		text->copy(token)->expandInsitu()->at(0) :
		text->at(token->i0());
}

NODE PatternCompiler::compileRangeMinMax(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	int n = token->countChildren();
	bool invert = (text->at(token->i0() + 1) == '^');
	if (n == 2) {
		Ref<Token> min = token->firstChild();
		Ref<Token> max = min->nextSibling();
		char a = readChar(text, min, definition);
		char b = readChar(text, max, definition);
		return  invert ? definition->EXCEPT(a, b) : definition->RANGE(a, b);
	}
	else if (n == 1) {
		Ref<Token> child = token->firstChild();
		char ch = readChar(text, child, definition);
		return invert ?
			( (child->i0() - token->i0() <= 2) ? definition->BELOW(ch)            : definition->GREATER(ch)        ) :
			( (child->i0() - token->i0() <= 2) ? definition->GREATER_OR_EQUAL(ch) : definition->BELOW_OR_EQUAL(ch) );
	}
	return definition->ANY();
}

NODE PatternCompiler::compileRangeExplicit(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition)
{
	Ref<Token> child = token->firstChild();
	bool invert = (text->at(token->i0() + 1) == '^');
	int n = token->countChildren();
	String s(n);
	for (int i = 0; i < n; ++i) {
		s->set(i, readChar(text, child, definition));
		child = child->nextSibling();
	}
	return invert ? definition->EXCEPT(s->data()) : definition->RANGE(s->data());
}

NODE PatternCompiler::compileRepeat(Ref<ByteArray> text, Ref<Token> token, Ref<SyntaxDefinition> definition, NODE previous)
{
	Ref<Token> child = token->firstChild(), min, max;
	while (child) {
		if (child->rule() == minRepeat_) min = child;
		else if (child->rule() == maxRepeat_) max = child;
		child = child->nextSibling();
	}
	int minRepeat = min ? text->copy(min)->toInt() : 0;
	int maxRepeat = max ? text->copy(max)->toInt() : intMax;
	int modifier = text->at(token->i1() - 2);
	previous->unlink();
	if (modifier == '?')
		return definition->LAZY_REPEAT(minRepeat, previous);
	else if (modifier == '~')
		return definition->REPEAT(minRepeat, maxRepeat, previous);
	return definition->GREEDY_REPEAT(minRepeat, maxRepeat, previous);
}

Pattern::Pattern()
{}

Pattern::Pattern(const char *text)
{
	*this = String(text);
}

Pattern::Pattern(const String &text)
{
	*this = text;
}

Pattern::Pattern(const Variant &text)
{
	*this = text.toString();
}

const Pattern &Pattern::operator=(const char *text)
{
	return *this = String(text);
}

const Pattern &Pattern::operator=(const String &text)
{
	text_ = text;
	set(
		SyntaxDefinition::create(
#ifndef NDEBUG
			SyntaxDebugger::create()
#endif
		)
	);
	PatternCompiler::instance()->compile(text_, *this);
	return *this;
}

const Pattern &Pattern::operator=(const Variant &text)
{
	return *this = String(text);
}

} // namespace ftl
