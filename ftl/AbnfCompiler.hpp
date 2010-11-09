/*
 * AbnfCompiler.hpp -- ABNF compiler according to RFC5234 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_ABNFCOMPILER_HPP
#define FTL_ABNFCOMPILER_HPP

#include "Syntax.hpp"
#include "Array.hpp"
#include "StringTrap.hpp"

namespace ftl
{

class AbnfCompiler: public Syntax<ByteArray>::Definition
{
public:
	AbnfCompiler();
	
	Ref<Definition, Owner> compile(Ref<ByteArray> text, bool printDefinition = false);
	
private:
	NODE defineValue(const char* digitRule);
	
	inline const char* str(Ref<ByteArray> text, Ref<Token> token) { return trap_.capture(text, token->i0(), token->i1()); }
	StringTrap trap_;
	
	void compileRuleList(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition);
	void compileEntry(Ref<ByteArray> text, Ref<Token> ruleList, Ref<Definition> definition);
	NODE compileAlternation(Ref<ByteArray> text, Ref<Token> alternation, Ref<Definition> definition);
	NODE compileAlternationCascade(Ref<ByteArray> text, Ref<Token> concatenation, Ref<Definition> definition);
	NODE compileConcatenation(Ref<ByteArray> text, Ref<Token> concatenation, Ref<Definition> definition);
	NODE compileConcatenationCascade(Ref<ByteArray> text, Ref<Token> repetition, Ref<Definition> definition);
	NODE compileRepetition(Ref<ByteArray> text, Ref<Token> repetition, Ref<Definition> definition);
	NODE compileOption(Ref<ByteArray> text, Ref<Token> option, Ref<Definition> definition);
	NODE compileElement(Ref<ByteArray> text, Ref<Token> element, Ref<Definition> definition);
	NODE compileNumVal(Ref<ByteArray> text, Ref<Token> numVal, Ref<Definition> definition);
	NODE compileCharVal(Ref<ByteArray> text, Ref<Token> charVal, Ref<Definition> definition);
	NODE compileProseVal(Ref<ByteArray> text, Ref<Token> proseVal, Ref<Definition> definition);
	
	NODE optimizedCHOICE(Ref<Definition> definition, NODE choice0, NODE choice1);
	
	int numVal_;
	int charVal_;
	int proseVal_;
	int group_;
	int option_;
	int element_;
	int repeat_;
	int repetition_;
	int concatenation_;
	int alternation_;
	int ruleName_;
	int definedAs_;
	int rule_;
	int rulelist_;
};

} // namespace ftl

#endif // FTL_ABNFCOMPILER_HPP
