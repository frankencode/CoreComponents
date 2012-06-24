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
#ifndef FTL_SYNTAXDEFINITION_HPP
#define FTL_SYNTAXDEFINITION_HPP

#include "atoms"
#include "Crc32.hpp"
#include "PrefixTree.hpp"
#include "SyntaxNode.hpp"

namespace ftl
{

class SyntaxDebugger;

namespace syntax
{

typedef SyntaxNode Node;

class RefNode;
class Definition;

class RuleNode: public Node
{
public:
	RuleNode(int definitionId, const char* name, int ruleId, Ref<Node> entry, bool isVoid = false)
		: definitionId_(definitionId),
		  name_(name),
		  id_(ruleId),
		  isVoid_(isVoid),
		  used_(false),
		  numberOfRefs_(-1)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		Ref<Token, Owner> token;
		if (tokenFactory) {
			token = tokenFactory->produce();
			token->init(definitionId_, id_);
			if (parentToken)
				parentToken->appendChild(token);
		}

		int i0 = i;
		i = (entry()) ? entry()->matchNext(media, i, tokenFactory, token, state) : i;

		if (tokenFactory) {
			if (i != -1) {
				if ((isVoid_) && (parentToken)) {
					parentToken->appendAllChildrenOf(token);
					token->unlink();
				}
				else {
					token->setRange(i0, i);
				}
			}
			else {
				token->unlink();
			}
		}

		return i;
	}

	int numberOfRefs() {
		if (numberOfRefs_ == -1) {
			numberOfRefs_ = 0;
			Ref<Node> node = Node::first();
			while (node) {
				if (Ref<RefNode>(node)) ++numberOfRefs_;
				node = node->next();
			}
		}
		return numberOfRefs_;
	}

	inline int id() const { return id_; }
	inline const char* name() const { return name_; }

	inline Ref<Node> entry() const { return Node::firstChild(); }
	inline bool isVoid() const { return isVoid_; }

	inline bool used() const { return used_; }
	inline void markUsed() { used_ = true; }

protected:
	friend class InlineNode;

	int definitionId_;
	const char* name_;
	int id_;
	bool isVoid_;
	bool used_;
	int numberOfRefs_;
};

class LinkNode: public Node
{
public:
	LinkNode(const char* ruleName)
		: ruleName_(ruleName)
	{}

	LinkNode(Ref<RuleNode> rule)
		: ruleName_(rule->name()),
		  rule_(rule)
	{}

	inline const char* ruleName() const { return ruleName_; }
	inline Ref<RuleNode> rule() const { return rule_; }

protected:
	friend class ftl::syntax::Definition;
	friend class ftl::SyntaxDebugger;

	const char* ruleName_;
	Ref<RuleNode> rule_;
	Ref<LinkNode, Owner> unresolvedNext_;
};

class RefNode: public LinkNode
{
public:
	RefNode(const char* ruleName = 0)
		: LinkNode(ruleName)
	{}

	RefNode(Ref<RuleNode> rule)
		: LinkNode(rule)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		i = LinkNode::rule_->matchNext(media, i, tokenFactory, parentToken, state);

		if (i == -1)
			rollBack(parentToken, lastChildSaved);

		return i;
	}
};

class InlineNode: public LinkNode
{
public:
	InlineNode(const char* ruleName)
		: LinkNode(ruleName)
	{}

	InlineNode(Ref<RuleNode> rule)
		: LinkNode(rule)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		return LinkNode::rule_->entry()->matchNext(media, i, tokenFactory, parentToken, state);
	}
};

class PreviousNode: public LinkNode
{
public:
	PreviousNode(const char* ruleName, const char* keyword = 0)
		: LinkNode(ruleName),
		  keywordName_(keyword),
		  keyword_(-1)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		int h = -1;
		if (parentToken) {
			Ref<Token> sibling = parentToken->previousSibling();
			if (sibling)
				if ( (sibling->rule() == LinkNode::rule_->id()) &&
					 ((keyword_ == -1) || (sibling->keyword() == keyword_)) )
					h = i;
		}

