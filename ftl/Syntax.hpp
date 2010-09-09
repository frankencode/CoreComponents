/*
 * Syntax.hpp -- syntax description and production engine
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SYNTAX_HPP
#define FTL_SYNTAX_HPP

#include "atoms"
#include "PrefixTree.hpp"
#include "Token.hpp"
#include "TokenFactory.hpp"
#include "SyntaxState.hpp"

namespace ftl
{

template<class Media> class SyntaxLinker;

template<class Media>
class Syntax: public Instance
{
public:
	typedef typename Media::Item Char;
	typedef Syntax Node;
	typedef PrefixTree<Char,int> KeywordMap;
	
	Syntax(Ref<Node> next = 0)
		: next_(next)
	{}
	
	inline void rollBackOnFailure(int i, Token* parentToken, Token* lastChildSaved) {
		if ((parentToken) && (i == -1)) {
			while (parentToken->lastChild() != lastChildSaved)
				parentToken->lastChild()->unlink();
		}
	}
	
	Ref<Node, Owner> next_;
	
	typedef SyntaxState<Char> State;
	
	virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state) = 0;
	
	class CharNode: public Node
	{
	public:
		CharNode(Char ch, int invert)
			: ch_(ch),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i))
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
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i))
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
	
	class RangeMinMaxNode: public Node
	{
	public:
		RangeMinMaxNode(Char a, Char b, int invert)
			: a_(a),
			  b_(b),
			  invert_(invert)
		{}
		
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				if (((ch < a_) || (b_ < ch)) ^ invert_)
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
		int invert_;
	};
	
	class RangeExplicitNode: public Node
	{
	public:
		template<class Char2>
		RangeExplicitNode(const Char2* s, int invert)
			: s_(str::len(s)),
			  invert_(invert)
		{
			str::cpy(s_.data(), s, s_.size());
		}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				int k = 0, len = s_.length();
				while (k < len) {
					if (s_.get(k) == ch) break;
					++k;
				}
				if ((k == len) ^ invert_)
					i = -1;
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Array<Char> s_;
		int invert_;
	};
	
	class StringNode: public Node
	{
	public:
		template<class Char2>
		StringNode(const Char2* s, bool caseSensitive)
			: s_(str::len(s)),
			  caseSensitive_(caseSensitive)
		{
			str::cpy(s_.data(), s, s_.size());
			if (!caseSensitive) {
				for (int i = 0, n = s_.size(); i < n; ++i)
					s_.set(i, ToLower<Char>::map(s_.at(i)));
			}
		}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				int k = 0, len = s_.length();
				while ((k < len) && (media->def(i))) {
					Char ch = media->get(i++);
					if (!caseSensitive_)
						ch = ToLower<Char>::map(ch);
					if (s_.at(k) != ch) break;
					++k;
				}
				if (k != len)
					i = -1;
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Array<Char> s_;
		bool caseSensitive_;
	};
	
	class KeywordNode: public Node
	{
	public:
		KeywordNode(Ref<KeywordMap> map, bool caseSensitive)
			: map_(map),
			  caseSensitive_(caseSensitive)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				int h = 0;
				int keyword = -1;
				if (map_->match(media, i, &h, &keyword, caseSensitive_)) {
					if (parentToken)
						parentToken->setKeyword(keyword);
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
		bool caseSensitive_;
	};
	
	class RepeatNode: public Node
	{
	public:
		RepeatNode(int minRepeat, int maxRepeat, Ref<Node> entry)
			: minRepeat_(minRepeat),
			  maxRepeat_(maxRepeat),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
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
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	
	private:
		int minRepeat_;
		int maxRepeat_;
		Ref<Node, Owner> entry_;
	};
	
	class BoiNode: public Node
	{
	public:
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i != 0)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class EoiNode: public Node
	{
	public:
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			bool eoi = (!media->def(i)) && ((i == 0) || (media->def(i - 1)));
			if (!eoi)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class FindNode: public Node
	{
	public:
		FindNode(Ref<Node> entry)
			: entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			bool found = false;
			while (media->def(i) || media->def(i - 1)) {
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
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
	};
	
	class OrNode: public Node
	{
	public:
		OrNode(Ref<Node> firstChoice, Ref<Node> secondChoice)
			: firstChoice_(firstChoice),
			  secondChoice_(secondChoice)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int h = (firstChoice_) ? firstChoice_->matchNext(media, i, tokenFactory, parentToken, state) : i;
			if (h == -1) {
				rollBackOnFailure(h, parentToken, lastChildSaved);
				h = (secondChoice_) ? secondChoice_->matchNext(media, i, tokenFactory, parentToken, state) : i;
				rollBackOnFailure(h, parentToken, lastChildSaved);
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> firstChoice_;
		Ref<Node, Owner> secondChoice_;
	};
	
	class AheadNode: public Node
	{
	public:
		AheadNode(Ref<Node> entry, int invert)
			: entry_(entry),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int h = i;
			if (entry_)
				h = entry_->matchNext(media, i, 0, parentToken, state);
			
			if ((h == -1) ^ invert_)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
		int invert_;
	};
	
	class LengthNode: public Node
	{
	public:
		LengthNode(int minLength, int maxLength, Ref<Node> entry)
			: minLength_(minLength),
			  maxLength_(maxLength),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
			if (h != -1) {
				int d = h - i;
				if ((d < minLength_) || (maxLength_ < d))
					h = -1;
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	
	private:
		int minLength_;
		int maxLength_;
		Ref<Node, Owner> entry_;
	};
	
	class Definition;
	class InlineNode;
	
	class RuleNode: public Node
	{
	public:
		RuleNode(Ref<Definition> definition, const char* name, int ruleId, Ref<Node> entry, bool isVoid = false)
			: definition_(definition),
			  name_(name),
			  id_(ruleId),
			  entry_(entry),
			  isVoid_(isVoid)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			check(!next_);
			
			Ref<Token, Owner> token;
			if (tokenFactory) {
				token = tokenFactory->produce();
				token->init(definition_->id(), id_);
				if (parentToken)
					parentToken->appendChild(token);
			}
			
			int i0 = i;
			i = (entry_) ? entry_->matchNext(media, i, tokenFactory, token, state) : i;
		
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
		
		inline int id() const { return id_; }
		inline const char* name() const { return name_; }
		
	protected:
		friend class InlineNode;
		
		Ref<Definition> definition_;
		const char* name_;
		int id_;
		Ref<Node, Owner> entry_;
		bool isVoid_;
	};
	
	class LinkNode: public Node
	{
	public:
		LinkNode(const char* ruleName)
			: ruleName_(ruleName)
		{}
		
		inline const char* ruleName() const { return ruleName_; }
		inline Ref<RuleNode> rule() const { return rule_; }
		
	protected:
		friend class Definition;
		
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
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			i = LinkNode::rule_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
	};
	
	class InlineNode: public LinkNode
	{
	public:
		InlineNode(const char* ruleName)
			: LinkNode(ruleName)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			i = LinkNode::rule_->entry_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
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
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = -1;
			if (parentToken) {
				Ref<Token> sibling = parentToken->previousSibling();
				if (sibling)
					if ( (sibling->rule() == LinkNode::rule_->id()) &&
						 ((keyword_ == -1) || (sibling->keyword() == keyword_)) )
						h = i;
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	protected:
		friend class Definition;
		const char* keywordName_;
		int keyword_;
		Ref<PreviousNode, Owner> unresolvedKeywordNext_;
	};
	
	typedef int (*CallBack) (Ref<Instance> self, Media* media, int i, State* state);
	
	class CallNode: public Node
	{
	public:
		CallNode(CallBack callBack, Ref<Instance> self)
			: callBack_(callBack),
			  self_(self)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			i = callBack_(self_, media, i, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		CallBack callBack_;
		Ref<Instance, SetNull> self_;
	};
	
	class SetNode: public Node
	{
	public:
		SetNode(int flagId, bool value)
			: flagId_(flagId),
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
		IfNode(int flagId, Ref<Node> trueBranch, Ref<Node> falseBranch)
			: flagId_(flagId),
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
		GetCharNode(int charId)
			: charId_(charId)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i))
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
		SetCharNode(int charId, Char value)
			: charId_(charId),
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
		VarCharNode(int charId, int invert)
			: charId_(charId),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
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
	
	class GetStringNode: public Node
	{
	public:
		GetStringNode(int stringId, Ref<Node> coverage)
			: stringId_(stringId),
			  coverage_(coverage)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int i0 = i;
			i = coverage_->matchNext(media, i, 0, parentToken, state);
			
			if (i != -1)
			{
				if (parentToken) {
					while (parentToken->lastChild() != lastChildSaved)
						parentToken->lastChild()->unlink();
				}
				
				Array<Char>* string = state->string(stringId_);
				*string = Array<Char>(i - i0);
				for (int k = i0; k < i; ++k)
					string->set(k - i0, media->get(k));
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		int stringId_;
		Ref<Node, Owner> coverage_;
	};
	
	class VarStringNode: public Node
	{
	public:
		VarStringNode(int stringId)
			: stringId_(stringId)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Array<Char>* string = state->string(stringId_);
			int k = 0, m = string->length();
			while (media->def(i) && (k < m)) {
				if (media->get(i) != string->get(k))
					break;
				++i;
				++k;
			}
			
			if (k != m)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		int stringId_;
	};
	
	class InvokeNode: public Node
	{
	public:
		InvokeNode(Ref<Definition> definition, Ref<Node> coverage)
			: definition_(definition),
			  coverage_(coverage)
		{}
		
		InvokeNode(const char* name, Ref<Node> coverage)
			: definitionName_(name),
			  coverage_(coverage)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			State* childState = 0;
			if (state) {
				childState = state->child();
				if (childState) {
					if (childState->definitionId() != definition_->id())
						childState = 0;
				}
				if (!childState)
					childState = definition_->newState(state);
			}
			
			if (coverage_) {
				Ref<Token> lastChildSaved;
				if (parentToken) lastChildSaved = parentToken->lastChild();
				int i0 = i;
				i = coverage_->matchNext(media, i, 0, parentToken, state);
				if (i != -1) {
					if (parentToken) {
						while (parentToken->lastChild() != lastChildSaved)
							parentToken->lastChild()->unlink();
					}
					Media range(media, i);
					definition_->matchNext(&range, i0, tokenFactory, parentToken, childState);
				}
			}
			else {
				i = definition_->matchNext(media, i, tokenFactory, parentToken, childState);
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		friend class Definition;
		
		const char* definitionName_;
		Ref<Definition, SetNull> definition_;
		Ref<Node, Owner> coverage_;
		Ref<InvokeNode, Owner> unresolvedNext_;
	};
	
	class Scope;
	
	class Definition: public RefNode
	{
	public:
		Definition(Ref<Scope> scope = 0, const char* name = 0)
			: scope_(scope),
			  id_(scope ? scope->numDefinitions(1) : -1),
			  name_(name),
			  caseSensitive_(true),
			  numRules_(0),
			  numKeywords_(0),
			  ruleByName_(new RuleByName),
			  keywordByName_(new KeywordByName),
			  statefulScope_(false),
			  numStateFlags_(0),
			  numStateChars_(0),
			  numStateStrings_(0),
			  flagIdByName_(new StateIdByName),
			  charIdByName_(new StateIdByName),
			  stringIdByName_(new StateIdByName)
		{
			if (scope_)
				scope_->addDefinition(this);
		}
		
		inline Ref<Scope> scope() const { return scope_; }
		inline const char* name() const { return name_; }
		
		typedef Ref<Node, Owner> NODE;
		typedef Ref<RuleNode, Owner> RULE;
		
		//-- stateless definition interface
		
		inline void OPTION(const char* name, bool value) {
			if (str::casecmp(name, "caseSensitive") == 0)
				caseSensitive_ = value;
			else
				FTL_THROW(DebugException, str::cat("Unknown option '", name, "'"));
		}
		
		inline static NODE CHAR(Char ch) { return new CharNode(ch, 0); }
		inline static NODE OTHER(Char ch) { return new CharNode(ch, 1); }
		inline static NODE ANY() { return new AnyNode(); }
		inline static NODE RANGE(Char a, Char b) { return new RangeMinMaxNode(a, b, 0); }
		template<class Char> inline static NODE RANGE(const Char* s) { return new RangeExplicitNode(s, 0); }
		inline static NODE EXCEPT(Char a, Char b) { return new RangeMinMaxNode(a, b, 1); }
		template<class Char> inline static NODE EXCEPT(Char* s) { return new RangeExplicitNode(s, 1); }
		template<class Char> inline NODE STRING(Char* s) { return new StringNode(s, caseSensitive_); }
		
		NODE KEYWORD(const char* keywords)
		{
			Ref<KeywordMap, Owner> map = new KeywordMap;
			const char* pos = keywords;
			while (*pos) {
				if ((*pos == ' ') || (*pos == '\t')) {
					++pos;
					continue;
				}
				int len = 0;
				while (true) {
					char ch = *(pos + len);
					if ((ch == ' ') || (ch == '\t') || (ch == '\0')) break;
					++len;
				}
				int keyword = numKeywords_;
				numKeywords_ += keywordByName_->insert(pos, len, keyword, &keyword);
				map->insert(pos, len, keyword);
				pos += len;
			}
			return new KeywordNode(map, caseSensitive_);
		}
		
		inline static NODE REPEAT(int minRepeat, int maxRepeat, NODE entry) { return new RepeatNode(minRepeat, maxRepeat, entry); }
		inline static NODE REPEAT(int minRepeat, NODE entry) { return REPEAT(minRepeat, intMax, entry); }
		inline static NODE REPEAT(NODE entry) { return REPEAT(0, intMax, entry); }
		inline static NODE BOI() { return new BoiNode(); }
		inline static NODE EOI() { return new EoiNode(); }
		inline static NODE PASS() { return 0; }
		inline static NODE FAIL() { return NOT(PASS()); }
		inline static NODE FIND(NODE entry) { return new FindNode(entry); }
		inline static NODE AHEAD(NODE entry) { return new AheadNode(entry, 0); }
		inline static NODE NOT(NODE entry) { return new AheadNode(entry, 1); }
		
		inline static NODE LENGTH(int minLength, int maxLength, NODE entry) { return new LengthNode(minLength, maxLength, entry); }
		inline static NODE LENGTH(int minLength, NODE entry) { return LENGTH(minLength, intMax, entry); }
		
		#include "SyntaxSugar.hpp"
		
		inline int DEFINE(const char* ruleName, NODE entry = 0) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, ruleName, numRules_++, entry);
			addRule(rule);
			return rule->id();
		}
		inline void DEFINE_VOID(const char* ruleName, NODE entry = 0) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, ruleName, numRules_++, entry, true);
			addRule(rule);
		}
		inline void ENTRY(const char* ruleName) {
			LinkNode::ruleName_ = ruleName;
		}
		
		inline NODE REF(const char* ruleName) {
			Ref<RefNode, Owner> link = new RefNode(ruleName);
			link->unresolvedNext_ = unresolvedLinkHead_;
			unresolvedLinkHead_ = link;
			return link;
		}
		inline NODE INLINE(const char* ruleName) {
			Ref<InlineNode, Owner> link = new InlineNode(ruleName);
			link->unresolvedNext_ = unresolvedLinkHead_;
			unresolvedLinkHead_ = link;
			return link;
		}
		inline NODE PREVIOUS(const char* ruleName, const char* keyword = 0) {
			Ref<PreviousNode, Owner> link =  new PreviousNode(ruleName, keyword);
			link->unresolvedNext_ = unresolvedLinkHead_;
			unresolvedLinkHead_ = link;
			if (keyword) {
				link->unresolvedKeywordNext_ = unresolvedKeywordHead_;
				unresolvedKeywordHead_ = link;
			}
			return link;
		}
		
		inline NODE CALL(CallBack callBack, Ref<Instance> self = 0) {
			if (!self) self = this;
			return new CallNode(callBack, self);
		}
		inline NODE ERROR() {
			return new CallNode(errorCallBack, this);
		}
		
		inline void LINK() {
			while (unresolvedLinkHead_) {
				unresolvedLinkHead_->rule_ = ruleByName(unresolvedLinkHead_->ruleName_);
				unresolvedLinkHead_ = unresolvedLinkHead_->unresolvedNext_;
			}
			while (unresolvedKeywordHead_) {
				unresolvedKeywordHead_->keyword_ = keywordByName(unresolvedKeywordHead_->keywordName_);
				unresolvedKeywordHead_ = unresolvedKeywordHead_->unresolvedKeywordNext_;
			}
			while (unresolvedInvokeHead_) {
				unresolvedInvokeHead_->definition_ = definitionByName(unresolvedInvokeHead_->definitionName_);
				unresolvedInvokeHead_ = unresolvedInvokeHead_->unresolvedNext_;
			}
			if (!LinkNode::rule_) {
				if (!LinkNode::ruleName_)
					FTL_THROW(DebugException, "Missing entry rule declaration");
				LinkNode::rule_ = ruleByName(LinkNode::ruleName_);
			}
		}
		
		//-- stateful definition interface
		
		inline void STATE_FLAG(const char* name, bool defaultValue = false) {
			stateFlagHead_ = new StateFlag(stateFlagHead_, defaultValue);
			flagIdByName()->insert(name, numStateFlags_);
			++numStateFlags_;
		}
		inline void STATE_CHAR(const char* name, Char defaultValue = 0) {
			stateCharHead_ = new StateChar(stateCharHead_, defaultValue);
			charIdByName()->insert(name, numStateChars_);
			++numStateChars_;
		}
		inline void STATE_STRING(const char* name, const char* defaultValue = 0) {
			stateStringHead_ = new StateString(stateStringHead_, defaultValue);
			stringIdByName()->insert(name, numStateStrings_);
			++numStateStrings_;
		}
		inline NODE SET(const char* name, bool value) {
			return new SetNode(flagIdByName(name), value);
		}
		inline NODE IF(const char* name, NODE trueBranch, NODE falseBranch = 0) {
			return new IfNode(flagIdByName(name), trueBranch, falseBranch);
		}
		inline NODE GETCHAR(const char* name) {
			return new GetCharNode(charIdByName(name));
		}
		inline NODE SETCHAR(const char* name, Char value) {
			return new SetCharNode(charIdByName(name), value);
		}
		inline NODE VARCHAR(const char* name) {
			return new VarCharNode(charIdByName(name), 0);
		}
		inline NODE VAROTHER(const char* name) {
			return new VarCharNode(charIdByName(name), 1);
		}
		inline NODE GETSTRING(const char* name, NODE coverage) {
			return new GetStringNode(stringIdByName(name), coverage);
		}
		inline NODE VARSTRING(const char* name) {
			return new VarStringNode(stringIdByName(name));
		}
		
		inline NODE INVOKE(Definition* definition, NODE coverage = 0) {
			if (definition != this)
				definition->LINK();
			return new InvokeNode(definition, coverage);
		}
		
		inline NODE INVOKE(const char* definitionName, NODE coverage = 0) {
			Ref<InvokeNode, Owner> node = new InvokeNode(definitionName, coverage);
			node->unresolvedNext_ = unresolvedInvokeHead_;
			unresolvedInvokeHead_ = node;
			return node;
		}
		
		//-- execution interface
		
		inline int id() const { return id_; }
		inline int numRules() const { return numRules_; }
		
		inline bool stateful() const { return (stateFlagHead_) || (stateCharHead_) || (stateStringHead_) || (statefulScope_); }
		
		State* newState(State* parent = 0)
		{
			if (!stateful())
				return 0;
			
			State* state = new State(id_, numStateFlags_, numStateChars_, numStateStrings_, parent);
			
			Ref<StateFlag> stateFlag = stateFlagHead_;
			for (int id = numStateFlags_ - 1; id >= 0; --id) {
				*state->flag(id) = stateFlag->defaultValue_;
				stateFlag = stateFlag->next_;
			}
			
			Ref<StateChar> stateChar = stateCharHead_;
			for (int id = numStateChars_ - 1; id >= 0; --id) {
				*state->character(id) = stateChar->defaultValue_;
				stateChar = stateChar->next_;
			}
			
			Ref<StateString> stateString = stateStringHead_;
			for (int id = numStateStrings_ - 1; id >= 0; --id) {
				*state->string(id) = stateString->defaultValue_;
				stateString = stateString->next_;
			}
			
			return state;
		}
		
		bool find(Media* media, int* i0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, Ref<TokenFactory> tokenFactory = 0)
		{
			int i = *i0;
			bool found = false;
			while (media->def(i)) {
				if (match(media, i, i1, rootToken, 0, tokenFactory)) {
					found = true;
					break;
				}
				++i;
			}
			*i0 = i;
			return found;
		}
		
		bool match(Media* media, int i0 = 0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, State* state = 0, Ref<TokenFactory> tokenFactory = 0)
		{
			if (scope_) scope_->link();
			else LINK();
			
			Ref<State, Owner> localState;
			if (!state) {
				localState = newState();
				state = localState;
			}
			
			TokenFactory localTokenFactory;
			if (!tokenFactory)
				tokenFactory = &localTokenFactory;
			
			int h = matchNext(media, i0, tokenFactory, 0, state);
			if (rootToken)
				*rootToken = tokenFactory->rootToken();
			
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
		
		Ref<RuleNode> ruleByName(const char* name)
		{
			Ref<RuleNode, Owner> node;
			if (!ruleByName_->lookup(name, &node))
				FTL_THROW(DebugException, str::cat("Undefined rule '", name, "' referenced"));
			return node;
		}
		
		virtual Ref<Definition> definitionByName(const char* name)
		{
			if (scope_) {
				return scope_->definitionByName(name);
			}
			else {
				if (str::cmp(name, name_) != 0)
					FTL_THROW(DebugException, str::cat("Undefined definition '", name, "' referenced"));
				return this;
			}
		}
		
		int keywordByName(const char* keyword)
		{
			int tokenType = -1;
			if (!keywordByName_->lookup(keyword, &tokenType))
				FTL_THROW(DebugException, str::cat("Undefined keyword '", keyword, "' referenced"));
			return tokenType;
		}
		
		int flagIdByName(const char* name)
		{
			int flagId = -1;
			if (!flagIdByName()->lookup(name, &flagId))
				FTL_THROW(DebugException, str::cat("Undefined state flag '", name, "' referenced"));
			return flagId;
		}
		
		int charIdByName(const char* name)
		{
			int charId = -1;
			if (!charIdByName()->lookup(name, &charId))
				FTL_THROW(DebugException, str::cat("Undefined state char '", name, "' referenced"));
			return charId;
		}
		
		int stringIdByName(const char* name)
		{
			int stringId = -1;
			if (!stringIdByName()->lookup(name, &stringId))
				FTL_THROW(DebugException, str::cat("Undefined state string '", name, "' referenced"));
			return stringId;
		}
		
		static void getLineAndPosFromIndex(Media* media, int index, int* line, int* pos)
		{
			*line = 1;
			*pos = 1;
			int j = 0;
			while (media->def(j) && (j < index)) {
				Char ch = media->get(j);
				if (ch == '\n') { ++*line; *pos = 1; }
				else ++*pos;
				++j;
			}
		}
		
		virtual int syntaxError(Media* media, int index, State* state)
		{
			FTL_THROW(DebugException, "Unhandled syntax error");
			return -1;
		}
		
	private:
		friend class Scope;
		friend class InvokeNode;
		
		Ref<Scope, SetNull> scope_;
		int id_;
		const char* name_;
		bool caseSensitive_;
		
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
			StateChar(Ref<StateChar> head, Char defaultValue)
				: next_(head),
				  defaultValue_(defaultValue)
			{}
			Ref<StateChar, Owner> next_;
			Char defaultValue_;
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
			Array<Char> defaultValue_;
		};
		
		int numRules_;
		int numKeywords_;
		typedef PrefixTree<char, Ref<RuleNode, Owner> > RuleByName;
		typedef PrefixTree<char, int> KeywordByName;
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
		
		static int errorCallBack(Ref<Instance> self, Media* media, int index, State* state) {
			Ref<Definition> definition = self;
			return definition->syntaxError(media, index, state);
		}
	};
	
	class Scope: public Instance
	{
	private:
		typedef PrefixTree<char, Ref<Definition, Owner> > DefinitionByName;
		
	public:
		Scope()
			: definitionByName_(new DefinitionByName),
			  numDefinitions_(0)
		{}
		
		Ref<Definition> definitionByName(const char* name) const
		{
			Ref<Definition, Owner> definition;
			if (!definitionByName_->lookup(name, &definition))
				FTL_THROW(DebugException, str::cat("Undefined definition '", name, "' referenced"));
			return definition;
		}
		
		int numDefinitions() const { return numDefinitions_; }
		
		void link()
		{
			bool stateful = false;
			{
				Ref<Definition> definition = unresolvedDefinitionHead_;
				while ((definition) && (!stateful)) {
					stateful = definition->stateful();
					definition = definition->unresolvedNext_;
				}
			}
			while (unresolvedDefinitionHead_) {
				unresolvedDefinitionHead_->statefulScope_ = stateful;
				unresolvedDefinitionHead_->LINK();
				commit(unresolvedDefinitionHead_);
				unresolvedDefinitionHead_ = unresolvedDefinitionHead_->unresolvedNext_;
			}
		}
		
	protected:
		virtual void commit(Ref<Definition> definition) {}
		
	private:
		friend class Definition;
		
		void addDefinition(Ref<Definition> definition)
		{
			if (!definitionByName_->insert(definition->name(), definition))
				FTL_THROW(DebugException, str::cat("Redefinition of '", definition->name(), "'"));
			definition->unresolvedNext_ = unresolvedDefinitionHead_;
			unresolvedDefinitionHead_ = definition;
		}
		
		int numDefinitions(int delta)
		{
			int h = numDefinitions_;
			numDefinitions_ += delta;
			return h;
		}
		
		Ref<DefinitionByName, Owner> definitionByName_;
		int numDefinitions_;
		Ref<Definition, Owner> unresolvedDefinitionHead_;
	};
};

} // namespace ftl

#endif // FTL_SYNTAX_HPP
