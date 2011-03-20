/*
 * SyntaxDebugger.hpp -- debugging syntax descriptions
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SYNTAXDEBUGGER_HPP
#define FTL_SYNTAXDEBUGGER_HPP

#include "Syntax.hpp"
#include "String.hpp"
#include "Map.hpp"
#include "PrintDebug.hpp"

namespace ftl
{

template<class Media>
class SyntaxDebugger: public Syntax<Media>::DebugFactory
{
public:
	typedef typename Syntax<Media>::DebugFactory DebugFactory;
	typedef typename Syntax<Media>::Definition Definition;
	typedef typename Syntax<Media>::Node Node;

	SyntaxDebugger()
		: factoryByNodeType_(new FactoryByNodeType),
		  indent_("  ")
	{
		factoryByNodeType_->insert("Char",          new DebugNodeFactory<CharDebugNode>         (this));
		factoryByNodeType_->insert("Any",           new DebugNodeFactory<AnyDebugNode>          (this));
		factoryByNodeType_->insert("RangeMinMax",   new DebugNodeFactory<RangeMinMaxDebugNode>  (this));
		factoryByNodeType_->insert("RangeExplicit", new DebugNodeFactory<RangeExplicitDebugNode>(this));
		factoryByNodeType_->insert("String",        new DebugNodeFactory<StringDebugNode>       (this));
		factoryByNodeType_->insert("Keyword",       new DebugNodeFactory<KeywordDebugNode>      (this));
		factoryByNodeType_->insert("Repeat",        new DebugNodeFactory<RepeatDebugNode>       (this));
		factoryByNodeType_->insert("Length",        new DebugNodeFactory<LengthDebugNode>       (this));
		factoryByNodeType_->insert("Eoi",           new DebugNodeFactory<EoiDebugNode>          (this));
		factoryByNodeType_->insert("Boi",           new DebugNodeFactory<BoiDebugNode>          (this));
		factoryByNodeType_->insert("Pass",          new DebugNodeFactory<PassDebugNode>         (this));
		factoryByNodeType_->insert("Find",          new DebugNodeFactory<FindDebugNode>         (this));
		factoryByNodeType_->insert("Ahead",         new DebugNodeFactory<AheadDebugNode>        (this));
		factoryByNodeType_->insert("Choice",        new DebugNodeFactory<ChoiceDebugNode>       (this));
		factoryByNodeType_->insert("Glue",          new DebugNodeFactory<GlueDebugNode>         (this));
		factoryByNodeType_->insert("Ref",           new DebugNodeFactory<RefDebugNode>          (this));
		factoryByNodeType_->insert("Inline",        new DebugNodeFactory<InlineDebugNode>       (this));
		factoryByNodeType_->insert("Previous",      new DebugNodeFactory<PreviousDebugNode>     (this));
		factoryByNodeType_->insert("Call",          new DebugNodeFactory<CallDebugNode>         (this));
		factoryByNodeType_->insert("Set",           new DebugNodeFactory<SetDebugNode>          (this));
		factoryByNodeType_->insert("If",            new DebugNodeFactory<IfDebugNode>           (this));
		factoryByNodeType_->insert("GetChar",       new DebugNodeFactory<GetCharDebugNode>      (this));
		factoryByNodeType_->insert("SetChar",       new DebugNodeFactory<SetCharDebugNode>      (this));
		factoryByNodeType_->insert("VarChar",       new DebugNodeFactory<VarCharDebugNode>      (this));
		factoryByNodeType_->insert("GetString",     new DebugNodeFactory<GetStringDebugNode>    (this));
		factoryByNodeType_->insert("SetString",     new DebugNodeFactory<SetStringDebugNode>    (this));
		factoryByNodeType_->insert("VarString",     new DebugNodeFactory<VarStringDebugNode>    (this));
		factoryByNodeType_->insert("Invoke",        new DebugNodeFactory<InvokeDebugNode>       (this));
	}
	
private:
	static void determineRulesInUse(Ref<typename Definition::RuleNode> rule)
	{
		if (!rule->used()) {
			rule->markUsed();
			Ref<Node> node = rule->entry()->first();
			while (node) {
				Ref<typename Definition::LinkNode> link = node;
				if (link)
					determineRulesInUse(link->rule());
				node = node->next();
			}
		}
	}
	
public:
	void printDefinition(bool omitUnusedRules = false)
	{
		if (omitUnusedRules)
			determineRulesInUse(DebugFactory::definition()->rule());
		
		if (DebugFactory::definition()->stateful()) {
			typedef typename Definition::StateFlag StateFlag;
			typedef typename Definition::StateChar StateChar;
			typedef typename Definition::StateString StateString;
			
			Ref<StateFlag> stateFlag = DebugFactory::definition()->stateFlagHead_;
			for (int id = DebugFactory::definition()->numStateFlags_ - 1; id >= 0; --id) {
				print("STATE_FLAG(\"%%\", %%);\n", flagNameById()->get(id), stateFlag->defaultValue_);
				stateFlag = stateFlag->next_;
			}
			
			Ref<StateChar> stateChar = DebugFactory::definition()->stateCharHead_;
			for (int id = DebugFactory::definition()->numStateChars_ - 1; id >= 0; --id) {
				print("STATE_CHAR(\"%%\", ", charNameById()->get(id));
				printCharAttr(stateChar->defaultValue_);
				print(")\n");
				stateChar = stateChar->next_;
			}
			
			Ref<StateString> stateString = DebugFactory::definition()->stateStringHead_;
			for (int id = DebugFactory::definition()->numStateStrings_ - 1; id >= 0; --id) {
				print("STATE_STRING(\"%%\", ", stringNameById()->get(id));
				printStringAttr(stateString->defaultValue_);
				print(")\n");
				stateString = stateString->next_;
			}
			
			print("\n");
		}
		
		typedef typename Definition::RuleByName RuleByName;
		typedef typename Syntax<Media>::RuleNode RuleNode;
		Ref<RuleByName> ruleByName = DebugFactory::definition()->ruleByName_;
		
		typedef Map<int, Ref<RuleNode> > RuleById;
		Ref<RuleById, Owner> ruleById = new RuleById;
		
		for (typename RuleByName::Index i = ruleByName->first(); ruleByName->def(i); ++i) {
			Ref<RuleNode> rule = ruleByName->value(i);
			ruleById->insert(rule->id(), rule);
		}
		
		typedef List< Ref<RuleNode> > RuleList;
		Ref<RuleList, Owner> ruleList = ruleById->valueList();
		
		for (typename RuleList::Index i = ruleList->first(); ruleList->def(i); ++i) {
			Ref<RuleNode> rule = ruleList->get(i);
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
	
private:
	class NodeFactory;
	
public:
	virtual Node* produce(Node* newNode, const char* nodeType)
	{
		Ref<NodeFactory, Owner> factory;
		return factoryByNodeType_->lookup(nodeType, &factory) ? factory->produce(newNode) : newNode;
	}
	
private:
	typedef typename Media::Item Char;
	typedef typename Media::Index Index;
	typedef typename Syntax<Media>::State State;
	
	class NodeFactory: public Instance {
	public:
		virtual Node* produce(Node* newNode) = 0;
	};
	
	template<class DebugNode>
	class DebugNodeFactory: public NodeFactory {
	public:
		DebugNodeFactory(Ref<SyntaxDebugger> debugger)
			: debugger_(debugger)
		{}
		virtual Node* produce(Node* newNode) {
			return new DebugNode(debugger_, newNode);
		}
	private:
		Ref<SyntaxDebugger> debugger_;
	};
	
	static void printCharAttr(Char ch) {
		print("'");
		if (ch == '\'')
			print("\\");
		printChar(ch);
		print("'");
	}
	
	static void printStringAttr(const Array<Char>& s) {
		print("\"");
		printString(s);
		print("\"");
	}
	
	static void printString(const Array<Char>& s) {
		printString(s.data(), s.size());
	}
	
	static void printString(const Array<Char, DeepCopyZeroTerminatedArray>& s) {
		printString(s.data(), s.size());
	}
	
	static void printString(Char* s, int size) {
		for (int i = 0; i < size; ++i) {
			Char ch = s[i];
			if (ch == '"')
				print("\\");
			printChar(s[i]);
		}
	}
	
	static void printChar(Char ch) {
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
				print("\\%oct%", int(ToAscii<Char>::map(ch)));
		}
		else if (ch < 127) {
			if (ch == '\\')
				print("\\");
			print("%%", ToAscii<Char>::map(ch));
		}
		else if (ch < 256) {
			print("\\%oct%", ToUnicode<Char>::map(ch));
		}
		else {
			print("\\x%hex%", ToUnicode<Char>::map(ch));
		}
	}
	
public:	
	class DebugNode: public Node {
	public:
		DebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: debugger_(debugger)
		{
			appendChild(newNode);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state) {
			return entry()->matchNext(media, i, tokenFactory, parentToken, state);
		}
		
		virtual const char* declType() const = 0;
		virtual void printAttributes(String indent) {}
		
		virtual void printNext(String indent = "") {
			print("%%%%(", indent, declType());
			printAttributes((StringList() << indent << debugger_->indent_).join());
			print(")");
		}
		
		inline Ref<Node> entry() const { return Node::firstChild(); }
		
	protected:
		void printBranch(Ref<Node> node, String indent) {
			if (node) {
				Ref<DebugNode> debugNode = node;
				if (debugNode) debugNode->printNext(indent);
				else print(indent);
			}
			else {
				print("%%PASS()", indent);
			}
		}
		
		String superIndent(String indent) const {
			return (indent != "") ? indent->copy(0, indent->size() - debugger_->indent_->size()) : indent;
		}
		
		String subIndent(String indent) const {
			return (StringList() << indent << DebugNode::debugger_->indent_).join();
		}
		
		Ref<SyntaxDebugger> debugger_;
	};
	
	class CharDebugNode: public DebugNode {
	public:
		CharDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return charNode()->invert() ? "OTHER" : "CHAR"; }
		
		virtual void printAttributes(String indent) {
			printCharAttr(charNode()->ch());
		}
		
	private:
		typedef typename Syntax<Media>::CharNode CharNode;
		inline Ref<CharNode> charNode() const { return DebugNode::entry(); }
	};
	
	class AnyDebugNode: public DebugNode {
	public:
		AnyDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "ANY"; }
	};
	
private:
	class RangeMinMaxDebugNode: public DebugNode {
	public:
		RangeMinMaxDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return rangeMinMaxNode()->invert() ? "EXCEPT" : "RANGE"; }
		
		virtual void printAttributes(String indent) {
			Ref<RangeMinMaxNode> node = rangeMinMaxNode();
			printCharAttr(node->a());
			print(", ");
			printCharAttr(node->b());
		}
		
	private:
		typedef typename Syntax<Media>::RangeMinMaxNode RangeMinMaxNode;
		inline Ref<RangeMinMaxNode> rangeMinMaxNode() const { return DebugNode::entry(); }
	};
	
	class RangeExplicitDebugNode: public DebugNode {
	public:
		RangeExplicitDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return rangeExplicitNode()->invert() ? "EXCEPT" : "RANGE"; }
		
		virtual void printAttributes(String indent) {
			printStringAttr(rangeExplicitNode()->s());
		}
		
	private:
		typedef typename Syntax<Media>::RangeExplicitNode RangeExplicitNode;
		inline Ref<RangeExplicitNode> rangeExplicitNode() const { return DebugNode::entry(); }
	};
	
	class StringDebugNode: public DebugNode {
	public:
		StringDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "STRING"; }
		
		virtual void printAttributes(String indent) {
			printStringAttr(stringNode()->s());
		}
		
	private:
		typedef typename Syntax<Media>::StringNode StringNode;
		inline Ref<StringNode> stringNode() const { return DebugNode::entry(); }
	};
	
	class KeywordDebugNode: public DebugNode {
	public:
		KeywordDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "KEYWORD"; }
		
		virtual void printAttributes(String indent) {
			print("\n%%\"", indent);
			Ref<KeywordMap> map = keywordNode()->map();
			for (typename KeywordMap::Index index = map->first(); map->def(index); ++index) {
				Ref<typename KeywordMap::Key, Owner> s = map->key(index);
				printString(*s);
				if (index != map->last())
					print(" ");
			}
			print("\"\n%%", DebugNode::superIndent(indent));
		}
		
	private:
		typedef typename Syntax<Media>::KeywordNode KeywordNode;
		typedef typename Syntax<Media>::KeywordMap KeywordMap;
		inline Ref<KeywordNode> keywordNode() const { return DebugNode::entry(); }
	};
	
	class RepeatDebugNode: public DebugNode {
	public:
		RepeatDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "REPEAT"; }
		
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
		typedef typename Syntax<Media>::RepeatNode RepeatNode;
		inline Ref<RepeatNode> repeatNode() const { return DebugNode::entry(); }
	};
	
	class LengthDebugNode: public DebugNode {
	public:
		LengthDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "LENGTH"; }
		
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
		typedef typename Syntax<Media>::LengthNode LengthNode;
		inline Ref<LengthNode> lengthNode() const { return DebugNode::entry(); }
	};
	
	class BoiDebugNode: public DebugNode {
	public:
		BoiDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "BOI"; }
	};
	
	class EoiDebugNode: public DebugNode {
	public:
		EoiDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "EOI"; }
	};
	
	class PassDebugNode: public DebugNode {
	public:
		PassDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return passNode()->invert() ? "FAIL" : "PASS"; }
	
	private:
		typedef typename Syntax<Media>::PassNode PassNode;
		inline Ref<PassNode> passNode() const { return DebugNode::entry(); }
	};
	
	class FindDebugNode: public DebugNode {
	public:
		FindDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "FIND"; }
		
		virtual void printAttributes(String indent) {
			print("\n");
			printBranch(findNode()->entry(), indent);
			print("\n%%", DebugNode::superIndent(indent));
		}
		
	private:
		typedef typename Syntax<Media>::FindNode FindNode;
		inline Ref<FindNode> findNode() const { return DebugNode::entry(); }
	};
	
	class AheadDebugNode: public DebugNode {
	public:
		AheadDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return aheadNode()->invert() ? "NOT" : "AHEAD"; }
		
		virtual void printAttributes(String indent) {
			print("\n");
			printBranch(aheadNode()->entry(), indent);
			print("\n%%", DebugNode::superIndent(indent));
		}
		
	private:
		typedef typename Syntax<Media>::AheadNode AheadNode;
		inline Ref<AheadNode> aheadNode() const { return DebugNode::entry(); }
	};
	
	class ChoiceDebugNode: public DebugNode {
	public:
		ChoiceDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "CHOICE"; }
		
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
		typedef typename Syntax<Media>::ChoiceNode ChoiceNode;
		inline Ref<ChoiceNode> choiceNode() const { return DebugNode::entry(); }
	};
	
	class GlueDebugNode: public DebugNode {
	public:
		GlueDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "GLUE"; }
		
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
		typedef typename Syntax<Media>::GlueNode GlueNode;
		inline Ref<GlueNode> glueNode() const { return DebugNode::entry(); }
	};
	
	class RefDebugNode: public DebugNode {
	public:
		RefDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return (inlineNode()) ? "INLINE" : "REF"; }
		
		virtual void printAttributes(String indent) {
			print("\"%%\"", linkNode()->ruleName());
		}
		
	private:
		typedef typename Syntax<Media>::LinkNode LinkNode;
		inline Ref<LinkNode> linkNode() const { return DebugNode::entry(); }
		
		typedef typename Syntax<Media>::InlineNode InlineNode;
		inline Ref<InlineNode> inlineNode() const { return DebugNode::entry(); }
	};
	
	class InlineDebugNode: public DebugNode {
	public:
		InlineDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "INLINE"; }
		
		virtual void printAttributes(String indent) {
			print("\"%%\"", inlineNode()->ruleName());
		}
		
	private:
		typedef typename Syntax<Media>::InlineNode InlineNode;
		inline Ref<InlineNode> inlineNode() const { return DebugNode::entry(); }
	};
	
	class PreviousDebugNode: public DebugNode {
	public:
		PreviousDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "PREVIOUS"; }
		
		virtual void printAttributes(String indent) {
			Ref<PreviousNode> node = previousNode();
			print("\"%%\"", node->ruleName());
			if (node->keywordName())
				print(", \"%%\"", node->keywordName());
		}
		
	private:
		typedef typename Syntax<Media>::PreviousNode PreviousNode;
		inline Ref<PreviousNode> previousNode() const { return DebugNode::entry(); }
	};
	
	class CallDebugNode: public DebugNode {
	public:
		CallDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "CALL"; }
		
		virtual void printAttributes(String indent) {
			print("0x%hex:8.:'0'", (void*)callNode()->callBack());
		}
		
	private:
		typedef typename Syntax<Media>::CallNode CallNode;
		inline Ref<CallNode> callNode() const { return DebugNode::entry(); }
	};
	
	class SetDebugNode: public DebugNode {
	public:
		SetDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "SET"; }
		
		virtual void printAttributes(String indent) {
			Ref<SetNode> node = setNode();
			print("\"%%\", %%", DebugNode::debugger_->flagNameById()->get(node->flagId()), node->value());
		}
		
	private:
		typedef typename Syntax<Media>::SetNode SetNode;
		inline Ref<SetNode> setNode() const { return DebugNode::entry(); }
	};
	
	class IfDebugNode: public DebugNode {
	public:
		IfDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "IF"; }
		
		virtual void printAttributes(String indent) {
			Ref<IfNode> node = ifNode();
			print("\"%%\",\n", DebugNode::debugger_->flagNameById()->get(node->flagId()));
			printBranch(node->trueBranch(), indent);
			print(",\n");
			printBranch(node->falseBranch(), indent);
			print("\n%%", DebugNode::superIndent(indent));
		}
	
	private:
		typedef typename Syntax<Media>::IfNode IfNode;
		inline Ref<IfNode> ifNode() const { return DebugNode::entry(); }
	};
	
	class GetCharDebugNode: public DebugNode {
	public:
		GetCharDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "GETCHAR"; }
		
		virtual void printAttributes(String indent) {
			print("\"%%\"", DebugNode::debugger_->charNameById()->get(getCharNode()->charId()));
		}
		
	private:
		typedef typename Syntax<Media>::GetCharNode GetCharNode;
		inline Ref<GetCharNode> getCharNode() const { return DebugNode::entry(); }
	};
	
	class SetCharDebugNode: public DebugNode {
	public:
		SetCharDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "SETCHAR"; }
		
		virtual void printAttributes(String indent) {
			Ref<SetCharNode> node = setCharNode();
			print("\"%%\", ", DebugNode::debugger_->charNameById()->get(node->charId()));
			printCharAttr(node->value());
		}
		
	private:
		typedef typename Syntax<Media>::SetCharNode SetCharNode;
		inline Ref<SetCharNode> setCharNode() const { return DebugNode::entry(); }
	};
	
	class VarCharDebugNode: public DebugNode {
	public:
		VarCharDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return varCharNode()->invert() ? "VAROTHER" : "VARCHAR"; }
		
		virtual void printAttributes(String indent) {
			print("\"%%\"", DebugNode::debugger_->charNameById()->get(varCharNode()->charId()));
		}
		
	private:
		typedef typename Syntax<Media>::VarCharNode VarCharNode;
		inline Ref<VarCharNode> varCharNode() const { return DebugNode::entry(); }
	};
	
	class GetStringDebugNode: public DebugNode {
	public:
		GetStringDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "GETSTRING"; }
		
		virtual void printAttributes(String indent) {
			Ref<GetStringNode> node = getStringNode();
			print("\"%%\",\n", DebugNode::debugger_->stringNameById()->get(node->stringId()));
			printBranch(node->coverage(), indent);
			print("\n%%", DebugNode::superIndent(indent));
		}
		
	private:
		typedef typename Syntax<Media>::GetStringNode GetStringNode;
		inline Ref<GetStringNode> getStringNode() const { return DebugNode::entry(); }
	};
	
	class SetStringDebugNode: public DebugNode {
	public:
		SetStringDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "SETSTRING"; }
		
		virtual void printAttributes(String indent) {
			Ref<SetStringNode> node = setStringNode();
			print("\"%%\", ", DebugNode::debugger_->stringNameById()->get(node->stringId()));
			printString(node->value());
		}
		
	private:
		typedef typename Syntax<Media>::SetStringNode SetStringNode;
		inline Ref<SetStringNode> setStringNode() const { return DebugNode::entry(); }
	};
	
	class VarStringDebugNode: public DebugNode {
	public:
		VarStringDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "VARSTRING"; }
		
		virtual void printAttributes(String indent) {
			print("\"%%\"", DebugNode::debugger_->stringNameById()->get(varStringNode()->stringId()));
		}
		
	private:
		typedef typename Syntax<Media>::VarStringNode VarStringNode;
		inline Ref<VarStringNode> varStringNode() const { return DebugNode::entry(); }
	};
	
	class InvokeDebugNode: public DebugNode {
	public:
		InvokeDebugNode(Ref<SyntaxDebugger> debugger, Ref<Node> newNode)
			: DebugNode(debugger, newNode)
		{}
		
		virtual const char* declType() const { return "INVOKE"; }
		
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
		typedef typename Syntax<Media>::InvokeNode InvokeNode;
		inline Ref<InvokeNode> invokeNode() const { return DebugNode::entry(); }
	};
	
	friend class Syntax<Media>::Definition;
	friend class DebugNode;
	
	typedef PrefixTree< char, Ref<NodeFactory, Owner> > FactoryByNodeType;
	Ref<FactoryByNodeType, Owner> factoryByNodeType_;
	
	String indent_;
	
	typedef typename Syntax<Media>::Definition::StateIdByName StateIdByName;
	typedef Map<int, String> StateNameById;
	
	Ref<StateNameById, Owner> flagNameById_;
	Ref<StateNameById, Owner> charNameById_;
	Ref<StateNameById, Owner> stringNameById_;
	
	Ref<StateNameById, Owner> newReverseMap(Ref<StateIdByName> stateIdByName)
	{
		Ref<StateNameById, Owner> stateNameById = new StateNameById;
		for (typename StateIdByName::Index i = stateIdByName->first(); stateIdByName->def(i); ++i) {
			String name = stateIdByName->key(i);
			int id = stateIdByName->value(i);
			stateNameById->insert(id, name);
		}
		return stateNameById;
	}
	
	Ref<StateNameById> flagNameById() {
		if (!flagNameById_)
			flagNameById_ = newReverseMap(this->definition()->flagIdByName());
		return flagNameById_;
	}
	
	Ref<StateNameById> charNameById() {
		if (!charNameById_)
			charNameById_ = newReverseMap(this->definition()->charIdByName());
		return charNameById_;
	}
	
	Ref<StateNameById> stringNameById() {
		if (!stringNameById_)
			stringNameById_ = newReverseMap(this->definition()->stringIdByName());
		return stringNameById_;
	}
};

} // namespace ftl

#endif // FTL_SYNTAXDEBUGGER_HPP
