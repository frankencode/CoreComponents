/*
 * AbnfCompiler.hpp -- compiles syntax definitions according to RFC5434 (former RFC4234, RFC2234)
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef BNCC_ABNFCOMPILER_HPP
#define BNCC_ABNFCOMPILER_HPP

#include <pona/stdio>
#include <pona/syntax>
#include "use_pona"

namespace bncc
{

class AbnfCompiler: public Syntax<StringMedia>::Definition
{
public:
	AbnfCompiler(Ref<Scope> scope = 0, const char* name = 0);
	
	void compile(Ref<StringMedia> text);
	
protected:
	virtual int syntaxError(StringMedia* text, int index, State* state);
	
private:
	void compileRuleList(Ref<StringMedia> text, Ref<Token> ruleList);
	void compileAlternation(Ref<StringMedia> text, Ref<Token> alternation, int depth);
	void compileConcatenation(Ref<StringMedia> text, Ref<Token> concatenation, int depth);
	void compileRepetition(Ref<StringMedia> text, Ref<Token> repetition, int depth);
	void compileOption(Ref<StringMedia> text, Ref<Token> option, int depth);
	void compileElement(Ref<StringMedia> text, Ref<Token> element, int depth);
	void compileNumVal(Ref<StringMedia> text, Ref<Token> numVal, int depth);
	void compileCharVal(Ref<StringMedia> text, Ref<Token> charVal, int depth);
	void compileProseVal(Ref<StringMedia> text, Ref<Token> proseVal, int depth);
	void printLeaf(const char* nodeName, int depth, String arguments = "");
	void printOpening(const char* nodeName, int depth, String arguments = "", bool closeSameLine = false);
	void printClosing(Ref<Token> token, int depth);
	String indent(int depth);
	
	NODE defineValue(const char* digitRule);
	
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
	
	int depthSaved_; // notes: prevents compile() from being reentrant
};

} // namespace bncc

#endif // BNCC_ABNFCOMPILER_HPP
