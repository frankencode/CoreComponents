/*
 * SyntaxDebugger.cpp -- debugging syntax descriptions
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "SyntaxDebugger.hpp"

namespace ftl
{

namespace syntax
{

static void printChar(char ch) {
	if (ch < ' '/*32*/) {
		if (ch == '\t')
			print("\\t");
		else if (ch == '\n')
			print("\\n");
		else if (ch == '\f')
			print("\\f");
		else if (ch == '\r')
			print("\\r");
		else if (ch == 0)
			print("\\0");
		else
			print("\\%oct%", int(ToAscii<char>::map(ch)));
	}
	else if (ch < 127) {
		if (ch == '\\')
			print("\\");
		print("%%", ToAscii<char>::map(ch));
	}
	else if (ch < 256) {
		print("\\%oct%", ToUnicode<char>::map(ch));
	}
	else {
		print("\\x%hex%", ToUnicode<char>::map(ch));
	}
}

static void printString(char *s, int size) {
	for (int i = 0; i < size; ++i) {
		char ch = s[i];
		if (ch == '"')
			print("\\");
		printChar(s[i]);
	}
}

static void printString(const ByteArray &s) {
	printString(s.data(), s.size());
}

static void printCharAttr(char ch) {
	print("'");
	if (ch == '\'')
		print("\\");
	printChar(ch);
	print("'");
}

static void printStringAttr(const ByteArray &s) {
	print("\"");
	printString(s);
	print("\"");
}

void DebugNode::printNext(String indent)
{
	print("%%%%(", indent, declType());
	printAttributes(Format() << indent << debugger_->indent_);
	print(")");
}

void DebugNode::printBranch(Ref<Node> node, String indent) {
	if (node) {
		Ref<DebugNode> debugNode = node;
		if (debugNode) debugNode->printNext(indent);
		else print(indent);
	}
	else {
		print("%%PASS()", indent);
	}
}

String DebugNode::superIndent(String indent) const {
	return (indent != "") ? indent->copy(0, indent->size() - debugger_->indent_->size()) : indent;
}

String DebugNode::subIndent(String indent) const {
	return Format() << indent << DebugNode::debugger_->indent_;
}

class CharDebugNode: public DebugNode {
public:
	CharDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return charNode()->invert() ? "OTHER" : "CHAR"; }

	virtual void printAttributes(String indent) {
		printCharAttr(charNode()->ch());
	}

private:
	typedef syntax::CharNode CharNode;
	inline Ref<CharNode> charNode() const { return DebugNode::entry(); }
};

class GreaterDebugNode: public DebugNode {
public:
	GreaterDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return greaterNode()->invert() ? "BELOW" : "GREATER"; }

	virtual void printAttributes(String index) {
		printCharAttr(greaterNode()->ch());
	}

private:
	typedef syntax::GreaterNode GreaterNode;
	inline Ref<GreaterNode> greaterNode() const { return DebugNode::entry(); }
};

class GreaterOrEqualDebugNode: public DebugNode {
public:
	GreaterOrEqualDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return greaterOrEqualNode()->invert() ? "BELOW_OR_EQUAL" : "GREATER_OR_EQUAL"; }

	virtual void printAttributes(String index) {
		printCharAttr(greaterOrEqualNode()->ch());
	}

private:
	typedef syntax::GreaterOrEqualNode GreaterOrEqualNode;
	inline Ref<GreaterOrEqualNode> greaterOrEqualNode() const { return DebugNode::entry(); }
};

class AnyDebugNode: public DebugNode {
public:
	AnyDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "ANY"; }
};

class RangeMinMaxDebugNode: public DebugNode {
public:
	RangeMinMaxDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return rangeMinMaxNode()->invert() ? "EXCEPT" : "RANGE"; }

	virtual void printAttributes(String indent) {
		Ref<RangeMinMaxNode> node = rangeMinMaxNode();
		printCharAttr(node->a());
		print(", ");
		printCharAttr(node->b());
	}

private:
	typedef syntax::RangeMinMaxNode RangeMinMaxNode;
	inline Ref<RangeMinMaxNode> rangeMinMaxNode() const { return DebugNode::entry(); }
};

