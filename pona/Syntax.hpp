/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_SYNTAX_HPP
#define PONA_SYNTAX_HPP

#include "Atoms.hpp"
#include "LineSink.hpp"
#include "LinePrinter.hpp"
#include "PrefixTree.hpp"
#include "Token.hpp"
#include "TokenFactory.hpp"

#ifdef ECHO
#undef ECHO
#endif

namespace pona
{

template<class Media>
class Syntax: public Instance
{
protected:
	typedef typename Media::Element Char;
	typedef Syntax Node;
	typedef PrefixTree<Char,int> KeywordMap;
	
	Syntax(Ref<Node> next = 0)
		: next_(next)
	{}
	
	int type_;
	Ref<Node, Owner> next_;
	
public:
	class State;
	
protected:
	virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state) = 0;
	
	class CharNode: public Node
	{
	public:
		CharNode(Char ch, int invert, Ref<Node> next)
			: Node(next),
			  ch_(ch),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
			{
				Char ch = media->get(i++);
				if ((ch != ch_) ^ invert_)
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Char ch_;
		int invert_;
	};
	
	class AnyNode: public Node
	{
	public:
		AnyNode(Ref<Node> next)
			: Node(next)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
			{
				++i;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class RangeNode: public Node
	{
	public:
		RangeNode(Char a, Char b, Ref<Node> next)
			: Node(next),
			  a_(a),
			  b_(b)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length()) {
				Char ch = media->get(i++);
				if ((ch < a_) || (b_ < ch))
					i = -1;
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Char a_, b_;
	};
	
	class StringNode: public Node
	{
	public:
		template<class Char2>
		StringNode(const Char2* s, int len, Ref<Node> next)
			: Node(next),
			  s_(new Char[len+1]),
			  len_(len)
		{
			for (int i = 0; i < len; ++i)
				s_[i] = s[i];
			s_[len] = 0;
		}
		
		template<class Char2>
		StringNode(const Char2* s, Ref<Node> next)
			: Node(next)
		{
			len_ = 0;
			while (*(s + len_)) ++len_;
			s_ = new Char[len_+1];
			for (int i = 0; i < len_; ++i)
				s_[i] = s[i];
			s_[len_] = 0;
		}
		
		~StringNode()
		{
			delete[] s_;
			s_ = 0;
		}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int n = media->length();
			
			if (i < n) {
				int k = 0;
				while ((k < len_) && (i < n)) {
					Char ch = media->get(i++);
					if (s_[k] != ch) break;
					++k;
				}
				if (k != len_)
					i = -1;
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Char* s_;
		int len_;
	};
	
	class KeywordNode: public Node
	{
	public:
		KeywordNode(Ref<KeywordMap> map, Ref<Node> next)
			: Node(next),
			  map_(map)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length()) {
				int h = 0;
				int tokenType = -1;
				if (map_->lookup(media, i, &h, &tokenType)) {
					parentToken->setType(tokenType);
					i = h;
				}
				else
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<KeywordMap, Owner> map_;
	};
	
	class RepeatNode: public Node
	{
	public:
		RepeatNode(int minRepeat, int maxRepeat, Ref<Node> entry, Ref<Node> next)
			: Node(next),
			  minRepeat_(minRepeat),
			  maxRepeat_(maxRepeat),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int repeatCount = 0;
			int h = i;
			while ((repeatCount < maxRepeat_) && (h != -1))
			{
				h = entry_->matchNext(media, h, tokenFactory, parentToken, state);
				if (h != -1) {
					i = h;
					++repeatCount;
				}
			}
			if ((repeatCount < minRepeat_) || (maxRepeat_ < repeatCount))
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int minRepeat_;
		int maxRepeat_;
		Ref<Node, Owner> entry_;
	};
	
	class EoiNode: public Node
	{
	public:
		EoiNode(Ref<Node> next)
			: Node(next)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i != media->length())
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class NotNode: public Node
	{
	public:
		NotNode(Ref<Node> entry, Ref<Node> next)
			: Node(next),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = 0;
			if (entry_)
				h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
			if (i < media->length())
				++i;
			if (h != -1)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
	};
	
	class FindNode: public Node
	{
	public:
		FindNode(Ref<Node> entry, Ref<Node> next)
			: Node(next),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			bool found = false;
			int n = media->length();
			while (i <= n) {
				int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
				if (h != -1) {
					found = true;
					i = h;
					break;
				}
				++i;
			}
			if (!found)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
	};
	
	class OrNode: public Node
	{
	public:
		OrNode(Ref<Node> firstChoice, Ref<Node> secondChoice, Ref<Node> next)
			: Node(next),
			  firstChoice_(firstChoice),
			  secondChoice_(secondChoice)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = firstChoice_->matchNext(media, i, tokenFactory, parentToken, state);
			if (h == -1)
				h = secondChoice_->matchNext(media, i, tokenFactory, parentToken, state);
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> firstChoice_;
		Ref<Node, Owner> secondChoice_;
	};
	
	class LookAheadNode: public Node
	{
	public:
		LookAheadNode(Ref<Node> entry, Ref<Node> next)
			: Node(next),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
			if (h == -1)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
	};
	
	class LengthNode: public Node
	{
	public:
		LengthNode(int minLength, int maxLength, Ref<Node> entry, Ref<Node> next)
			: Node(next),
			  minLength_(minLength),
			  maxLength_(maxLength),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
			if (h != -1) {
				int d = h - i;
				if ((d < minLength_) || (maxLength_ < d))
					h = -1;
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int minLength_;
		int maxLength_;
		Ref<Node, Owner> entry_;
	};
	
public:
	class Definition;
	
private:
	class InlineNode;
	
	class RuleNode: public Node
	{
	public:
		RuleNode(Definition* definition)
			: definition_(definition),
			  name_("undefined"),
			  tokenType_(-1),
			  isVoid_(false)
		{}
		
		RuleNode(Definition* definition, const char* name, int tokenType, Ref<Node> entry, bool isVoid = false)
			: definition_(definition),
			  name_(name),
			  tokenType_(tokenType),
			  entry_(entry),
			  isVoid_(isVoid)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token, Owner> token = tokenFactory->produce();
			
			int h = entry_->matchNext(media, i, tokenFactory, token, state);
			
			if (h != -1) {
				if (isVoid_)
					parentToken->appendAllChildrenOf(token);
				else
					token->join(parentToken, name_, definition_->language(), tokenType_, i, h);
			}
			
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
		inline int tokenType() const { return tokenType_; }
		
	protected:
		friend class InlineNode;
		
		Definition* definition_;
		const char* name_;
		int tokenType_;
		Ref<Node, Owner> entry_;
		bool isVoid_;
	};
	
	class RefNode: public Node
	{
	public:
		RefNode(Definition* definition, const char* ruleName, Ref<Node> next)
			: Node(next),
			  definition_(definition),
			  ruleName_(ruleName)
		{}
		
		inline const char* ruleName() const { return ruleName_; }
		inline Ref<RuleNode> rule() const { return rule_; }
		
		inline void link() {
			rule_ = definition_->ruleByName(ruleName_);
		}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Token tempToken;
			tempToken.liberate();
			
			if (!rule_)
				link();
			
			int h = rule_->matchNext(media, i, tokenFactory, &tempToken, state);
			
			if ((h != -1) && (next_))
				h = next_->matchNext(media, h, tokenFactory, &tempToken, state);
			
			if (h != -1) {
				parentToken->appendAllChildrenOf(&tempToken);
				i = h;
			}
			else
				i = -1;
			
			return i;
		}
		
	private:
		Definition* definition_;
		const char* ruleName_;
		Ref<RuleNode> rule_;
	};
	
	class InlineNode: public Node
	{
	public:
		InlineNode(Definition* definition, const char* ruleName, Ref<Node> next)
			: Node(next),
			  definition_(definition),
			  ruleName_(ruleName)
		{}
		
		inline const char* ruleName() const { return ruleName_; }
		inline Ref<RuleNode> rule() const { return rule_; }
		
		inline void link() {
			rule_ = definition_->ruleByName(ruleName_);
		}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (!rule_)
				link();
			
			i = rule_->entry_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Definition* definition_;
		const char* ruleName_;
		Ref<RuleNode> rule_;
	};
	
public:
	class State: public Instance
	{
	public:
		State()
			: flags_(0),
			  chars_(0),
			  numFlags_(0),
			  numChars_(0)
		{}
		
		State(Ref<Definition> definition, int numFlags, int numChars = 0, Ref<State> parent = 0)
			: definition_(definition),
			  flags_((numFlags > 0) ? new bool[numFlags] : 0),
			  chars_((numChars > 0) ? new Char[numChars] : 0),
			  numFlags_(numFlags),
			  numChars_(numChars)
		{
			if (parent)
				parent->child_ = this;
		}
		
		~State()
		{
			if (flags_) { delete[] flags_; flags_ = 0; }
			if (chars_) { delete[] chars_; chars_ = 0; }
		}
		
		inline Ref<Definition> definition() const { return definition_; }
		
		inline bool* flag(int id) const { return flags_ + id; }
		inline Char* character(int id) const { return chars_ + id; }
		
		inline Ref<State> child() const { return child_; }
		inline void setChild(Ref<State> state) { child_ = state; }
		
		bool equals(const State& b) const
		{
			bool equal = (definition_ == b.definition_) && (numFlags_ == b.numFlags_) && (numChars_ == b.numChars_);
			for (int i = 0; (i < numFlags_) && equal; ++i)
				equal = equal && (flags_[i] == b.flags_[i]);
			for (int i = 0; (i < numChars_) && equal; ++i)
				equal = equal && (chars_[i] == b.chars_[i]);
			if (equal) {
				if (child_) {
					if (b.child_) {
						if (child_ != b.child_)
							equal = equal && child_->equals(*b.child_);
					}
					else
						equal = false;
				}
				else {
					equal = equal && (!b.child_);
				}
			}
			return equal;
		}
		
		void copy(const State& b)
		{
			definition_ = b.definition_;
			if (numFlags_ != b.numFlags_) {
				numFlags_ = b.numFlags_;
				if (flags_) { delete[] flags_; flags_ = 0; }
				if (numFlags_ > 0) flags_ = new bool[numFlags_];
			}
			if (numChars_ != b.numChars_) {
				numChars_ = b.numChars_;
				if (chars_) { delete[] chars_; chars_ = 0; }
				if (numChars_ > 0) chars_ = new Char[numChars_];
			}
			for (int i = 0; i < numFlags_; ++i)
				flags_[i] = b.flags_[i];
			for (int i = 0; i < numChars_; ++i)
				chars_[i] = b.chars_[i];
			if (b.child_) {
				child_ = new State;
				child_->copy(*b.child_);
			}
			else
				child_ = 0;
		}
		
	private:
		Ref<Definition, Owner> definition_;
		bool* flags_;
		Char* chars_;
		int numFlags_;
		int numChars_;
		Ref<State, Owner> child_;
	};
	
protected:
	class SetNode: public Node
	{
	public:
		SetNode(int flagId, bool value, Ref<Node> next)
			: Node(next),
			  flagId_(flagId),
			  value_(value)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->flag(flagId_) = value_;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int flagId_;
		bool value_;
	};
	
	class IfNode: public Node
	{
	public:
		IfNode(int flagId, Ref<Node> trueBranch, Ref<Node> falseBranch, Ref<Node> next)
			: Node(next),
			  flagId_(flagId),
			  trueBranch_(trueBranch),
			  falseBranch_(falseBranch)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (*state->flag(flagId_)) {
				if (trueBranch_)
					i = trueBranch_->matchNext(media, i, tokenFactory, parentToken, state);
			}
			else {
				if (falseBranch_)
					i = falseBranch_->matchNext(media, i, tokenFactory, parentToken, state);
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int flagId_;
		Ref<Node, Owner> trueBranch_;
		Ref<Node, Owner> falseBranch_;
	};
	
	class GetCharNode: public Node
	{
	public:
		GetCharNode(int charId, Ref<Node> next)
			: Node(next),
			  charId_(charId)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
				*state->character(charId_) = media->get(i++);
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
	};
	
	class SetCharNode: public Node
	{
	public:
		SetCharNode(int charId, Char value, Ref<Node> next)
			: Node(next),
			  charId_(charId),
			  value_(value)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->character(charId_) = value_;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
		Char value_;
	};
	
	class VarCharNode: public Node
	{
	public:
		VarCharNode(int charId, int invert, Ref<Node> next)
			: Node(next),
			  charId_(charId),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length()) {
				Char ch = media->get(i++);
				if ((ch != *state->character(charId_)) ^ invert_)
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
		int invert_;
	};
	
	class InvokeNode: public Node
	{
	public:
		InvokeNode(Ref<Definition, SetNull> definition, Ref<Node> next)
			: Node(next),
			  definition_(definition)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			State* childState = 0;
			if (state) {
				childState = state->child();
				if (childState) {
					if (childState->definition() != definition_)
						childState = 0;
				}
				if (!childState)
					childState = definition_->newState(state);
			}
			
			i = definition_->matchNext(media, i, tokenFactory, parentToken, childState);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Ref<Definition, SetNull> definition_;
	};
	
	class EchoNode: public Node
	{
	public:
		EchoNode(Ref<LineSink> output, const char* message, Ref<Node> next)
			: Node(next),
			  output_(output),
			  message_(message)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			output_->write(message_);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Ref<LineSink, Owner> output_;
		const char* message_;
	};
	
public:
	class Definition: public RuleNode
	{
	public:
		Definition(int language = -1)
			: RuleNode(this),
			  language_(language),
			  numRules_(0),
			  numKeywords_(0),
			  ruleByName_(new RuleByName),
			  stateFlagByName_(new StateIdByName),
			  stateCharByName_(new StateIdByName),
			  tokenTypeByName_(new TokenTypeByName)
		{}
		
		typedef Ref<Node, Owner> NODE;
		typedef Ref<RuleNode, Owner> RULE;
		
		//-- stateless definition interface
		
		inline static NODE CHAR(Char ch, NODE next = 0) { return new CharNode(ch, 0, next); }
		inline static NODE OTHER(Char ch, NODE next = 0) { return new CharNode(ch, 1, next); }
		inline static NODE ANY(NODE next = 0) { return new AnyNode(next); }
		inline static NODE RANGE(Char a, Char b, NODE next = 0) { return new RangeNode(a, b, next); }
		inline static NODE STRING(Char* s, int len, NODE next = 0) { return new StringNode(s, len, next); }
		inline static NODE STRING(const char* s, NODE next = 0) { return new StringNode(s, next); }
		
		NODE KEYWORD(const char* keys, NODE next = 0)
		{
			Ref<KeywordMap, Owner> map = new KeywordMap;
			while (*keys) {
				if ((*keys == ' ') || (*keys == '\t')) {
					++keys;
					continue;
				}
				int n = 0;
				while (true) {
					char ch = *(keys + n);
					if ((ch == ' ') || (ch == '\t') || (ch == '\0')) break;
					++n;
				}
				map->insert(keys, n, numKeywords_);
				tokenTypeByName_->insert(keys, n, numKeywords_);
				++numKeywords_;
				keys += n;
			}
			return new KeywordNode(map, next);
		}
		inline NODE TOKEN(const char* name, NODE next = 0) {
			int tokenType = -1;
			tokenTypeByName_->lookup(name, &tokenType);
			return CHAR(tokenType);
		}
		
		inline static NODE REPEAT(int minRepeat, int maxRepeat, NODE entry, NODE next = 0) { return new RepeatNode(minRepeat, maxRepeat, entry, next); }
		inline static NODE EOI(NODE next = 0) { return new EoiNode(next); }
		inline static NODE NOT(NODE entry, NODE next = 0) { return new NotNode(entry, next); }
		inline static NODE NEVER() { return new NotNode(0, 0); }
		inline static NODE FIND(NODE entry, NODE next = 0) { return new FindNode(entry, next); }
		inline static NODE OR(NODE firstChoice, NODE secondChoice, NODE next = 0) { return new OrNode(firstChoice, secondChoice, next); }
		
		inline static NODE LOOK_AHEAD(NODE entry, NODE next = 0) { return new LookAheadNode(entry, next); }
		
		inline static NODE LENGTH(int minLength, int maxLength, NODE entry, NODE next = 0) { return new LengthNode(minLength, maxLength, entry, next); }
		
		#include "SyntaxSugar.hpp"
		
		inline RULE DEFINE(const char* name, NODE entry) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, name, numRules_++, entry);
			ruleByName_->insert(name, rule);
			return rule;
		}
		inline RULE DEFINE_VOID(const char* name, NODE entry) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, name, numRules_++, entry, true);
			ruleByName_->insert(name, rule);
			return rule;
		}
		inline void DEFINE_SELF(const char* name, Ref<Node> entry) {
			RuleNode::name_ = name;
			RuleNode::tokenType_ = numRules_++;
			RuleNode::entry_ = entry;
			ruleByName_->insert(name, this);
		}
		
