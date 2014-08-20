/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXABNF_ABNFCOMPILER_H
#define FLUXABNF_ABNFCOMPILER_H

#include <flux/SyntaxDebugger.h>
#include "AbnfSyntax.h"
#include "StringTrap.h"

namespace flux {
namespace abnf {

class AbnfCompiler: public AbnfSyntax
{
public:
	inline static Ref<AbnfCompiler> create() { return new AbnfCompiler; }
	Ref<SyntaxDefinition> compile(ByteArray *text, SyntaxDebugger *debugger = 0);

private:
	inline const char *str(ByteArray *text, Token *token) { return trap_->capture(text, token->i0(), token->i1()); }
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

}} // namespace flux::abnf

#endif // FLUXABNF_ABNFCOMPILER_H
