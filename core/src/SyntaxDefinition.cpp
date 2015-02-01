/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/syntax>
#include <flux/SyntaxDefinition>

namespace flux {

SyntaxDefinition::SyntaxDefinition(SyntaxDebugFactory *debugFactory)
    : def_(new syntax::DefinitionNode(debugFactory))
{}

SyntaxDefinition::~SyntaxDefinition()
{}

int SyntaxDefinition::scope(const char *name) { return syntax::DefinitionNode::scope(name); }

int SyntaxDefinition::id() const { return def_->id(); }
const char *SyntaxDefinition::name() const { return def_->name(); }

SyntaxDebugFactory *SyntaxDefinition::debugFactory() const { return def_->debugFactory(); }
SyntaxNode *SyntaxDefinition::debug(SyntaxNode *newNode, const char *nodeType) { return def_->debug(newNode, nodeType); }

int SyntaxDefinition::ruleByName(const char *name) const { return def_->ruleByName(name)->id(); }
int SyntaxDefinition::keywordByName(const char *name) const { return def_->keywordByName(name); }

Ref<SyntaxState> SyntaxDefinition::find(const ByteArray *text, int i, TokenFactory *tokenFactory) const
{
    return def_->find(const_cast<ByteArray *>(text), i, tokenFactory);
}

Ref<SyntaxState> SyntaxDefinition::match(const ByteArray *text, int i, TokenFactory *tokenFactory) const
{
    return def_->match(const_cast<ByteArray *>(text), i, tokenFactory);
}

int SyntaxDefinition::matchLength() const
{
    return def_->matchLength();
}

typedef syntax::NODE NODE;

void SyntaxDefinition::SYNTAX(const char *name) { def_->SYNTAX(name); }
void SyntaxDefinition::IMPORT(const SyntaxDefinition *definition, const char *name) { def_->IMPORT(definition->def_, name); }
void SyntaxDefinition::OPTION(const char *name, bool value) { def_->OPTION(name, value); }

NODE SyntaxDefinition::CHAR(char ch) { return def_->CHAR(ch); }
NODE SyntaxDefinition::OTHER(char ch) { return def_->OTHER(ch); }
NODE SyntaxDefinition::GREATER(char ch) { return def_->GREATER(ch); }
NODE SyntaxDefinition::BELOW(char ch) { return def_->BELOW(ch); }
NODE SyntaxDefinition::GREATER_OR_EQUAL(char ch) { return def_->GREATER_OR_EQUAL(ch); }
NODE SyntaxDefinition::BELOW_OR_EQUAL(char ch) { return def_->BELOW_OR_EQUAL(ch); }
NODE SyntaxDefinition::ANY() { return def_->ANY(); }

NODE SyntaxDefinition::RANGE(char a, char b) { return def_->RANGE(a, b); }
NODE SyntaxDefinition::RANGE(const char *s) { return def_->RANGE(s); }
NODE SyntaxDefinition::EXCEPT(char a, char b) { return def_->EXCEPT(a, b); }
NODE SyntaxDefinition::EXCEPT(const char *s) { return def_->EXCEPT(s); }

NODE SyntaxDefinition::STRING(const char *s) { return def_->STRING(s); }
NODE SyntaxDefinition::KEYWORD(const char *keywords) { return def_->KEYWORD(keywords); }

NODE SyntaxDefinition::REPEAT(int minRepeat, int maxRepeat, NODE entry) { return def_->REPEAT(minRepeat, maxRepeat, entry); }
NODE SyntaxDefinition::REPEAT(int minRepeat, NODE entry) { return def_->REPEAT(minRepeat, entry); }
NODE SyntaxDefinition::REPEAT(NODE entry) { return def_->REPEAT(entry); }

NODE SyntaxDefinition::LAZY_REPEAT(int minRepeat, NODE entry) { return def_->LAZY_REPEAT(minRepeat, entry); }
NODE SyntaxDefinition::LAZY_REPEAT(NODE entry) { return def_->LAZY_REPEAT(entry); }

NODE SyntaxDefinition::GREEDY_REPEAT(int minRepeat, int maxRepeat, NODE entry) { return def_->GREEDY_REPEAT(minRepeat, maxRepeat, entry); }
NODE SyntaxDefinition::GREEDY_REPEAT(int minRepeat, NODE entry) { return def_->GREEDY_REPEAT(minRepeat, entry); }
NODE SyntaxDefinition::GREEDY_REPEAT(NODE entry) { return def_->GREEDY_REPEAT(entry); }

NODE SyntaxDefinition::FILTER(NODE filter, char blank, NODE entry) { return def_->FILTER(filter, blank, entry); }

NODE SyntaxDefinition::LENGTH(int minLength, int maxLength, NODE entry) { return def_->LENGTH(minLength, maxLength, entry); }
NODE SyntaxDefinition::LENGTH(int minLength, NODE entry) { return def_->LENGTH(minLength, entry); }
NODE SyntaxDefinition::BOI() { return def_->BOI(); }
NODE SyntaxDefinition::EOI() { return def_->EOI(); }
NODE SyntaxDefinition::PASS() { return def_->PASS(); }
NODE SyntaxDefinition::FAIL() { return def_->FAIL(); }
NODE SyntaxDefinition::FIND(NODE entry) { return def_->FIND(entry); }
NODE SyntaxDefinition::AHEAD(NODE entry) { return def_->AHEAD(entry); }
NODE SyntaxDefinition::NOT(NODE entry) { return def_->NOT(entry); }
NODE SyntaxDefinition::BEHIND(NODE entry) { return def_->BEHIND(entry); }
NODE SyntaxDefinition::NOT_BEHIND(NODE entry) { return def_->NOT_BEHIND(entry); }

NODE SyntaxDefinition::CHOICE() { return def_->CHOICE(); } // FIXME
NODE SyntaxDefinition::GLUE() { return def_->GLUE(); }

NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1) { return def_->CHOICE(choice0, choice1); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2) { return def_->CHOICE(choice0, choice1, choice2); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3) { return def_->CHOICE(choice0, choice1, choice2, choice3); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15, choice16); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15, choice16, choice17); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15, choice16, choice17, choice18); }
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19) { return def_->CHOICE(choice0, choice1, choice2, choice3, choice4, choice5, choice6, choice7, choice8, choice9, choice10, choice11, choice12, choice13, choice14, choice15, choice16, choice17, choice18, choice19); }

