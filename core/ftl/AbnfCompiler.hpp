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
	Ref<Definition, Owner> compile(Ref<ByteArray> text, Ref<SyntaxDebugger> debugger = 0);

private:
	inline const char *str(Ref<ByteArray> text, Ref<Token> token) { return trap_->capture(text, token->i0(), token->i1()); }
	Ref<StringTrap, Owner> trap_;

	AbnfCompiler();
	Ref<Node> ignoreDebug(Ref<Node> node);

	void compileRuleList(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition);
	void compileEntry(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition);
	NODE compileAlternation(Ref<ByteArray> text, Ref<Token> alternation, Ref<Definition> definition);
	NODE compileConcatenation(Ref<ByteArray> text, Ref<Token> concatenation, Ref<Definition> definition);
	NODE compileRepetition(Ref<ByteArray> text, Ref<Token> repetition, Ref<Definition> definition);
	NODE compileOption(Ref<ByteArray> text, Ref<Token> option, Ref<Definition> definition);
	NODE compileElement(Ref<ByteArray> text, Ref<Token> element, Ref<Definition> definition);
	NODE compileNumVal(Ref<ByteArray> text, Ref<Token> numVal, Ref<Definition> definition);
	NODE compileCharVal(Ref<ByteArray> text, Ref<Token> charVal, Ref<Definition> definition);
	NODE compileProseVal(Ref<ByteArray> text, Ref<Token> proseVal, Ref<Definition> definition);

	NODE optimizeChoice(Ref<Node> choice, Ref<Definition> definition);
	void deepOptimizeChoice(Ref<Node> node, Ref<Definition> definition);
	void deepOptimizeChoice(Ref<Node> node, Ref<Node> fin, int numChars, Ref<Definition> definition);
};

} // namespace ftl

#endif // FTL_ABNFCOMPILER_HPP