class RangeExplicitDebugNode: public DebugNode {
public:
	RangeExplicitDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return rangeExplicitNode()->invert() ? "EXCEPT" : "RANGE"; }

	virtual void printAttributes(String indent) {
		printStringAttr(rangeExplicitNode()->s());
	}

private:
	typedef syntax::RangeExplicitNode RangeExplicitNode;
	inline Ref<RangeExplicitNode> rangeExplicitNode() const { return DebugNode::entry(); }
};

class StringDebugNode: public DebugNode {
public:
	StringDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "STRING"; }

	virtual void printAttributes(String indent) {
		printStringAttr(stringNode()->s());
	}

private:
	typedef syntax::StringNode StringNode;
	inline Ref<StringNode> stringNode() const { return DebugNode::entry(); }
};

class KeywordDebugNode: public DebugNode {
public:
	KeywordDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "KEYWORD"; }

	virtual void printAttributes(String indent) {
		print("\n%%\"", indent);
		Ref<KeywordMap> map = keywordNode()->map();
		for (KeywordMap::Index index = map->first(); map->has(index); ++index) {
			Ref<KeywordMap::Key, Owner> s = map->key(index);
			printString(*s);
			if (index != map->last())
				print(" ");
		}
		print("\"\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::KeywordNode KeywordNode;
	typedef syntax::KeywordMap KeywordMap;
	inline Ref<KeywordNode> keywordNode() const { return DebugNode::entry(); }
};

class RepeatDebugNode: public DebugNode {
public:
	RepeatDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "REPEAT"; }

	virtual void printAttributes(String indent) {
		Ref<RepeatNode> node = repeatNode();
		if (node->maxRepeat() != intMax)
			print("%%, %%,", node->minRepeat(), node->maxRepeat());
		else if (node->minRepeat() != 0)
			print("%%,", node->minRepeat());
		print("\n");
		printBranch(node->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::RepeatNode RepeatNode;
	inline Ref<RepeatNode> repeatNode() const { return DebugNode::entry(); }
};

class LazyRepeatDebugNode: public DebugNode {
public:
	LazyRepeatDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "LAZY_REPEAT"; }

	virtual void printAttributes(String indent) {
		Ref<LazyRepeatNode> node = repeatNode();
		if (node->minRepeat() != 0)
			print("%%,", node->minRepeat());
		print("\n");
		printBranch(node->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::LazyRepeatNode LazyRepeatNode;
	inline Ref<LazyRepeatNode> repeatNode() const { return DebugNode::entry(); }
};

class GreedyRepeatDebugNode: public DebugNode {
public:
	GreedyRepeatDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "GREEDY_REPEAT"; }

	virtual void printAttributes(String indent) {
		Ref<GreedyRepeatNode> node = repeatNode();
		if (node->maxRepeat() != intMax)
			print("%%, %%,", node->minRepeat(), node->maxRepeat());
		else if (node->minRepeat() != 0)
			print("%%,", node->minRepeat());
		print("\n");
		printBranch(node->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::GreedyRepeatNode GreedyRepeatNode;
	inline Ref<GreedyRepeatNode> repeatNode() const { return DebugNode::entry(); }
};

class LengthDebugNode: public DebugNode {
public:
	LengthDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "LENGTH"; }

	virtual void printAttributes(String indent) {
		Ref<LengthNode> node = lengthNode();
		if (node->maxLength() != intMax)
			print("%%, %%,\n", node->minLength(), node->maxLength());
		else
			print("%%,\n", node->minLength());
		printBranch(node->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::LengthNode LengthNode;
	inline Ref<LengthNode> lengthNode() const { return DebugNode::entry(); }
};

class BoiDebugNode: public DebugNode {
public:
	BoiDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "BOI"; }
};

class EoiDebugNode: public DebugNode {
public:
	EoiDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "EOI"; }
};

class PassDebugNode: public DebugNode {
public:
	PassDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return passNode()->invert() ? "FAIL" : "PASS"; }

private:
	typedef syntax::PassNode PassNode;
	inline Ref<PassNode> passNode() const { return DebugNode::entry(); }
};

class FindDebugNode: public DebugNode {
public:
	FindDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "FIND"; }

	virtual void printAttributes(String indent) {
		print("\n");
		printBranch(findNode()->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::FindNode FindNode;
	inline Ref<FindNode> findNode() const { return DebugNode::entry(); }
};

class AheadDebugNode: public DebugNode {
public:
	AheadDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return aheadNode()->invert() ? "NOT" : "AHEAD"; }

	virtual void printAttributes(String indent) {
		print("\n");
		printBranch(aheadNode()->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::AheadNode AheadNode;
	inline Ref<AheadNode> aheadNode() const { return DebugNode::entry(); }
};

class BehindDebugNode: public DebugNode {
public:
	BehindDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return behindNode()->invert() ? "NOT_BEHIND" : "BEHIND"; }

	virtual void printAttributes(String indent) {
		print("\n");
		printBranch(behindNode()->entry(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::BehindNode BehindNode;
	inline Ref<BehindNode> behindNode() const { return DebugNode::entry(); }
};

class ChoiceDebugNode: public DebugNode {
public:
	ChoiceDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "CHOICE"; }

	virtual void printAttributes(String indent) {
		print("\n");
		Ref<Node> node = choiceNode()->firstChoice();
		while (node) {
			printBranch(node, indent);
			node = node->nextSibling();
			if (node) print(",\n");
		}
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::ChoiceNode ChoiceNode;
	inline Ref<ChoiceNode> choiceNode() const { return DebugNode::entry(); }
};

class GlueDebugNode: public DebugNode {
public:
	GlueDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "GLUE"; }

	virtual void printAttributes(String indent) {
		print("\n");
		Ref<Node> node = glueNode()->firstChild();
		while (node) {
			printBranch(node, indent);
			node = node->nextSibling();
			if (node) print(",\n");
		}
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::GlueNode GlueNode;
	inline Ref<GlueNode> glueNode() const { return DebugNode::entry(); }
};

class HintDebugNode: public DebugNode {
public:
	HintDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return hintNode()->text() ? "HINT" : "DONE"; }

	virtual void printAttributes(String indent) {
		if (hintNode()->text())
			print("\"%%\"", hintNode()->text());
	}

private:
	typedef syntax::HintNode HintNode;
	inline Ref<HintNode> hintNode() const { return DebugNode::entry(); }
};

class RefDebugNode: public DebugNode {
public:
	RefDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return (inlineNode()) ? "INLINE" : "REF"; }

	virtual void printAttributes(String indent) {
		print("\"%%\"", linkNode()->ruleName());
	}

private:
	typedef syntax::LinkNode LinkNode;
	inline Ref<LinkNode> linkNode() const { return DebugNode::entry(); }

	typedef syntax::InlineNode InlineNode;
	inline Ref<InlineNode> inlineNode() const { return DebugNode::entry(); }
};

class InlineDebugNode: public DebugNode {
public:
	InlineDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "INLINE"; }

	virtual void printAttributes(String indent) {
		print("\"%%\"", inlineNode()->ruleName());
	}

private:
	typedef syntax::InlineNode InlineNode;
	inline Ref<InlineNode> inlineNode() const { return DebugNode::entry(); }
};

class PreviousDebugNode: public DebugNode {
public:
	PreviousDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "PREVIOUS"; }

	virtual void printAttributes(String indent) {
		Ref<PreviousNode> node = previousNode();
		print("\"%%\"", node->ruleName());
		if (node->keywordName())
			print(", \"%%\"", node->keywordName());
	}

private:
	typedef syntax::PreviousNode PreviousNode;
	inline Ref<PreviousNode> previousNode() const { return DebugNode::entry(); }
};

class CallDebugNode: public DebugNode {
public:
	CallDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "CALL"; }

	virtual void printAttributes(String indent) {
		print("0x%hex:8.:'0'", (void *)callNode()->callBack());
	}

private:
	typedef syntax::CallNode CallNode;
	inline Ref<CallNode> callNode() const { return DebugNode::entry(); }
};

class SetDebugNode: public DebugNode {
public:
	SetDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "SET"; }

	virtual void printAttributes(String indent) {
		Ref<SetNode> node = setNode();
		print("\"%%\", %%", DebugNode::debugger_->flagNameById()->value(node->flagId()), node->value());
	}

private:
	typedef syntax::SetNode SetNode;
	inline Ref<SetNode> setNode() const { return DebugNode::entry(); }
};

