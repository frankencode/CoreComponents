/*
 * AbnfCompiler.hpp -- ABNF compiler according to RFC5234 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_ABNFCOMPILER_HPP
#define FTL_ABNFCOMPILER_HPP

#include "AbnfSyntax.hpp"
#include "StringTrap.hpp"

namespace ftl
{

class AbnfCompiler: public AbnfSyntax
{
public:
	typedef Syntax<ByteArray>::Definition Definition;
	
	Ref<Definition, Owner> compile(Ref<ByteArray> text, Ref<Debugger> debugger = 0);
	
private:
	inline const char* str(Ref<ByteArray> text, Ref<Token> token) { return trap_.capture(text, token->i0(), token->i1()); }
	StringTrap trap_;
	
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
