 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "syntax.hpp"
#include "SyntaxDefinition.hpp"

namespace ftl
{

namespace syntax
{

Definition::Definition(Ref<DebugFactory> debugFactory)
	: def_(new DefinitionNode(debugFactory))
{}

Definition::~Definition()
{}

int Definition::id() const { return def_->id(); }
const char *Definition::name() const { return def_->name(); }

Ref<DebugFactory> Definition::debugFactory() const { return def_->debugFactory(); }
Node *Definition::debug(Node *newNode, const char *nodeType) { return def_->debug(newNode, nodeType); }

int Definition::keywordByName(const char *keyword) { return def_->keywordByName(keyword); }
Ref<State, Owner> Definition::newState(State *parent) const { return def_->newState(parent); }

Ref<Token, Owner> Definition::find(const ByteArray *media, int i) const
{
	return def_->find(const_cast<ByteArray*>(media), &i);
}

Ref<Token, Owner> Definition::match(const ByteArray *media, int i, Ref<SyntaxState> state) const
{
	int i0 = (i >= 0) ? i : 0, h;
	Ref<Token, Owner> token = def_->match(const_cast<ByteArray*>(media), i0, &h, state);
	if ((i < 0) && (token)) {
		if (media->has(token->i1())) token = 0;
	}
	return token;
}

Ref<Token, Owner> Definition::match(const ByteArray *media, Ref<SyntaxState> state) const
{
	return match(media, -1, state);
}

int Definition::matchLength() const
{
	return def_->matchLength();
}

void Definition::SYNTAX(const char *name) { def_->SYNTAX(name); }
void Definition::IMPORT(Ref<Definition> definition, const char *name) { def_->IMPORT(definition->def_, name); }
void Definition::OPTION(const char *name, bool value) { def_->OPTION(name, value); }

NODE Definition::CHAR(char ch) { return def_->CHAR(ch); }
NODE Definition::OTHER(char ch) { return def_->OTHER(ch); }
NODE Definition::GREATER(char ch) { return def_->GREATER(ch); }
NODE Definition::BELOW(char ch) { return def_->BELOW(ch); }
NODE Definition::GREATER_OR_EQUAL(char ch) { return def_->GREATER_OR_EQUAL(ch); }
NODE Definition::BELOW_OR_EQUAL(char ch) { return def_->BELOW_OR_EQUAL(ch); }
NODE Definition::ANY() { return def_->ANY(); }

NODE Definition::RANGE(char a, char b) { return def_->RANGE(a, b); }
NODE Definition::RANGE(const char *s) { return def_->RANGE(s); }
NODE Definition::EXCEPT(char a, char b) { return def_->EXCEPT(a, b); }
NODE Definition::EXCEPT(const char *s) { return def_->EXCEPT(s); }

NODE Definition::STRING(const char *s) { return def_->STRING(s); }
NODE Definition::KEYWORD(const char *keywords) { return def_->KEYWORD(keywords); }

NODE Definition::REPEAT(int minRepeat, int maxRepeat, NODE entry) { return def_->REPEAT(minRepeat, maxRepeat, entry); }
NODE Definition::REPEAT(int minRepeat, NODE entry) { return def_->REPEAT(minRepeat, entry); }
NODE Definition::REPEAT(NODE entry) { return def_->REPEAT(entry); }

NODE Definition::LAZY_REPEAT(int minRepeat, NODE entry) { return def_->LAZY_REPEAT(minRepeat, entry); }
NODE Definition::LAZY_REPEAT(NODE entry) { return def_->LAZY_REPEAT(entry); }

NODE Definition::GREEDY_REPEAT(int minRepeat, int maxRepeat, NODE entry) { return def_->GREEDY_REPEAT(minRepeat, maxRepeat, entry); }
NODE Definition::GREEDY_REPEAT(int minRepeat, NODE entry) { return def_->GREEDY_REPEAT(minRepeat, entry); }
NODE Definition::GREEDY_REPEAT(NODE entry) { return def_->GREEDY_REPEAT(entry); }

NODE Definition::LENGTH(int minLength, int maxLength, NODE entry) { return def_->LENGTH(minLength, maxLength, entry); }
NODE Definition::LENGTH(int minLength, NODE entry) { return def_->LENGTH(minLength, entry); }
NODE Definition::BOI() { return def_->BOI(); }
NODE Definition::EOI() { return def_->EOI(); }
NODE Definition::PASS() { return def_->PASS(); }
NODE Definition::FAIL() { return def_->FAIL(); }
NODE Definition::FIND(NODE entry) { return def_->FIND(entry); }
NODE Definition::AHEAD(NODE entry) { return def_->AHEAD(entry); }
NODE Definition::NOT(NODE entry) { return def_->NOT(entry); }
NODE Definition::BEHIND(NODE entry) { return def_->BEHIND(entry); }
NODE Definition::NOT_BEHIND(NODE entry) { return def_->NOT_BEHIND(entry); }

NODE Definition::CHOICE() { return def_->CHOICE(); } // FIXME
NODE Definition::GLUE() { return def_->GLUE(); }

NODE Definition::CHOICE(NODE choice0, NODE choice1) { return def_->CHOICE(choice0, choice1); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2) { return def_->CHOICE(choice0, choice1, choice2); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3) { return def_->CHOICE(choice0, choice1, choice2, choice3); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14); }
NODE Definition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15); }