		inline NODE REF(const char* ruleName, NODE next = 0) { return new RefNode(this, ruleName, next); }
		inline NODE INLINE(const char* ruleName, NODE next = 0) { return new InlineNode(this, ruleName, next); }
		
		//-- stateful definition interface
		
		inline void STATE_FLAG(const char* name, bool defaultValue = false) {
			stateFlagHead_ = new StateFlag(stateFlagHead_, name, defaultValue);
			stateFlagByName_->insert(name, stateFlagHead_->count_ - 1);
		}
		inline void STATE_CHAR(const char* name, Char defaultValue = 0) {
			stateCharHead_ = new StateChar(stateCharHead_, name, defaultValue);
			stateCharByName_->insert(name, stateCharHead_->count_ - 1);
		}
		inline NODE SET(const char* name, bool value, NODE next = 0) {
			int flagId = -1;
			stateFlagByName_->lookup(name, &flagId);
			return new SetNode(flagId, value, next);
		}
		inline NODE IF(const char* name, NODE trueBranch, NODE falseBranch = 0, NODE next = 0) {
			int flagId = -1;
			stateFlagByName_->lookup(name, &flagId);
			return new IfNode(flagId, trueBranch, falseBranch, next);
		}
		inline NODE GETCHAR(const char* name, NODE next = 0) {
			int charId = -1;
			stateCharByName_->lookup(name, &charId);
			return new GetCharNode(charId, next);
		}
		inline NODE SETCHAR(const char* name, Char value, NODE next = 0) {
			int charId = -1;
			stateCharByName_->lookup(name, &charId);
			return new SetCharNode(charId, value, next);
		}
		inline NODE VARCHAR(const char* name, NODE next = 0) {
			int charId = -1;
			stateCharByName_->lookup(name, &charId);
			return new VarCharNode(charId, 0, next);
		}
		inline NODE VAROTHER(const char* name, NODE next = 0) {
			int charId = -1;
			stateCharByName_->lookup(name, &charId);
			return new VarCharNode(charId, 1, next);
		}
		inline static NODE INVOKE(Definition* definition, NODE next = 0) {
			return new InvokeNode(definition, next);
		}
		
