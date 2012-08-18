/*
 * SyntaxDefinition.hpp -- syntax description and production formalism
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAXDEFINITIONWRAPPER_HPP
#define FTL_SYNTAXDEFINITIONWRAPPER_HPP

#include "ByteArray.hpp"
#include "TokenFactory.hpp"
#include "SyntaxNode.hpp"
#include "SyntaxDebugFactory.hpp"
#include "SyntaxState.hpp"

namespace ftl
{

namespace syntax
{

class DefinitionNode;

class Definition: public Instance
{
public:
	typedef syntax::Node Node;
	typedef syntax::NODE NODE;

	inline static Ref<Definition, Owner> newInstance(Ref<DebugFactory> debugFactory = 0) {
		return new Definition(debugFactory);
	}
	~Definition();

	int id() const;
	const char *name() const;

	Ref<DebugFactory> debugFactory() const;
	Node *debug(Node *newNode, const char *nodeType);

	int keywordByName(const char *keyword);
	State *newState(State *parent = 0) const;

	Ref<Token, Owner> find(const ByteArray *media, int i = 0) const;
	Ref<Token, Owner> match(const ByteArray *media, int i = -1, Ref<SyntaxState> state = 0) const;

	void SYNTAX(const char *name);
	void IMPORT(Ref<Definition> definition, const char *name = 0);
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

	NODE HINT(const char *text = "");
	NODE DONE();

	int DEFINE(const char *ruleName, NODE entry = 0);
	void DEFINE_VOID(const char *ruleName, NODE entry = 0);
	void ENTRY(const char *ruleName);
	NODE REF(const char *ruleName);
	NODE INLINE(const char *ruleName);
	NODE PREVIOUS(const char *ruleName, const char *keyword = 0);
	NODE CONTEXT(const char *ruleName, NODE entry = 0);

	typedef int (*CallBack) (Ref<Instance> self, ByteArray *media, int i, State *state);

	NODE CALL(CallBack callBack, Ref<Instance> self = 0);
	NODE ERROR();

	void OPTIMIZE();
	void LINK(bool optimize = true);

	void STATE_FLAG(const char *name, bool defaultValue = false);
	void STATE_CHAR(const char *name, char defaultValue = 0);
	void STATE_STRING(const char *name, const char *defaultValue = "");
	void TOUCH_STRING(const char *name);

	NODE SET(const char *name, bool value);
	NODE IF(const char *name, NODE trueBranch, NODE falseBranch = 0);
	NODE GETCHAR(const char *name);
	NODE SETCHAR(const char *name, char value);
	NODE VARCHAR(const char *name);
	NODE VAROTHER(const char *name);
	NODE GETSTRING(const char *name, NODE coverage);
	NODE SETSTRING(const char *name, const char *value);
	NODE VARSTRING(const char *name);

	NODE INVOKE(DefinitionNode *definition, NODE coverage = 0);
	NODE INVOKE(const char *definitionName, NODE coverage = 0);

protected:
	Definition(Ref<DebugFactory> debugFactory = 0);

private:
	Definition(const Definition &);
	// const Definition &operator=(const Defintion &);

	Ref<DefinitionNode, Owner> def_;
};

} // namespace syntax

typedef syntax::Definition SyntaxDefinition;

} // namespace ftl

#endif // FTL_SYNTAXDEFINITIONWRAPPER_HPP