NODE Definition::GLUE(NODE child0, NODE child1) { return def_->GLUE(child0, child1); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2) { return def_->GLUE(child0, child1, child2); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3) { return def_->GLUE(child0, child1, child2, child3); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4) { return def_->GLUE(child0, child1, child2, child3, child4); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5) { return def_->GLUE(child0, child1, child2, child3, child4, child5); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14); }
NODE Definition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15); }

NODE Definition::HINT(const char *text)  { return def_->HINT(text); }
NODE Definition::DONE() { return def_->DONE(); }

int Definition::DEFINE(const char *ruleName, NODE entry) { return def_->DEFINE(ruleName, entry); }
void Definition::DEFINE_VOID(const char *ruleName, NODE entry) { def_->DEFINE_VOID(ruleName, entry); }
void Definition::ENTRY(const char *ruleName) { def_->ENTRY(ruleName); }
NODE Definition::REF(const char *ruleName) { return def_->REF(ruleName); }
NODE Definition::INLINE(const char *ruleName) { return def_->INLINE(ruleName); }
NODE Definition::PREVIOUS(const char *ruleName, const char *keyword) { return def_->PREVIOUS(ruleName, keyword); }
NODE Definition::CONTEXT(const char *ruleName, NODE entry) { return def_->CONTEXT(ruleName, entry); }

NODE Definition::CALL(CallBack callBack, Ref<Instance> self) { return def_->CALL(callBack, self); }
NODE Definition::ERROR() { return def_->ERROR(); }

void Definition::OPTIMIZE() { return def_->OPTIMIZE(); }
void Definition::LINK(bool optimize) { return def_->LINK(optimize); }

NODE Definition::SET(const char *name, bool value) { return def_->SET(name, value); }
NODE Definition::IF(const char *name, NODE trueBranch, NODE falseBranch) { return def_->IF(name, trueBranch, falseBranch); }
NODE Definition::CAPTURE(const char *name, NODE coverage) { return def_->CAPTURE(name, coverage); }
NODE Definition::REPLAY(const char *name) { return def_->REPLAY(name); }

NODE Definition::INVOKE(DefinitionNode *definition, NODE coverage) { return def_->INVOKE(definition, coverage); }
NODE Definition::INVOKE(const char *definitionName, NODE coverage) { return def_->INVOKE(definitionName, coverage); }

} // namespace syntax

} // namespace ftl