		return h;
	}

	inline const char* keywordName() const { return keywordName_; }

protected:
	friend class Definition;
	const char* keywordName_;
	int keyword_;
	Ref<PreviousNode, Owner> unresolvedKeywordNext_;
};

class ContextNode: public LinkNode
{
public:
	ContextNode(const char* ruleName, Ref<Node> entry = 0)
		: LinkNode(ruleName)
	{
		if (entry) appendChild(entry);
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		int h = -1;

		if (parentToken) {
			parentToken = parentToken->parent();
			if (parentToken) {
				if (parentToken->rule() == LinkNode::rule_->id()) {
					h = i;
					if (Node::firstChild()) {
						Ref<Token> lastChildSaved;
						if (parentToken) lastChildSaved = parentToken->lastChild();

						h = Node::firstChild()->matchNext(media, h, tokenFactory, parentToken, state);

						if (h == -1)
							rollBack(parentToken, lastChildSaved);
					}
				}
			}
		}

		return h;
	}
};

class Definition;

class DebugFactory: public Instance
{
public:
	virtual Node* produce(Node* newNode, const char* nodeType) = 0;

protected:
	Ref<Definition> definition() const { return definition_; }

private:
	friend class Definition;
	Ref<Definition> definition_;
};

class InvokeNode;

class Definition: public RefNode
{
public:
	Definition(Ref<DebugFactory> debugFactory = 0);

	inline Ref<DebugFactory> debugFactory() const { return debugFactory_; }

	inline int id() const { return id_; }
	inline const char* name() const { return name_; }

	//-- stateless definition interface

	inline void SYNTAX(const char* name) {
		id_ = crc32(name);
		name_ = name;
	}

	inline void IMPORT(Ref<Definition> definition, const char* name = 0) {
		if (!name) name = definition->name();
		if (!name)
			FTL_THROW(DebugException, "Cannot import anonymous syntax definition");
		definitionByName_->insert(name, definition);
	}

	typedef Ref<Node, Owner> NODE;
	typedef Ref<RuleNode, Owner> RULE;

	inline void OPTION(const char* name, bool value) {
		if (str::casecmp(name, "caseSensitive") == 0)
			caseSensitive_ = value;
		else
			FTL_THROW(DebugException, str::cat("Unknown option '", name, "'"));
	}

	NODE CHAR(char ch);
	NODE OTHER(char ch);
	NODE GREATER(char ch);
	NODE BELOW(char ch);
	NODE GREATER_OR_EQUAL(char ch);
	NODE BELOW_OR_EQUAL(char ch);
	NODE ANY();

	NODE RANGE(char a, char b);
	NODE RANGE(const char* s);
	NODE EXCEPT(char a, char b);
	NODE EXCEPT(const char* s);

	NODE STRING(const char* s);
	NODE KEYWORD(const char* keywords);

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

	NODE HINT(const char* text = "");
	NODE DONE();

	inline int DEFINE(const char* ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode, Owner> rule = new RuleNode(id_, ruleName, numRules_++, entry);
		addRule(rule);
		return rule->id();
	}
	inline void DEFINE_VOID(const char* ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode, Owner> rule = new RuleNode(id_, ruleName, numRules_++, entry, true);
		addRule(rule);
	}
	inline void ENTRY(const char* ruleName) {
		LinkNode::ruleName_ = ruleName;
	}

	inline NODE REF(const char* ruleName) {
		Ref<RefNode, Owner> link = new RefNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Ref");
	}
	inline NODE INLINE(const char* ruleName) {
		Ref<InlineNode, Owner> link = new InlineNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Inline");
	}
	inline NODE PREVIOUS(const char* ruleName, const char* keyword = 0) {
		Ref<PreviousNode, Owner> link = new PreviousNode(ruleName, keyword);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		if (keyword) {
			link->unresolvedKeywordNext_ = unresolvedKeywordHead_;
			unresolvedKeywordHead_ = link;
		}
		return debug(link, "Previous");
	}
	inline NODE CONTEXT(const char* ruleName, NODE entry = 0) {
		Ref<ContextNode, Owner> link = new ContextNode(ruleName, entry);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Context");
	}

