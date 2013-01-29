 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SYNTAXDEBUGGER_HPP
#define FTL_SYNTAXDEBUGGER_HPP

#include "syntax.hpp"
#include "string.hpp"
#include "Map.hpp"
#include "PrintDebug.hpp"

namespace ftl
{

namespace syntax
{

class DebugNode: public Node {
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, SyntaxState *state) const {
		return entry()->matchNext(media, i, tokenFactory, parentToken, state);
	}

	virtual Node *succ(Node *node) const {
		return Node::parent() ? Node::parent()->succ(Node::self()) : null<Node>();
	}

	virtual int matchLength() const {
		return entry() ? entry()->matchLength() : -1;
	}

	virtual const char *declType() const = 0;
	virtual void printAttributes(string indent) {}

	virtual void printNext(string indent = "");

	inline Node *entry() const { return Node::firstChild(); }

protected:
	DebugNode(Debugger *debugger, Node *newNode)
		: debugger_(debugger)
	{
		appendChild(newNode);
	}

	void printBranch(Node *node, string indent);

	string superIndent(string indent) const;
	string subIndent(string indent) const;

	Debugger *debugger_;
};

class Debugger: public SyntaxDebugFactory
{
public:
	typedef SyntaxDebugFactory DebugFactory;
	typedef SyntaxNode Node;

	inline static hook<Debugger> create(string indent = "\t") {
		return new Debugger(indent);
	}

	virtual Node *produce(Node *newNode, const char *nodeType);
	void printDefinition(bool omitUnusedRules = false);

	typedef DefinitionNode::StateIdByName StateIdByName;
	typedef Map<int, string> StateNameById;

	hook<StateNameById> newReverseMap(StateIdByName *stateIdByName);
	StateNameById *flagNameById();
	StateNameById *captureNameById();

private:
	friend class DefinitionNode;
	friend class DebugNode;

	Debugger(string indent);

	static void determineRulesInUse(RuleNode *rule);

	class NodeFactory: public Instance {
	public:
		virtual Node *produce(Node *newNode) = 0;
	};

	template<class DebugNodeType>
	class DebugNodeFactory: public NodeFactory {
	public:
		DebugNodeFactory(Debugger *debugger)
			: debugger_(debugger)
		{}
		virtual Node *produce(Node *newNode) {
			return new DebugNodeType(debugger_, newNode);
		}
	private:
		Debugger *debugger_;
	};

	typedef PrefixTree< char, hook<NodeFactory> > FactoryByNodeType;
	hook<FactoryByNodeType> factoryByNodeType_;

	string indent_;

	hook<StateNameById> flagNameById_;
	hook<StateNameById> captureNameById_;
};

} // namespace syntax

typedef syntax::DebugNode SyntaxDebugNode;
typedef syntax::Debugger SyntaxDebugger;

} // namespace ftl

#endif // FTL_SYNTAXDEBUGGER_HPP
