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
#include "Format.hpp"
#include "Pattern.hpp"

namespace ftl
{

PatternException::PatternException(const String& error, int pos)
	: error_(error),
	  pos_(pos)
{
	message_ = Format("%%: %%") << pos << error;
}

PatternException::~PatternException() throw()
{}

const char* PatternException::what() const throw()
{
	return message_;
}

class PatternCompiler: public Syntax<ByteArray>::Definition, public Singleton<PatternCompiler>
{
private:
	friend class Singleton<PatternCompiler>;
	friend class Pattern;

	PatternCompiler();
	void compile(Ref<ByteArray> text, Ref<Pattern> pattern);
	NODE compileSequence(Ref<ByteArray> text, Ref<Token> token, Ref<Pattern> pattern);

	int gap_;
	int any_;
	int boi_;
	int eoi_;
	int char_;
	int seq_;
};

PatternCompiler::PatternCompiler()
{
	gap_ = DEFINE("Gap", CHAR('*'));
	any_ = DEFINE("Any", CHAR('#'));
	boi_ = DEFINE("Boi", CHAR('^'));
	eoi_ = DEFINE("Eoi", CHAR('$'));

	char_ =
		DEFINE("Char",
			CHOICE(
				EXCEPT("#*\\^$"), // {}[]|()
				GLUE(
					CHAR('\\'),
					HINT("Illegal escape sequence"),
					CHOICE(
						RANGE(
							"#*\\^$" // {}[]|()
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

	seq_ =
		DEFINE("Sequence",
			REPEAT(
				CHOICE(
					REF("Char"),
					REF("Gap"),
					REF("Any"),
					REF("Boi"),
					REF("Eoi")
				)
			)
		);

	ENTRY("Sequence");
	LINK();
}

void PatternCompiler::compile(Ref<ByteArray> text, Ref<Pattern> pattern)
{
	Ref<State, Owner> state = newState();
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> token = match(text, i0, &i1, state);
	if (!token) {
		String reason = "Syntax error";
		int pos = 1;
		if (state->hint()) {
			reason = state->hint();
			pos = state->hintOffset();
		}
		throw PatternException(reason, pos);
	}
	NODE entry = compileSequence(text, token, pattern);
	pattern->DEFINE("Expression", entry);
	pattern->ENTRY("Expression");
	pattern->LINK();
}

PatternCompiler::NODE PatternCompiler::compileSequence(Ref<ByteArray> text, Ref<Token> token, Ref<Pattern> pattern)
{
	NODE node = new GlueNode;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		if (child->rule() == char_) node->appendChild(pattern->CHAR(text->at(child->i0())));
		else if (child->rule() == gap_) node->appendChild(pattern->GREEDY_REPEAT(pattern->ANY()));
		else if (child->rule() == any_) node->appendChild(pattern->ANY());
		else if (child->rule() == boi_) node->appendChild(pattern->BOI());
		else if (child->rule() == eoi_) node->appendChild(pattern->EOI());
	}
	return pattern->debug(node, "Glue");
}

Pattern::Pattern(const char* text)
#ifndef NDEBUG
	: Syntax<ByteArray>::Definition(new Debugger)
#endif
{
	PatternCompiler::instance()->compile(String(text), this);
}

Pattern::Pattern(const String& text)
#ifndef NDEBUG
	: Syntax<ByteArray>::Definition(new Debugger)
#endif
{
	PatternCompiler::instance()->compile(text, this);
}

} // namespace ftl
