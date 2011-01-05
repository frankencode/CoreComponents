/*
 * Syntax.hpp -- syntax description and production engine
 *
 * Copyright (c) 2007-2011, Frank Mertens
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
template<class Media> class SyntaxDebugger;

template<class Media>
class Syntax: public Tree< Syntax<Media> >
{
public:
	typedef typename Media::Item Char;
	typedef typename Media::Index Index;
	typedef Syntax Node;
	typedef PrefixTree<Char,int> KeywordMap;
	typedef SyntaxDebugger<Media> Debugger;
	
	inline static void rollBackOnFailure(Index i, Token* parentToken, Token* lastChildSaved) {
		if ((i == Media::ill()) && (parentToken)) {
			while (parentToken->lastChild() != lastChildSaved)
				parentToken->lastChild()->unlink();
		}
	}
	
	typedef SyntaxState<Char> State;
	
	virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state) = 0;
	
	class CharNode: public Node
	{
	public:
		CharNode(Char ch, int invert)
			: ch_(ch),
			  invert_(invert)
		{}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				if ((ch != ch_) ^ invert_)
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline Char ch() const { return ch_; }
		inline bool invert() const { return invert_; }
		
	private:
		Char ch_;
		int invert_;
	};
	
	class AnyNode: public Node
	{
	public:
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			return media->def(i) ? i + 1 : Media::ill();
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				if (((ch < a_) || (b_ < ch)) ^ invert_)
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline Char a() const { return a_; }
		inline Char b() const { return b_; }
		inline int invert() const { return invert_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				int k = 0, len = s_.length();
				while (k < len) {
					if (s_.get(k) == ch) break;
					++k;
				}
				if ((k == len) ^ invert_)
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline const Array<Char>& s() const { return s_; }
		inline int invert() const { return invert_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
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
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline const Array<Char>& s() const { return s_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Index h = 0;
				int keyword = -1;
				if (map_->match(media, i, &h, &keyword, caseSensitive_)) {
					if (parentToken)
						parentToken->setKeyword(keyword);
					i = h;
				}
				else
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline Ref<KeywordMap> map() const { return map_; }
		
	private:
		Ref<KeywordMap, Owner> map_;
		bool caseSensitive_;
	};
	
	class RepeatNode: public Node
	{
	public:
		RepeatNode(int minRepeat, int maxRepeat, Ref<Node> entry)
			: minRepeat_(minRepeat),
			  maxRepeat_(maxRepeat)
		{
			appendChild(entry);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int repeatCount = 0;
			Index h = i;
			while ((repeatCount < maxRepeat_) && (h != Media::ill()))
			{
				h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
				if (h != Media::ill()) {
					i = h;
					++repeatCount;
				}
			}
			if ((repeatCount < minRepeat_) || (maxRepeat_ < repeatCount))
				i = Media::ill();
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
		
		inline int minRepeat() const { return minRepeat_; }
		inline int maxRepeat() const { return maxRepeat_; }
		inline Ref<Node> entry() const { return Node::firstChild(); }
	
	private:
		int minRepeat_;
		int maxRepeat_;
	};
	
	class LengthNode: public Node
	{
	public:
		LengthNode(int minLength, int maxLength, Ref<Node> entry)
			: minLength_(minLength),
			  maxLength_(maxLength)
		{
			appendChild(entry);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			Index h = entry()->matchNext(media, i, tokenFactory, parentToken, state);
			if (h != Media::ill()) {
				int d = h - i;
				if ((d < minLength_) || (maxLength_ < d))
					h = Media::ill();
			}
			
			rollBackOnFailure(h, parentToken, lastChildSaved);
			
			return h;
		}
		
		inline int minLength() const { return minLength_; }
		inline int maxLength() const { return maxLength_; }
		inline Ref<Node> entry() const { return Node::firstChild(); }
		
	private:
		int minLength_;
		int maxLength_;
	};
	
	class BoiNode: public Node
	{
	public:
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			return (i == 0) ? i : Media::ill();
		}
	};
	
	class EoiNode: public Node
	{
	public:
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			bool eoi = (!media->def(i)) && ((i == 0) || (media->def(i - 1)));
			return eoi ? i : Media::ill();
		}
	};
	
	class PassNode: public Node
	{
	public:
		PassNode(int invert)
			: invert_(invert)
		{}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			return invert_ ? Media::ill() : i;
		}
		
		inline int invert() const { return invert_; }
		
	private:
		int invert_;
	};
	
	class FindNode: public Node
	{
	public:
		FindNode(Ref<Node> entry)
		{
			appendChild(entry);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			bool found = false;
			while (media->def(i) || media->def(i - 1)) {
				Index h = entry()->matchNext(media, i, tokenFactory, parentToken, state);
				if (h != Media::ill()) {
					found = true;
					i = h;
					break;
				}
				++i;
			}
			if (!found)
				i = Media::ill();
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
		
		inline Ref<Node> entry() const { return Node::firstChild(); }
	};
	
	class AheadNode: public Node
	{
	public:
		AheadNode(Ref<Node> entry, int invert)
			: invert_(invert)
		{
			appendChild(entry);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			Index h = i;
			if (entry())
				h = entry()->matchNext(media, i, 0, parentToken, state);
			
			if ((h == Media::ill()) ^ invert_)
				i = Media::ill();
			
			rollBackOnFailure(Media::ill(), parentToken, lastChildSaved);
			
			return i;
		}
		
		inline Ref<Node> entry() const { return Node::firstChild(); }
		inline int invert() const { return invert_; }
		
	private:
		int invert_;
	};
	
	class ChoiceNode: public Node
	{
	public:
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			Index h = Media::ill();
			Ref<Node> node = Node::firstChild();
			while ((node) && (h == Media::ill())) {
				h = node->matchNext(media, i, tokenFactory, parentToken, state);
				rollBackOnFailure(h, parentToken, lastChildSaved);
				node = node->nextSibling();
			}
			
			rollBackOnFailure(h, parentToken, lastChildSaved);
			
			return h;
		}
		
		inline Ref<Node> firstChoice() const { return Node::firstChild(); }
		inline Ref<Node> lastChoice() const { return Node::lastChild(); }
	};
	
	class GlueNode: public Node
	{
	public:
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			Ref<Node> node = Node::firstChild();
			while ((node) && (i != Media::ill())) {
				i = node->matchNext(media, i, tokenFactory, parentToken, state);
				node = node->nextSibling();
			}
			
			rollBackOnFailure(i, parentToken, lastChildSaved);
			
			return i;
		}
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
			  isVoid_(isVoid),
			  used_(false),
			  numberOfRefs_(-1)
		{
			appendChild(entry);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token, Owner> token;
			if (tokenFactory) {
				token = tokenFactory->produce();
				token->init(definition_->id(), id_);
				if (parentToken)
					parentToken->appendChild(token);
			}
			
			Index i0 = i;
			i = (entry()) ? entry()->matchNext(media, i, tokenFactory, token, state) : i;
			
			if (tokenFactory) {
				if (i != Media::ill()) {
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
		
		Ref<Definition> definition_;
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			i = LinkNode::rule_->matchNext(media, i, tokenFactory, parentToken, state);
			
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Index h = Media::ill();
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
	
	typedef int (*CallBack) (Ref<Instance> self, Media* media, int i, State* state);
	
	class CallNode: public Node
	{
	public:
		CallNode(CallBack callBack, Ref<Instance> self)
			: callBack_(callBack),
			  self_(self)
		{}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			return callBack_(self_, media, i, state);
		}
		
		inline CallBack callBack() const { return callBack_; }
		
	private:
		CallBack callBack_;
		Ref<Instance> self_;
	};
	
	class SetNode: public Node
	{
	public:
		SetNode(int flagId, bool value)
			: flagId_(flagId),
			  value_(value)
		{}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->flag(flagId_) = value_;
			return i;
		}
		
		inline int flagId() const { return flagId_; }
		inline bool value() const { return value_; }
		
	private:
		int flagId_;
		bool value_;
	};
	
	class IfNode: public Node
	{
	public:
		IfNode(int flagId, Ref<Node> trueBranch, Ref<Node> falseBranch)
			: flagId_(flagId)
		{
			appendChild(trueBranch);
			appendChild(falseBranch);
		}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			return (*state->flag(flagId_)) ?
				trueBranch()->matchNext(media, i, tokenFactory, parentToken, state) :
				falseBranch()->matchNext(media, i, tokenFactory, parentToken, state);
		}
		
		inline int flagId() const { return flagId_; }
		inline Ref<Node> trueBranch() const { return Node::firstChild(); }
		inline Ref<Node> falseBranch() const { return Node::lastChild(); }
		
	private:
		int flagId_;
	};
	
	class GetCharNode: public Node
	{
	public:
		GetCharNode(int charId)
			: charId_(charId)
		{}
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i))
				*state->character(charId_) = media->get(i++);
			else
				i = Media::ill();
			
			return i;
		}
		
		inline int charId() const { return charId_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->character(charId_) = value_;
			return i;
		}
		
		inline int charId() const { return charId_; }
		inline Char value() const { return value_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (media->def(i)) {
				Char ch = media->get(i++);
				if ((ch != *state->character(charId_)) ^ invert_)
					i = Media::ill();
			}
			else
				i = Media::ill();
			
			return i;
		}
		
		inline int charId() const { return charId_; }
		inline bool invert() const { return invert_; }
	
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved;
			if (parentToken) lastChildSaved = parentToken->lastChild();
			
			int i0 = i;
			i = coverage_->matchNext(media, i, 0, parentToken, state);
			
			if (i != Media::ill())
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
			
			return i;
		}
		
		inline int stringId() const { return stringId_; }
		inline Ref<Node> coverage() const { return coverage_; }
		
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
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
				i = Media::ill();
			
			return i;
		}
		
		inline int stringId() const { return stringId_; }
		
	private:
		int stringId_;
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
		
		virtual Index matchNext(Media* media, Index i, TokenFactory* tokenFactory, Token* parentToken, State* state)
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
			
			if (coverage()) {
				Ref<Token> lastChildSaved;
				if (parentToken) lastChildSaved = parentToken->lastChild();
				Index i0 = i;
				i = coverage()->matchNext(media, i, 0, parentToken, state);
				if (i != Media::ill()) {
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
	
	class Scope;
	
	class Definition: public RefNode
	{
	public:
		Definition(Ref<DebugFactory> debugFactory = 0, Ref<Scope> scope = 0, const char* name = 0)
			: debugFactory_(debugFactory),
			  scope_(scope),
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
			if (debugFactory_)
				debugFactory_->definition_ = this;
			if (scope_)
				scope_->addDefinition(this);
		}
		
		inline Ref<DebugFactory> debugFactory() const { return debugFactory_; }
		
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
		
		inline NODE CHAR(Char ch) { return debug(new CharNode(ch, 0), "Char"); }
		inline NODE OTHER(Char ch) { return debug(new CharNode(ch, 1), "Char"); }
		inline NODE ANY() { return debug(new AnyNode(), "Any"); }
		inline NODE RANGE(Char a, Char b) { return debug(new RangeMinMaxNode(a, b, 0), "RangeMinMax"); }
		template<class Char> inline NODE RANGE(const Char* s) { return debug(new RangeExplicitNode(s, 0), "RangeExplicit"); }
		inline NODE EXCEPT(Char a, Char b) { return debug(new RangeMinMaxNode(a, b, 1), "RangeMinMax"); }
		template<class Char> inline NODE EXCEPT(Char* s) { return debug(new RangeExplicitNode(s, 1), "RangeExplicit"); }
		template<class Char> inline NODE STRING(Char* s) { return debug(new StringNode(s, caseSensitive_), "String"); }
		
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
			return debug(new KeywordNode(map, caseSensitive_), "Keyword");
		}
		
		inline NODE REPEAT(int minRepeat, int maxRepeat, NODE entry) { return debug(new RepeatNode(minRepeat, maxRepeat, entry), "Repeat"); }
		inline NODE REPEAT(int minRepeat, NODE entry) { return REPEAT(minRepeat, intMax, entry); }
		inline NODE REPEAT(NODE entry) { return REPEAT(0, intMax, entry); }
		inline NODE LENGTH(int minLength, int maxLength, NODE entry) { return debug(new LengthNode(minLength, maxLength, entry), "Length"); }
		inline NODE LENGTH(int minLength, NODE entry) { return LENGTH(minLength, intMax, entry); }
		inline NODE BOI() { return debug(new BoiNode(), "Boi"); }
		inline NODE EOI() { return debug(new EoiNode(), "Eoi"); }
		inline NODE PASS() { return debug(new PassNode(0), "Pass"); }
		inline NODE FAIL() { return debug(new PassNode(1), "Pass"); }
		inline NODE FIND(NODE entry) { return debug(new FindNode(entry), "Find"); }
		inline NODE AHEAD(NODE entry) { return debug(new AheadNode(entry, 0), "Ahead"); }
		inline NODE NOT(NODE entry) { return debug(new AheadNode(entry, 1), "Ahead"); }
		
		inline NODE CHOICE() { return debug(new ChoiceNode, "Choice"); }
		inline NODE GLUE() { return debug(new GlueNode, "Glue"); }
		
		#include "SyntaxSugar.hpp"
		
		inline int DEFINE(const char* ruleName, NODE entry = 0) {
			if (!entry) entry = PASS();
			Ref<RuleNode, Owner> rule = new RuleNode(this, ruleName, numRules_++, entry);
			addRule(rule);
			return rule->id();
		}
		inline void DEFINE_VOID(const char* ruleName, NODE entry = 0) {
			if (!entry) entry = PASS();
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
		
		inline NODE CALL(CallBack callBack, Ref<Instance> self = 0) {
			if (!self) self = this;
			return debug(new CallNode(callBack, self), "Call");
		}
		inline NODE ERROR() {
			return debug(new CallNode(errorCallBack, this), "Call");
		}
		
		void OPTIMIZE()
		{
			while (unresolvedLinkHead_) {
				Ref<LinkNode> link = unresolvedLinkHead_;
				link->rule_ = ruleByName(link->ruleName_);
				if (link->rule_->isVoid()) {
					if (Ref<RefNode>(link)) {
						if (link->rule_->numberOfRefs() == 0) {
							Ref<LinkNode, Owner> iLink = new InlineNode(link->ruleName_);
							link->parent()->insertChild(iLink, link);
							iLink->rule_ = link->rule_;
							link->unlink();
						}
					}
				}
				unresolvedLinkHead_ = link->unresolvedNext_;
			}
		}
		
		void LINK(bool optimize = true)
		{
			if (optimize) OPTIMIZE();
			
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
			return debug(new SetNode(flagIdByName(name), value), "Set");
		}
		inline NODE IF(const char* name, NODE trueBranch, NODE falseBranch = 0) {
			if (!trueBranch) trueBranch = PASS();
			if (!falseBranch) falseBranch = PASS();
			return debug(new IfNode(flagIdByName(name), trueBranch, falseBranch), "If");
		}
		inline NODE GETCHAR(const char* name) {
			return debug(new GetCharNode(charIdByName(name)), "GetChar");
		}
		inline NODE SETCHAR(const char* name, Char value) {
			return debug(new SetCharNode(charIdByName(name), value), "SetChar");
		}
		inline NODE VARCHAR(const char* name) {
			return debug(new VarCharNode(charIdByName(name), 0), "VarChar");
		}
		inline NODE VAROTHER(const char* name) {
			return debug(new VarCharNode(charIdByName(name), 1), "VarChar");
		}
		inline NODE GETSTRING(const char* name, NODE coverage) {
			return debug(new GetStringNode(stringIdByName(name), coverage), "GetString");
		}
		inline NODE VARSTRING(const char* name) {
			return debug(new VarStringNode(stringIdByName(name)), "VarString");
		}
		
		inline NODE INVOKE(Definition* definition, NODE coverage = 0) {
			if (definition != this)
				definition->LINK();
			return debug(new InvokeNode(definition, coverage), "Invoke");
		}
		
		inline NODE INVOKE(const char* definitionName, NODE coverage = 0) {
			Ref<InvokeNode, Owner> node = new InvokeNode(definitionName, coverage);
			node->unresolvedNext_ = unresolvedInvokeHead_;
			unresolvedInvokeHead_ = node;
			return debug(node, "Invoke");
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
		
		Ref<Token, Owner> find(Media* media, Index* i0, Index* i1 = 0, Ref<TokenFactory> tokenFactory = 0)
		{
			Index i = *i0;
			Ref<Token, Owner> rootToken;
			while (media->def(i)) {
				if (rootToken = match(media, i, i1, 0, tokenFactory))
					break;
				++i;
			}
			*i0 = i;
			return rootToken;
		}
		
		Ref<Token, Owner> match(Media* media, Index i0 = 0, Index* i1 = 0, State* state = 0, Ref<TokenFactory> tokenFactory = 0)
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
			
			Index h = matchNext(media, i0, tokenFactory, 0, state);
			
			if ((i1 != 0) && (h != Media::ill()))
				*i1 = h;
			
			return (h != Media::ill()) ? tokenFactory->rootToken() : Ref<Token>();
		}
		
		Ref<RuleNode> ruleByName(const char* name) const
		{
			Ref<RuleNode, Owner> node;
			if (!ruleByName_->lookup(name, &node))
				FTL_THROW(DebugException, str::cat("Undefined rule '", name, "' referenced"));
			return node;
		}
		
		int keywordByName(const char* keyword)
		{
			int tokenType = -1;
			if (!keywordByName_->lookup(keyword, &tokenType))
				FTL_THROW(DebugException, str::cat("Undefined keyword '", keyword, "' referenced"));
			return tokenType;
		}
		
		Ref<Definition> definitionByName(const char* name) const
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
		friend class SyntaxDebugger<Media>;
		Ref<DebugFactory, Owner> debugFactory_;
		
		inline Node* debug(Node* newNode, const char* nodeType) {
			return debugFactory_ ? debugFactory_->produce(newNode, nodeType) : newNode;
		}
		
		friend class Scope;
		friend class InvokeNode;
		
		Ref<Scope> scope_;
		int id_;
		const char* name_;
		bool caseSensitive_;
		
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