		//-- debugging interface
		
		inline static NODE ECHO(Ref<LineSink> output, const char* message, NODE next = 0) {
			return new EchoNode(output, message, next);
		}
		
		//-- execution interface
		
		inline int language() const { return language_; }
		inline int numRules() const { return numRules_; }
		
		State* newState(State* parent = 0)
		{
			State* state = new State(this, stateFlagHead_ ? stateFlagHead_->count_ : 0, stateCharHead_ ? stateCharHead_->count_ : 0, parent);
			Ref<StateFlag> stateFlag = stateFlagHead_;
			while (stateFlag) {
				*state->flag(stateFlag->count_ - 1) = stateFlag->defaultValue_;
				stateFlag = stateFlag->next_;
			}
			Ref<StateChar> stateChar = stateCharHead_;
			while (stateChar)  {
				*state->character(stateChar->count_ - 1) = stateChar->defaultValue_;
				stateChar = stateChar->next_;
			}
			
			return state;
		}
		
		bool find(Media* media, int* i0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, uint8_t* buf = 0, int bufSize = 0)
		{
			int i = *i0;
			int n = media->length();
			bool found = false;
			while (i < n) {
				if (match(media, i, i1, rootToken, 0, buf, bufSize)) {
					found = true;
					break;
				}
				++i;
			}
			*i0 = i;
			return found;
		}
		
