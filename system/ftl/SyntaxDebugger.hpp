/*
 * SyntaxDebugger.hpp -- debugging syntax descriptions
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAXDEBUGGER_HPP
#define FTL_SYNTAXDEBUGGER_HPP

#include "syntax.hpp"
#include "String.hpp"
#include "Map.hpp"
#include "PrintDebug.hpp"

namespace ftl
{

namespace syntax
{

class DebugNode: public Node {
public:
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const {
		return entry()->matchNext(media, i, tokenFactory, parentToken, state);
	}

	virtual Ref<Node> succ(Ref<Node> node) const {
		return Node::parent() ? Node::parent()->succ(this) : Ref<Node>();
	}

	virtual int matchLength() const {
		return entry() ? entry()->matchLength() : -1;
	}

	virtual const char* declType() const = 0;
	virtual void printAttributes(String indent) {}

	virtual void printNext(String indent = "");

	inline Ref<Node> entry() const { return Node::firstChild(); }

protected:
	DebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: debugger_(debugger)
	{
		appendChild(newNode);
	}

	void printBranch(Ref<Node> node, String indent);

	String superIndent(String indent) const;
	String subIndent(String indent) const;

	Ref<Debugger> debugger_;
};

class Debugger: public SyntaxDebugFactory
{
public:
	typedef SyntaxDebugFactory DebugFactory;
	typedef SyntaxDefinitionNode DefinitionNode;
	typedef SyntaxNode Node;

	inline static Ref<Debugger, Owner> newInstance(String indent = "\t") {
		return new Debugger(indent);
	}

	virtual Node* produce(Node* newNode, const char* nodeType);
	void printDefinition(bool omitUnusedRules = false);

	typedef syntax::DefinitionNode::StateIdByName StateIdByName;
	typedef Map<int, String> StateNameById;

	Ref<StateNameById, Owner> newReverseMap(Ref<StateIdByName> stateIdByName);
	Ref<StateNameById> flagNameById();
	Ref<StateNameById> charNameById();
	Ref<StateNameById> stringNameById();

private:
	friend class syntax::DefinitionNode;
	friend class DebugNode;

	Debugger(String indent);

	static void determineRulesInUse(Ref<syntax::RuleNode> rule);

	class NodeFactory: public Instance {
	public:
		virtual Node* produce(Node* newNode) = 0;
	};

	template<class DebugNodeType>
	class DebugNodeFactory: public NodeFactory {
	public:
		DebugNodeFactory(Ref<Debugger> debugger)
			: debugger_(debugger)
		{}
		virtual Node* produce(Node* newNode) {
			return new DebugNodeType(debugger_, newNode);
		}
	private:
		Ref<Debugger> debugger_;
	};

	typedef PrefixTree< char, Ref<NodeFactory, Owner> > FactoryByNodeType;
	Ref<FactoryByNodeType, Owner> factoryByNodeType_;

	String indent_;

	Ref<StateNameById, Owner> flagNameById_;
	Ref<StateNameById, Owner> charNameById_;
	Ref<StateNameById, Owner> stringNameById_;
};

} // namespace syntax

typedef syntax::DebugNode SyntaxDebugNode;
typedef syntax::Debugger SyntaxDebugger;

} // namespace ftl

#endif // FTL_SYNTAXDEBUGGER_HPP