	typedef int (*CallBack) (Ref<Instance> self, ByteArray* media, int i, SyntaxState* state);

	NODE CALL(CallBack callBack, Ref<Instance> self = 0);
	NODE ERROR();

	void OPTIMIZE();
	void LINK(bool optimize = true);

	//-- stateful definition interface

	inline void STATE_FLAG(const char* name, bool defaultValue = false) {
		stateFlagHead_ = new StateFlag(stateFlagHead_, defaultValue);
		flagIdByName()->insert(name, numStateFlags_);
		++numStateFlags_;
	}
	inline void STATE_CHAR(const char* name, char defaultValue = 0) {
		stateCharHead_ = new StateChar(stateCharHead_, defaultValue);
		charIdByName()->insert(name, numStateChars_);
		++numStateChars_;
	}
	inline void STATE_STRING(const char* name, const char* defaultValue = 0) {
		stateStringHead_ = new StateString(stateStringHead_, defaultValue);
		stringIdByName()->insert(name, numStateStrings_);
		++numStateStrings_;
	}
	NODE SET(const char* name, bool value);
	NODE IF(const char* name, NODE trueBranch, NODE falseBranch = 0);
	NODE GETCHAR(const char* name);
	NODE SETCHAR(const char* name, char value);
	NODE VARCHAR(const char* name);
	NODE VAROTHER(const char* name);
	NODE GETSTRING(const char* name, NODE coverage);
	NODE SETSTRING(const char* name, const char* value);
	NODE VARSTRING(const char* name);

	NODE INVOKE(Definition* definition, NODE coverage = 0);
	NODE INVOKE(const char* definitionName, NODE coverage = 0);

	//-- execution interface

	inline int numRules() const { return numRules_; }

	inline bool stateful() const { return (stateFlagHead_) || (stateCharHead_) || (stateStringHead_) || statefulScope_ || hasHints_; }

	SyntaxState* newState(SyntaxState* parent = 0) const;

	Ref<Token, Owner> find(ByteArray* media, int* i0, int* i1 = 0, Ref<TokenFactory> tokenFactory = 0) const;
	Ref<Token, Owner> match(ByteArray* media, int i0 = 0, int* i1 = 0, SyntaxState* state = 0, Ref<TokenFactory> tokenFactory = 0) const;

	Ref<Definition> resolveScope(const char*& name) const;

	Ref<Definition> definitionByName(const char* name) const;
	Ref<RuleNode> ruleByName(const char* name) const;
	int keywordByName(const char* keyword);

	static void getLineAndPosFromIndex(ByteArray* media, int index, int* line, int* pos);

	virtual int syntaxError(ByteArray* media, int index, SyntaxState* state) const;

	inline Node* debug(Node* newNode, const char* nodeType) {
		return debugFactory_ ? debugFactory_->produce(newNode, nodeType) : newNode;
	}

private:
	friend class ftl::SyntaxDebugger;
	Ref<DebugFactory, Owner> debugFactory_;

	friend class InvokeNode;

	int id_;
	const char* name_;
	bool caseSensitive_;

	typedef PrefixTree<char, Ref<Definition, Owner> > DefinitionByName;
	Ref<DefinitionByName, Owner> definitionByName_;

	typedef PrefixTree<char, Ref<RuleNode, Owner> > RuleByName;
	typedef PrefixTree<char, int> KeywordByName;

	int numRules_;
	int numKeywords_;
	Ref<RuleByName, Owner> ruleByName_;
	Ref<KeywordByName, Owner> keywordByName_;

	void addRule(Ref<RuleNode> rule)
	{
		if (!ruleByName_->insert(rule->name(), rule))
			FTL_THROW(DebugException, str::cat("Redefinition of rule '", rule->name(), "'"));
	}