		bool match(Media* media, int i0 = 0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, State* state = 0, uint8_t* buf = 0, int bufSize = 0)
		{
			if (!state) {
				if ((stateFlagHead_) || (stateCharHead_))
					state = newState();
			}
			
			TokenFactory tokenFactory(buf, bufSize);
			int h = matchNext(media, i0, &tokenFactory, 0, state);
			if (rootToken)
				*rootToken = tokenFactory.rootToken();
			
			if ((i1 != 0) && (h != -1))
				*i1 = h;
			return (h != -1);
		}
		
		Ref<Token, Owner> match(Media* media, State* state)
		{
			Ref<Token, Owner> rootToken = 0;
			int i0 = 0, i1 = 0;
			match(media, i0, &i1, &rootToken, state);
			return rootToken;
		}
		
		Ref<RuleNode, Owner> ruleByName(const char* name) 
		{
			Ref<RuleNode, Owner> node;
			ruleByName_->lookup(name, &node);
			return node;
		}
		
		int stateIdByName(const char* name)
		{
			int flagId = -1;
			stateFlagByName_->lookup(name, &flagId);
			return flagId;
		}
		
		int stateCharByName(const char* name)
		{
			int charId = -1;
			stateCharByName_->lookup(name, &charId);
			return charId;
		}
		