NODE SyntaxDefinition::GLUE(NODE child0, NODE child1) { return def_->GLUE(child0, child1); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2) { return def_->GLUE(child0, child1, child2); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3) { return def_->GLUE(child0, child1, child2, child3); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4) { return def_->GLUE(child0, child1, child2, child3, child4); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5) { return def_->GLUE(child0, child1, child2, child3, child4, child5); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15, NODE child16) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15, child16); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15, NODE child16, NODE child17) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15, child16, child17); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15, NODE child16, NODE child17, NODE child18) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15, child16, child17, child18); }
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15, NODE child16, NODE child17, NODE child18, NODE child19) { return def_->GLUE(child0, child1, child2, child3, child4, child5, child6, child7, child8, child9, child10, child11, child12, child13, child14, child15, child16, child17, child18, child19); }

NODE SyntaxDefinition::HINT(const char *text, NODE entry)  { return def_->HINT(text, entry); }
NODE SyntaxDefinition::EXPECT(const char *text, NODE entry)  { return def_->EXPECT(text, entry); }

int SyntaxDefinition::DEFINE(const char *ruleName, NODE entry) { return def_->DEFINE(ruleName, entry); }
void SyntaxDefinition::DEFINE_VOID(const char *ruleName, NODE entry) { def_->DEFINE(ruleName, entry, false); }
int SyntaxDefinition::DEFINE_OPTIONAL(const char *ruleName, bool generate, NODE entry) { return def_->DEFINE(ruleName, entry, generate); }

void SyntaxDefinition::ENTRY(const char *ruleName) { def_->ENTRY(ruleName); }
NODE SyntaxDefinition::REF(const char *ruleName) { return def_->REF(ruleName); }
NODE SyntaxDefinition::INLINE(const char *ruleName) { return def_->INLINE(ruleName); }
NODE SyntaxDefinition::INVOKE(const char *ruleName, NODE coverage) { return def_->INVOKE(ruleName, coverage); }
NODE SyntaxDefinition::PREVIOUS(const char *ruleName, const char *keyword) { return def_->PREVIOUS(ruleName, keyword); }
NODE SyntaxDefinition::CONTEXT(const char *ruleName, NODE inContext, NODE outOfContext) { return def_->CONTEXT(ruleName, inContext, outOfContext); }

NODE SyntaxDefinition::CALL(CallBack callBack, Object *self) { return def_->CALL(callBack, self); }
NODE SyntaxDefinition::ERROR() { return def_->ERROR(); }

void SyntaxDefinition::LINK() { return def_->LINK(); }

NODE SyntaxDefinition::SET(const char *name, bool value) { return def_->SET(name, value); }
NODE SyntaxDefinition::IF(const char *name, NODE trueBranch, NODE falseBranch) { return def_->IF(name, trueBranch, falseBranch); }
NODE SyntaxDefinition::CAPTURE(const char *name, NODE coverage) { return def_->CAPTURE(name, coverage); }
NODE SyntaxDefinition::REPLAY(const char *name) { return def_->REPLAY(name); }

} // namespace flux
