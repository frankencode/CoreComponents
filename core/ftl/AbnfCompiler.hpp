 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_ABNFCOMPILER_HPP
#define FTL_ABNFCOMPILER_HPP

#include "AbnfSyntax.hpp"
#include "SyntaxDebugger.hpp"
#include "StringTrap.hpp"

namespace ftl
{

class AbnfCompiler: public AbnfSyntax
{
public:
	typedef SyntaxDefinition Definition;

	inline static Ref<AbnfCompiler, Owner> create() { return new AbnfCompiler; }
	Ref<Definition, Owner> compile(ByteArray *text, SyntaxDebugger *debugger = 0);

private:
	inline const char *str(ByteArray *text, Token *token) { return trap_->capture(text, token->i0(), token->i1()); }
	Ref<StringTrap, Owner> trap_;

	AbnfCompiler();
	Node *ignoreDebug(Node *node);

	void compileRuleList(ByteArray *text, Token *ruleList, Definition *definition);
	void compileEntry(ByteArray *text, Token *ruleList, Definition *definition);
	NODE compileAlternation(ByteArray *text, Token *alternation, Definition *definition);
	NODE compileConcatenation(ByteArray *text, Token *concatenation, Definition *definition);
	NODE compileRepetition(ByteArray *text, Token *repetition, Definition *definition);
	NODE compileOption(ByteArray *text, Token *option, Definition *definition);
	NODE compileElement(ByteArray *text, Token *element, Definition *definition);
	NODE compileNumVal(ByteArray *text, Token *numVal, Definition *definition);
	NODE compileCharVal(ByteArray *text, Token *charVal, Definition *definition);
	NODE compileProseVal(ByteArray *text, Token *proseVal, Definition *definition);

	NODE optimizeChoice(Node *choice, Definition *definition);
	void deepOptimizeChoice(Node *node, Definition *definition);
	void deepOptimizeChoice(Node *node, Node *fin, int numChars, Definition *definition);
};

} // namespace ftl

#endif // FTL_ABNFCOMPILER_HPP