	Ref<LinkNode, Owner> unresolvedLinkHead_;
	Ref<PreviousNode, Owner> unresolvedKeywordHead_;
	Ref<InvokeNode, Owner> unresolvedInvokeHead_;
	Ref<Definition, Owner> unresolvedNext_;
	bool statefulScope_;
	bool hasHints_;

	class StateFlag: public Instance {
	public:
		StateFlag(Ref<StateFlag> head, bool defaultValue)
			: next_(head),
			  defaultValue_(defaultValue)
		{}
		Ref<StateFlag, Owner> next_;
		bool defaultValue_;
	};

	class StateChar: public Instance {
	public:
		StateChar(Ref<StateChar> head, char defaultValue)
			: next_(head),
			  defaultValue_(defaultValue)
		{}
		Ref<StateChar, Owner> next_;
		char defaultValue_;
	};

	class StateString: public Instance {
	public:
		StateString(Ref<StateString> head, const char* defaultValue)
			: next_(head),
			  defaultValue_(str::len(defaultValue))
		{
			str::cpy(defaultValue_.data(), defaultValue, defaultValue_.size());
		}
		Ref<StateString, Owner> next_;
		ByteArray defaultValue_;
	};

	int numStateFlags_;
	int numStateChars_;
	int numStateStrings_;
	Ref<StateFlag, Owner> stateFlagHead_;
	Ref<StateChar, Owner> stateCharHead_;
	Ref<StateString, Owner> stateStringHead_;

	typedef PrefixTree<char, int> StateIdByName;

	Ref<StateIdByName, Owner> flagIdByName_;
	Ref<StateIdByName, Owner> charIdByName_;
	Ref<StateIdByName, Owner> stringIdByName_;

	int flagIdByName(const char* name);
	int charIdByName(const char* name);
	int stringIdByName(const char* name);

	inline Ref<StateIdByName> flagIdByName() {
		if (!flagIdByName_) flagIdByName_ = new StateIdByName;
		return flagIdByName_;
	}
	inline Ref<StateIdByName> charIdByName() {
		if (!charIdByName_) charIdByName_ = new StateIdByName;
		return charIdByName_;
	}
	inline Ref<StateIdByName> stringIdByName() {
		if (!stringIdByName_) stringIdByName_ = new StateIdByName;
		return stringIdByName_;
	}

	static int errorCallBack(Ref<Instance> self, ByteArray* media, int index, SyntaxState* state);
};

class InvokeNode: public Node
{
public:
	InvokeNode(Ref<Definition> definition, Ref<Node> coverage)
		: definition_(definition)
	{
		if (coverage) appendChild(coverage);
	}

	InvokeNode(const char* name, Ref<Node> coverage)
		: definitionName_(name)
	{
		if (coverage) appendChild(coverage);
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		SyntaxState* childState = 0;
		if (state) {
			childState = state->child();
			if (childState) {
				if (childState->definitionId() != definition_->id())
					childState = 0;
			}
			if (!childState)
				childState = definition_->newState(state);
		}

		if (coverage())
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();

			int i0 = i;
			i = coverage()->matchNext(media, i, 0, parentToken, state);

			if (i != -1)
			{
				rollBack(parentToken, lastChildSaved);

				ByteArray range(media, i);
				definition_->matchNext(&range, i0, tokenFactory, parentToken, childState);
			}
		}
		else {
			i = definition_->matchNext(media, i, tokenFactory, parentToken, childState);
		}

		return i;
	}

	inline const char* definitionName() const { return definitionName_; }
	inline Ref<Node> coverage() const { return Node::firstChild(); }

private:
	friend class Definition;

	const char* definitionName_;
	Ref<Definition> definition_;
	Ref<InvokeNode, Owner> unresolvedNext_;
};

} // namespace syntax

typedef syntax::Definition SyntaxDefinition;
typedef syntax::DebugFactory SyntaxDebugFactory;

} // namespace ftl

#endif // FTL_SYNTAXDEFINITION_HPP