class IfDebugNode: public DebugNode {
public:
	IfDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "IF"; }

	virtual void printAttributes(String indent) {
		Ref<IfNode> node = ifNode();
		print("\"%%\",\n", DebugNode::debugger_->flagNameById()->value(node->flagId()));
		printBranch(node->trueBranch(), indent);
		print(",\n");
		printBranch(node->falseBranch(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::IfNode IfNode;
	inline Ref<IfNode> ifNode() const { return DebugNode::entry(); }
};

class CaptureDebugNode: public DebugNode {
public:
	CaptureDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "CAPTURE"; }

	virtual void printAttributes(String indent) {
		Ref<CaptureNode> node = captureNode();
		print("\"%%\",\n", DebugNode::debugger_->captureNameById()->value(node->captureId()));
		printBranch(node->coverage(), indent);
		print("\n%%", DebugNode::superIndent(indent));
	}

private:
	typedef syntax::CaptureNode CaptureNode;
	inline Ref<CaptureNode> captureNode() const { return DebugNode::entry(); }
};

class ReplayDebugNode: public DebugNode {
public:
	ReplayDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "REPLAY"; }

	virtual void printAttributes(String indent) {
		print("\"%%\"", DebugNode::debugger_->captureNameById()->value(replayNode()->captureId()));
	}
private:
	typedef syntax::ReplayNode ReplayNode;
	inline Ref<ReplayNode> replayNode() const { return DebugNode::entry(); }
};