	private:
		int language_;
		
		class StateFlag: public Instance {
		public:
			StateFlag(Ref<StateFlag> head, const char* name, bool defaultValue = false)
				: next_(head),
				  count_((head) ? head->count_ + 1 : 1),
				  name_(name),
				  defaultValue_(defaultValue)
			{}
			Ref<StateFlag, Owner> next_;
			int count_;
			const char* name_;
			bool defaultValue_;
		};
		
		class StateChar: public Instance {
		public:
			StateChar(Ref<StateChar> head, const char* name, Char defaultValue = 0)
				: next_(head),
				  count_((head) ? head->count_ + 1 : 1),
				  name_(name),
				  defaultValue_(defaultValue)
			{}
			Ref<StateChar, Owner> next_;
			int count_;
			const char* name_;
			Char defaultValue_;
		};
		
		Ref<StateFlag, Owner> stateFlagHead_;
		Ref<StateChar, Owner> stateCharHead_;
		
		int numRules_;
		int numKeywords_;
		
		typedef PrefixTree<char, Ref<RuleNode, Owner> > RuleByName;
		Ref<RuleByName, Owner> ruleByName_;
		
		typedef PrefixTree<char, int> StateIdByName;
		Ref<StateIdByName, Owner> stateFlagByName_;
		Ref<StateIdByName, Owner> stateCharByName_;
		
		typedef PrefixTree<char, int> TokenTypeByName;
		Ref<TokenTypeByName, Owner> tokenTypeByName_;
	};
};

} // namespace pona

#endif // PONA_SYNTAX_HPP
