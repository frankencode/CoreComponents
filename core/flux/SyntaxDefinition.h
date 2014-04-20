/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYNTAXDEFINITION_H
#define FLUX_SYNTAXDEFINITION_H

#include "ByteArray.h"
#include "TokenFactory.h"
#include "SyntaxNode.h"
#include "SyntaxDebugFactory.h"
#include "SyntaxState.h"

namespace flux
{

namespace syntax
{

class DefinitionNode;

class Definition: public Object
{
public:
	typedef syntax::Node Node;
	typedef syntax::NODE NODE;

	inline static Ref<Definition> create(DebugFactory *debugFactory = 0) {
		return new Definition(debugFactory);
	}
	~Definition();

	int id() const;
	const char *name() const;

	DebugFactory *debugFactory() const;
	Node *debug(Node *newNode, const char *nodeType);

	int keywordByName(const char *keyword);
	Ref<State> newState(State *parent = 0) const;

	Ref<Token> find(const ByteArray *media, int i = 0) const;
	Ref<Token> match(const ByteArray *media, int i = -1, SyntaxState *state = 0) const;
	Ref<Token> match(const ByteArray *media, SyntaxState *state) const;

	int matchLength() const;

	void SYNTAX(const char *name);
	void IMPORT(Definition *definition, const char *name = 0);
	void OPTION(const char *name, bool value);

	NODE CHAR(char ch);
	NODE OTHER(char ch);
	NODE GREATER(char ch);
	NODE BELOW(char ch);
	NODE GREATER_OR_EQUAL(char ch);
	NODE BELOW_OR_EQUAL(char ch);
	NODE ANY();

	NODE RANGE(char a, char b);
	NODE RANGE(const char *s);
	NODE EXCEPT(char a, char b);
	NODE EXCEPT(const char *s);

	NODE STRING(const char *s);
	NODE KEYWORD(const char *keywords);

	NODE REPEAT(int minRepeat, int maxRepeat, NODE entry);
	NODE REPEAT(int minRepeat, NODE entry);
	NODE REPEAT(NODE entry);

	NODE LAZY_REPEAT(int minRepeat, NODE entry);
	NODE LAZY_REPEAT(NODE entry);

	NODE GREEDY_REPEAT(int minRepeat, int maxRepeat, NODE entry);
	NODE GREEDY_REPEAT(int minRepeat, NODE entry);
	NODE GREEDY_REPEAT(NODE entry);

	NODE LENGTH(int minLength, int maxLength, NODE entry);
	NODE LENGTH(int minLength, NODE entry);
	NODE BOI();
	NODE EOI();
	NODE PASS();
	NODE FAIL();
	NODE FIND(NODE entry);
	NODE AHEAD(NODE entry);
	NODE NOT(NODE entry);
	NODE BEHIND(NODE entry);
	NODE NOT_BEHIND(NODE entry);

	NODE CHOICE();
	NODE GLUE();

	NODE CHOICE(NODE choice0, NODE choice1);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14);
	NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15);

	NODE GLUE(NODE child0, NODE child1);
	NODE GLUE(NODE child0, NODE child1, NODE child2);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14);
	NODE GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15);

	NODE EXPECT(const char *text, NODE entry);

	int DEFINE(const char *ruleName, NODE entry = 0);
	void ENTRY(const char *ruleName);
	NODE REF(const char *ruleName);
	NODE INLINE(const char *ruleName);
	NODE PREVIOUS(const char *ruleName, const char *keyword = 0);
	NODE CONTEXT(const char *ruleName, NODE inContext = 0, NODE outOfContext = 0);

	typedef int (*CallBack) (Object *self, ByteArray *media, int i, State *state);

	NODE CALL(CallBack callBack, Object *self = 0);
	NODE ERROR();

	void LINK();

	NODE SET(const char *name, bool value);
	NODE IF(const char *name, NODE trueBranch, NODE falseBranch = 0);
	NODE CAPTURE(const char *name, NODE coverage);
	NODE REPLAY(const char *name);

	NODE INVOKE(DefinitionNode *definition, NODE coverage = 0);
	NODE INVOKE(const char *definitionName, NODE coverage = 0);

protected:
	Definition(DebugFactory *debugFactory = 0);

private:
	Definition(const Definition &);
	// const Definition &operator=(const Defintion &);

	Ref<DefinitionNode> def_;
};

} // namespace syntax

typedef syntax::Definition SyntaxDefinition;

} // namespace flux

#endif // FLUX_SYNTAXDEFINITION_H