class InvokeDebugNode: public DebugNode {
public:
	InvokeDebugNode(Ref<Debugger> debugger, Ref<Node> newNode)
		: DebugNode(debugger, newNode)
	{}

	virtual const char *declType() const { return "INVOKE"; }

	virtual void printAttributes(String indent) {
		Ref<InvokeNode> node = invokeNode();
		if (node->coverage()) {
			print("\"%%\",\n", node->definitionName());
			printBranch(node->coverage(), indent);
			print("\n%%", DebugNode::superIndent(indent));
		}
		else {
			print("\"%%\"", node->definitionName());
		}
	}

private:
	typedef syntax::InvokeNode InvokeNode;
	inline Ref<InvokeNode> invokeNode() const { return DebugNode::entry(); }
};

Debugger::Debugger(String indent)
	: factoryByNodeType_(FactoryByNodeType::create()),
	  indent_(indent)
{
	factoryByNodeType_->insert("Char",           new DebugNodeFactory<CharDebugNode>           (this));
	factoryByNodeType_->insert("Greater",        new DebugNodeFactory<GreaterDebugNode>        (this));
	factoryByNodeType_->insert("GreaterOrEqual", new DebugNodeFactory<GreaterOrEqualDebugNode> (this));
	factoryByNodeType_->insert("Any",            new DebugNodeFactory<AnyDebugNode>            (this));
	factoryByNodeType_->insert("RangeMinMax",    new DebugNodeFactory<RangeMinMaxDebugNode>    (this));
	factoryByNodeType_->insert("RangeExplicit",  new DebugNodeFactory<RangeExplicitDebugNode>  (this));
	factoryByNodeType_->insert("String",         new DebugNodeFactory<StringDebugNode>         (this));
	factoryByNodeType_->insert("Keyword",        new DebugNodeFactory<KeywordDebugNode>        (this));
	factoryByNodeType_->insert("Repeat",         new DebugNodeFactory<RepeatDebugNode>         (this));
	factoryByNodeType_->insert("LazyRepeat",     new DebugNodeFactory<LazyRepeatDebugNode>     (this));
	factoryByNodeType_->insert("GreedyRepeat",   new DebugNodeFactory<GreedyRepeatDebugNode>   (this));
	factoryByNodeType_->insert("Length",         new DebugNodeFactory<LengthDebugNode>         (this));
	factoryByNodeType_->insert("Eoi",            new DebugNodeFactory<EoiDebugNode>            (this));
	factoryByNodeType_->insert("Boi",            new DebugNodeFactory<BoiDebugNode>            (this));
	factoryByNodeType_->insert("Pass",           new DebugNodeFactory<PassDebugNode>           (this));
	factoryByNodeType_->insert("Find",           new DebugNodeFactory<FindDebugNode>           (this));
	factoryByNodeType_->insert("Ahead",          new DebugNodeFactory<AheadDebugNode>          (this));
	factoryByNodeType_->insert("Behind",         new DebugNodeFactory<BehindDebugNode>         (this));
	factoryByNodeType_->insert("Choice",         new DebugNodeFactory<ChoiceDebugNode>         (this));
	factoryByNodeType_->insert("Glue",           new DebugNodeFactory<GlueDebugNode>           (this));
	factoryByNodeType_->insert("Hint",           new DebugNodeFactory<HintDebugNode>           (this));
	factoryByNodeType_->insert("Ref",            new DebugNodeFactory<RefDebugNode>            (this));
	factoryByNodeType_->insert("Inline",         new DebugNodeFactory<InlineDebugNode>         (this));
	factoryByNodeType_->insert("Previous",       new DebugNodeFactory<PreviousDebugNode>       (this));
	factoryByNodeType_->insert("Call",           new DebugNodeFactory<CallDebugNode>           (this));
	factoryByNodeType_->insert("Set",            new DebugNodeFactory<SetDebugNode>            (this));
	factoryByNodeType_->insert("If",             new DebugNodeFactory<IfDebugNode>             (this));
	factoryByNodeType_->insert("Capture",        new DebugNodeFactory<CaptureDebugNode>        (this));
	factoryByNodeType_->insert("Replay",         new DebugNodeFactory<ReplayDebugNode>         (this));
	factoryByNodeType_->insert("Invoke",         new DebugNodeFactory<InvokeDebugNode>         (this));
}

void Debugger::printDefinition(bool omitUnusedRules)
{
	if (omitUnusedRules)
		determineRulesInUse(DebugFactory::definition()->rule());

	typedef DefinitionNode::RuleByName RuleByName;
	typedef syntax::RuleNode RuleNode;
	Ref<RuleByName> ruleByName = DebugFactory::definition()->ruleByName_;

	typedef Map<int, Ref<RuleNode> > RuleById;
	Ref<RuleById, Owner> ruleById = RuleById::create();

	for (RuleByName::Index i = ruleByName->first(); ruleByName->has(i); ++i) {
		Ref<RuleNode> rule = ruleByName->value(i);
		ruleById->insert(rule->id(), rule);
	}

	for (int i = 0; i < ruleById->length(); ++i) {
		Ref<RuleNode> rule = ruleById->get(i).value();
		if (omitUnusedRules && !rule->used()) continue;
		print("DEFINE%%(\"%%\",\n", rule->isVoid() ? "_VOID" : "", rule->name());
		if (rule->entry()) {
			Ref<DebugNode> debugNode = rule->entry();
			if (debugNode) debugNode->printNext(indent_);
		}
		else {
			print("%%PASS()", indent_);
		}
		print("\n);\n\n");
	}

	print("ENTRY(\"%%\");\n", DebugFactory::definition()->ruleName_);
}

Node *Debugger::produce(Node *newNode, const char *nodeType)
{
	Ref<NodeFactory, Owner> factory;
	return factoryByNodeType_->lookup(nodeType, &factory) ? factory->produce(newNode) : newNode;
}

Ref<Debugger::StateNameById, Owner> Debugger::newReverseMap(Ref<StateIdByName> stateIdByName)
{
	Ref<StateNameById, Owner> stateNameById = StateNameById::create();
	for (StateIdByName::Index i = stateIdByName->first(); stateIdByName->has(i); ++i) {
		String name = stateIdByName->key(i);
		int id = stateIdByName->value(i);
		stateNameById->insert(id, name);
	}
	return stateNameById;
}

Ref<Debugger::StateNameById> Debugger::flagNameById() {
	if (!flagNameById_)
		flagNameById_ = newReverseMap(this->definition()->flagIdByName_);
	return flagNameById_;
}

Ref<Debugger::StateNameById> Debugger::captureNameById() {
	if (!captureNameById_)
		captureNameById_ = newReverseMap(this->definition()->captureIdByName_);
	return captureNameById_;
}

void Debugger::determineRulesInUse(Ref<syntax::RuleNode> rule)
{
	if (!rule->used()) {
		rule->markUsed();
		Ref<Node> node = rule->entry()->first();
		while (node) {
			Ref<syntax::LinkNode> link = node;
			if (link)
				determineRulesInUse(link->rule());
			node = node->next();
		}
	}
}

} // namespace syntax

} // namespace ftl
