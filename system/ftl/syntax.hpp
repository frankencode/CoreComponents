/*
 * syntax.hpp -- syntax description and production formalism
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAX_HPP
#define FTL_SYNTAX_HPP

#include "Crc32.hpp"
#include "PrefixTree.hpp"
#include "SyntaxNode.hpp"
#include "SyntaxDebugFactory.hpp"

namespace ftl
{

namespace syntax
{

class Debugger;

class CharNode: public Node
{
public:
	CharNode(char ch, int invert)
		: ch_(ch),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			if ((ch != ch_) ^ invert_)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return 1; }

	inline char ch() const { return ch_; }
	inline bool invert() const { return invert_; }

private:
	char ch_;
	int invert_;
};

class GreaterNode: public Node
{
public:
	GreaterNode(char ch, int invert)
		: ch_(ch),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			if ((ch <= ch_) ^ invert_)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return 1; }

	inline char ch() const { return ch_; }
	inline bool invert() const { return invert_; }

private:
	char ch_;
	int invert_;
};


class GreaterOrEqualNode: public Node
{
public:
	GreaterOrEqualNode(char ch, int invert)
		: ch_(ch),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			if ((ch < ch_) ^ invert_)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return 1; }

	inline char ch() const { return ch_; }
	inline bool invert() const { return invert_; }

private:
	char ch_;
	int invert_;
};

class AnyNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		return media->has(i) ? i + 1 : -1;
	}
};

class RangeMinMaxNode: public Node
{
public:
	RangeMinMaxNode(char a, char b, int invert)
		: a_(a),
		  b_(b),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			if (((ch < a_) || (b_ < ch)) ^ invert_)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return 1; }

	inline char a() const { return a_; }
	inline char b() const { return b_; }
	inline int invert() const { return invert_; }

private:
	char a_, b_;
	int invert_;
};

class RangeExplicitNode: public Node
{
public:
	RangeExplicitNode(const char *s, int invert)
		: s_(ByteArray::create(s)),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			int k = 0, len = s_->length();
			while (k < len) {
				if (s_->get(k) == ch) break;
				++k;
			}
			if ((k == len) ^ invert_)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return 1; }

	inline const ByteArray &s() const { return *s_; }
	inline int invert() const { return invert_; }

private:
	Ref<ByteArray, Owner> s_;
	int invert_;
};

class StringNode: public Node
{
public:
	StringNode(const char *s, bool caseSensitive)
		: s_(ByteArray::create(s)),
		  caseSensitive_(caseSensitive)
	{
		if (!caseSensitive) s_ = s_->toLower();
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			int k = 0, len = s_->length();
			while ((k < len) && (media->has(i))) {
				char ch = media->get(i++);
				if (!caseSensitive_)
					ch = ToLower<char>::map(ch);
				if (s_->at(k) != ch) break;
				++k;
			}
			if (k != len)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return s_->length(); }

	inline const ByteArray &s() const { return *s_; }

private:
	Ref<ByteArray, Owner> s_;
	bool caseSensitive_;
};

typedef PrefixTree<char, int> KeywordMap;

class KeywordNode: public Node
{
public:
	KeywordNode(Ref<KeywordMap> map, bool caseSensitive)
		: map_(map),
		  caseSensitive_(caseSensitive)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int repeatCount = 0;
		int h = i;
		while ((repeatCount < maxRepeat_) && (h != -1))
		{
			h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
			if (h == i)
				FTL_THROW(DebugException, "Repeated empty match, bailing out");
			if (h != -1) {
				i = h;
				++repeatCount;
			}
		}
		if ((repeatCount < minRepeat_) || (maxRepeat_ < repeatCount))
			i = -1;

		if (i == -1)
			rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const {
		return (minRepeat_ == maxRepeat_) ? minRepeat_ * entry()->matchLength() : -1;
	}

	inline int minRepeat() const { return minRepeat_; }
	inline int maxRepeat() const { return maxRepeat_; }
	inline Ref<Node> entry() const { return Node::firstChild(); }

private:
	int minRepeat_;
	int maxRepeat_;
};

class LazyRepeatNode: public Node
{
public:
	LazyRepeatNode(int minRepeat, Ref<Node> entry)
		: minRepeat_(minRepeat)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int repeatCount = 0;
		int h = i;
		while (h != -1)
		{
			h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
			if (h == i)
				FTL_THROW(DebugException, "Repeated empty match, bailing out");
			if (h != -1) {
				++repeatCount;
				if (minRepeat_ <= repeatCount) {
					int j = h;
					Ref<Node> succ = Node::succ();
					if (succ) {
						Ref<Token> lastChildSaved2;
						if (parentToken) lastChildSaved2 = parentToken->lastChild();
						while (succ) {
							j = succ->matchNext(media, j, tokenFactory, parentToken, state);
							if (j == -1) break;
							succ = succ->succ();
						}
						rollBack(parentToken, lastChildSaved2);
					}
					if (j != -1) return h;
				}
			}
		}

		rollBack(parentToken, lastChildSaved);
		return -1;
	}

	inline int matchLength() const { return -1; }

	inline int minRepeat() const { return minRepeat_; }
	inline Ref<Node> entry() const { return Node::firstChild(); }

private:
	int minRepeat_;
};

class GreedyRepeatNode: public Node
{
public:
	GreedyRepeatNode(int minRepeat, int maxRepeat, Ref<Node> entry)
		: minRepeat_(minRepeat),
		  maxRepeat_(maxRepeat)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved, lastChildSaved2;
		if (parentToken) {
			lastChildSaved = parentToken->lastChild();
			lastChildSaved2 = parentToken->lastChild();
		}

		int repeatCount = 0;
		int h = i, j = -1;
		while ((repeatCount < maxRepeat_) && (h != -1))
		{
			h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
			if (h == i)
				FTL_THROW(DebugException, "Repeated empty match, bailing out");
			if (h != -1) {
				++repeatCount;
				if (minRepeat_ <= repeatCount) {
					Ref<Node> succ = Node::succ();
					if (succ) {
						Ref<Token> lastChildSaved3;
						if (parentToken) lastChildSaved3 = parentToken->lastChild();
						j = h;
						while (succ) {
							j = succ->matchNext(media, j, tokenFactory, parentToken, state);
							if (j == -1) break;
							succ = succ->succ();
						}
						if (j != -1) {
							i = h;
							lastChildSaved2 = lastChildSaved3;
						}
						rollBack(parentToken, lastChildSaved3);
					}
					else {
						i = h;
					}
				}
			}
		}

		if ((repeatCount < minRepeat_) || (maxRepeat_ < repeatCount))
			i = -1;

		if (i == -1)
			rollBack(parentToken, lastChildSaved);
		else if (j == -1)
			rollBack(parentToken, lastChildSaved2);

		return i;
	}

	inline int matchLength() const {
		return (minRepeat_ == maxRepeat_) ? minRepeat_ * entry()->matchLength() : -1;
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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = entry()->matchNext(media, i, tokenFactory, parentToken, state);
		if (h != -1) {
			int d = h - i;
			if ((d < minLength_) || (maxLength_ < d))
				h = -1;
		}

		if (h == -1)
			rollBack(parentToken, lastChildSaved);

		return h;
	}

	inline int matchLength() const { return 0; }

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
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		return (i == 0) ? i : -1;
	}
	inline int matchLength() const { return 0; }
};

class EoiNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		bool eoi = (!media->has(i)) && ((i == 0) || (media->has(i - 1)));
		return eoi ? i : -1;
	}
	inline int matchLength() const { return 0; }
};

class PassNode: public Node
{
public:
	PassNode(int invert)
		: invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		return invert_ ? -1 : i;
	}

	inline int matchLength() const { return 0; }

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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		bool found = false;
		while (media->has(i) || media->has(i - 1)) {
			int h = entry()->matchNext(media, i, tokenFactory, parentToken, state);
			if (h != -1) {
				found = true;
				i = h;
				break;
			}
			++i;
		}
		if (!found)
			i = -1;

		if (i == -1)
			rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const { return 0; }

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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = i;
		if (entry())
			h = entry()->matchNext(media, i, 0, parentToken, state);

		if ((h == -1) ^ invert_)
			i = -1;

		rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const { return 0; }

	inline Ref<Node> entry() const { return Node::firstChild(); }
	inline int invert() const { return invert_; }

private:
	int invert_;
};

class BehindNode: public Node
{
public:
	BehindNode(Ref<Node> entry, int invert)
		: invert_(invert),
		  length_(entry->matchLength())
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		if (!media->has(i - length_))
			i = -1;
		else if ((entry()->matchNext(media, i - length_, tokenFactory, parentToken, state) == -1) ^ invert_)
			i = -1;

		rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const { return 0; }

	inline Ref<Node> entry() const { return Node::firstChild(); }
	inline int invert() const { return invert_; }
	inline int length() const { return length_; }

private:
	int invert_;
	int length_;
};

class ChoiceNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = -1;
		Ref<Node> node = Node::firstChild();
		while ((node) && (h == -1)) {
			h = node->matchNext(media, i, tokenFactory, parentToken, state);
			if (h == -1)
				rollBack(parentToken, lastChildSaved);
			node = node->nextSibling();
		}

		if (h == -1)
			rollBack(parentToken, lastChildSaved);

		return h;
	}

	virtual Ref<Node> succ(Ref<Node> node) const
	{
		return Node::parent() ? Node::parent()->succ(this) : Ref<Node>();
	}

	virtual int matchLength() const
	{
		int len = -1;
		for (Ref<Node> node = Node::firstChild(); node; node = Node::nextSibling()) {
			int len2 = node->matchLength();
			if ((len != -1) && (len2 != len))
				return -1;
			len = len2;
		}
		return len;
	}

	inline Ref<Node> firstChoice() const { return Node::firstChild(); }
	inline Ref<Node> lastChoice() const { return Node::lastChild(); }
};

class GlueNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		Ref<Node> node = Node::firstChild();
		while ((node) && (i != -1)) {
			i = node->matchNext(media, i, tokenFactory, parentToken, state);
			node = node->nextSibling();
		}

		if (i == -1)
			rollBack(parentToken, lastChildSaved);

		return i;
	}

	virtual Ref<Node> succ(Ref<Node> node) const
	{
		Ref<Node> succ = node->nextSibling();
		if ((!succ) && (Node::parent())) succ = Node::parent()->succ(this);
		return succ;
	}

	virtual int matchLength() const
	{
		int len = 0;
		for (Ref<Node> node = Node::firstChild(); node; node = node->nextSibling()) {
			int len2 = node->matchLength();
			if (len2 == -1) {
				len = -1;
				break;
			}
			len += len2;
		}
		return len;
	}
};

class HintNode: public Node
{
public:
	HintNode(const char *text)
		: text_(text)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if ((!text_) || (!state->hint())) {
			state->setHint(text_);
			state->setHintOffset(i);
		}
		return i;
	}

	inline int matchLength() const { return 0; }

	inline const char *text() const { return text_; }

private:
	const char *text_;
};

typedef int (*CallBack) (Ref<Instance> self, ByteArray *media, int i, State *state);

class CallNode: public Node
{
public:
	CallNode(CallBack callBack, Ref<Instance> self)
		: callBack_(callBack),
		  self_(self)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		state->setFlag(flagId_, value_);
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

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		return state->flag(flagId_) ?
			trueBranch()->matchNext(media, i, tokenFactory, parentToken, state) :
			falseBranch()->matchNext(media, i, tokenFactory, parentToken, state);
	}

	inline int flagId() const { return flagId_; }
	inline Ref<Node> trueBranch() const { return Node::firstChild(); }
	inline Ref<Node> falseBranch() const { return Node::lastChild(); }

private:
	int flagId_;
};

class CaptureNode: public Node
{
public:
	CaptureNode(int captureId, Ref<Node> coverage)
		: captureId_(captureId)
	{
		appendChild(coverage);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int i0 = i;
		i = coverage()->matchNext(media, i, 0, parentToken, state);

		if (i != -1) {
			rollBack(parentToken, lastChildSaved);
			state->setCapture(captureId_, Range::create(i0, i));
		}

		return i;
	}

	inline int captureId() const { return captureId_; }
	inline Ref<Node> coverage() const { return Node::firstChild(); }

private:
	int captureId_;
};

class ReplayNode: public Node
{
public:
	ReplayNode(int captureId)
		: captureId_(captureId)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Ref<Range> range = state->capture(captureId_);
		for (int j = range->i0(); (j < range->i1()) && media->has(i) && media->has(j); ++i, ++j) {
			if (media->get(i) != media->get(j)) return -1;
		}
		return i;
	}

	inline int captureId() const { return captureId_; }

private:
	int captureId_;
};

class RefNode;
class DefinitionNode;

class RuleNode: public Node
{
public:
	RuleNode(int definitionId, const char *name, int ruleId, Ref<Node> entry, bool isVoid = false)
		: definitionId_(definitionId),
		  name_(name),
		  id_(ruleId),
		  isVoid_(isVoid),
		  used_(false),
		  numberOfRefs_(-1)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
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
	inline const char *name() const { return name_; }

	inline Ref<Node> entry() const { return Node::firstChild(); }
	inline bool isVoid() const { return isVoid_; }

	inline bool used() const { return used_; }
	inline void markUsed() { used_ = true; }

protected:
	friend class InlineNode;

	int definitionId_;
	const char *name_;
	int id_;
	bool isVoid_;
	bool used_;
	int numberOfRefs_;
};

class LinkNode: public Node
{
public:
	LinkNode(const char *ruleName)
		: ruleName_(ruleName)
	{}

	LinkNode(Ref<RuleNode> rule)
		: ruleName_(rule->name()),
		  rule_(rule)
	{}

	inline const char *ruleName() const { return ruleName_; }
	inline Ref<RuleNode> rule() const { return rule_; }

protected:
	friend class ftl::syntax::DefinitionNode;
	friend class ftl::syntax::Debugger;

	const char *ruleName_;
	Ref<RuleNode> rule_;
	Ref<LinkNode, Owner> unresolvedNext_;
};

class RefNode: public LinkNode
{
public:
	RefNode(const char *ruleName = 0)
		: LinkNode(ruleName)
	{}

	RefNode(Ref<RuleNode> rule)
		: LinkNode(rule)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
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
	InlineNode(const char *ruleName)
		: LinkNode(ruleName)
	{}

	InlineNode(Ref<RuleNode> rule)
		: LinkNode(rule)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		return LinkNode::rule_->entry()->matchNext(media, i, tokenFactory, parentToken, state);
	}
};

class PreviousNode: public LinkNode
{
public:
	PreviousNode(const char *ruleName, const char *keyword = 0)
		: LinkNode(ruleName),
		  keywordName_(keyword),
		  keyword_(-1)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
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

	inline const char *keywordName() const { return keywordName_; }

protected:
	friend class DefinitionNode;
	const char *keywordName_;
	int keyword_;
	Ref<PreviousNode, Owner> unresolvedKeywordNext_;
};

class ContextNode: public LinkNode
{
public:
	ContextNode(const char *ruleName, Ref<Node> entry = 0)
		: LinkNode(ruleName)
	{
		if (entry) appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
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

class DefinitionNode;
class InvokeNode;
class DebugFactory;

class InvokeNode: public Node
{
public:
	InvokeNode(Ref<DefinitionNode> definition, Ref<Node> coverage)
		: definition_(definition)
	{
		if (coverage) appendChild(coverage);
	}

	InvokeNode(const char *name, Ref<Node> coverage)
		: definitionName_(name)
	{
		if (coverage) appendChild(coverage);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const;

	inline const char *definitionName() const { return definitionName_; }
	inline Ref<Node> coverage() const { return Node::firstChild(); }

private:
	friend class DefinitionNode;

	const char *definitionName_;
	Ref<DefinitionNode> definition_;
	Ref<InvokeNode, Owner> unresolvedNext_;
};

class DefinitionNode: public RefNode
{
public:
	DefinitionNode(Ref<DebugFactory> debugFactory = 0)
		: debugFactory_(debugFactory),
		  id_(Crc32().sum()),
		  name_(0),
		  caseSensitive_(true),
		  definitionByName_(DefinitionByName::create()),
		  numRules_(0),
		  numKeywords_(0),
		  ruleByName_(RuleByName::create()),
		  keywordByName_(KeywordByName::create()),
		  statefulScope_(false),
		  hasHints_(false),
		  numFlags_(0),
		  numCaptures_(0),
		  flagIdByName_(StateIdByName::create()),
		  captureIdByName_(StateIdByName::create())
	{
		if (debugFactory_)
			debugFactory_->definition_ = this;
	}

	inline Ref<DebugFactory> debugFactory() const { return debugFactory_; }

	inline int id() const { return id_; }
	inline const char *name() const { return name_; }

	//-- stateless definition interface

	inline void SYNTAX(const char *name) {
		id_ = crc32(name);
		name_ = name;
	}

	inline void IMPORT(Ref<DefinitionNode> definition, const char *name = 0) {
		if (!name) name = definition->name();
		if (!name)
			FTL_THROW(DebugException, "Cannot import anonymous syntax definition");
		definitionByName_->insert(name, definition);
	}

	typedef Ref<Node, Owner> NODE;
	typedef Ref<RuleNode, Owner> RULE;

	inline void OPTION(const char *name, bool value) {
		if (str::casecmp(name, "caseSensitive") == 0)
			caseSensitive_ = value;
		else
			FTL_THROW(DebugException, str::cat("Unknown option '", name, "'"));
	}

	inline NODE CHAR(char ch) { return debug(new CharNode(ch, 0), "Char"); }
	inline NODE OTHER(char ch) { return debug(new CharNode(ch, 1), "Char"); }
	inline NODE GREATER(char ch) { return debug(new GreaterNode(ch, 0), "Greater"); }
	inline NODE BELOW(char ch) { return debug(new GreaterNode(ch, 1), "Greater"); }
	inline NODE GREATER_OR_EQUAL(char ch) { return debug(new GreaterOrEqualNode(ch, 0), "GreaterOrEqual"); }
	inline NODE BELOW_OR_EQUAL(char ch) { return debug(new GreaterOrEqualNode(ch, 1), "GreaterOrEqual"); }
	inline NODE ANY() { return debug(new AnyNode(), "Any"); }

	inline NODE RANGE(char a, char b) { return debug(new RangeMinMaxNode(a, b, 0), "RangeMinMax"); }
	inline NODE RANGE(const char *s) { return debug(new RangeExplicitNode(s, 0), "RangeExplicit"); }
	inline NODE EXCEPT(char a, char b) { return debug(new RangeMinMaxNode(a, b, 1), "RangeMinMax"); }
	inline NODE EXCEPT(const char *s) { return debug(new RangeExplicitNode(s, 1), "RangeExplicit"); }

	inline NODE STRING(const char *s) { return debug(new StringNode(s, caseSensitive_), "String"); }
	NODE KEYWORD(const char *keywords);

	inline NODE REPEAT(int minRepeat, int maxRepeat, NODE entry) { return debug(new RepeatNode(minRepeat, maxRepeat, entry), "Repeat"); }
	inline NODE REPEAT(int minRepeat, NODE entry) { return REPEAT(minRepeat, intMax, entry); }
	inline NODE REPEAT(NODE entry) { return REPEAT(0, intMax, entry); }

	inline NODE LAZY_REPEAT(int minRepeat, NODE entry) { return debug(new LazyRepeatNode(minRepeat, entry), "LazyRepeat"); }
	inline NODE LAZY_REPEAT(NODE entry) { return LAZY_REPEAT(0, entry); }

	inline NODE GREEDY_REPEAT(int minRepeat, int maxRepeat, NODE entry) { return debug(new GreedyRepeatNode(minRepeat, maxRepeat, entry), "GreedyRepeat"); }
	inline NODE GREEDY_REPEAT(int minRepeat, NODE entry) { return GREEDY_REPEAT(minRepeat, intMax, entry); }
	inline NODE GREEDY_REPEAT(NODE entry) { return GREEDY_REPEAT(0, intMax, entry); }

	inline NODE LENGTH(int minLength, int maxLength, NODE entry) { return debug(new LengthNode(minLength, maxLength, entry), "Length"); }
	inline NODE LENGTH(int minLength, NODE entry) { return LENGTH(minLength, intMax, entry); }
	inline NODE BOI() { return debug(new BoiNode(), "Boi"); }
	inline NODE EOI() { return debug(new EoiNode(), "Eoi"); }
	inline NODE PASS() { return debug(new PassNode(0), "Pass"); }
	inline NODE FAIL() { return debug(new PassNode(1), "Pass"); }
	inline NODE FIND(NODE entry) { return debug(new FindNode(entry), "Find"); }
	inline NODE AHEAD(NODE entry) { return debug(new AheadNode(entry, 0), "Ahead"); }
	inline NODE NOT(NODE entry) { return debug(new AheadNode(entry, 1), "Ahead"); }
	inline NODE BEHIND(NODE entry) { return debug(new BehindNode(entry, 0), "Behind"); }
	inline NODE NOT_BEHIND(NODE entry) { return debug(new BehindNode(entry, 1), "Behind"); }

	inline NODE CHOICE() { return debug(new ChoiceNode, "Choice"); }
	inline NODE GLUE() { return debug(new GlueNode, "Glue"); }

	#include "SyntaxSugar.hpp"

	inline NODE HINT(const char *text = "") {
		hasHints_ = true;
		return debug(new HintNode(text), "Hint");
	}
	inline NODE DONE() { return debug(new HintNode(0), "Hint"); }

	inline int DEFINE(const char *ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode, Owner> rule = new RuleNode(id_, ruleName, numRules_++, entry);
		addRule(rule);
		return rule->id();
	}
	inline void DEFINE_VOID(const char *ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode, Owner> rule = new RuleNode(id_, ruleName, numRules_++, entry, true);
		addRule(rule);
	}
	inline void ENTRY(const char *ruleName) {
		LinkNode::ruleName_ = ruleName;
	}

	inline NODE REF(const char *ruleName) {
		Ref<RefNode, Owner> link = new RefNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Ref");
	}
	inline NODE INLINE(const char *ruleName) {
		Ref<InlineNode, Owner> link = new InlineNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Inline");
	}
	inline NODE PREVIOUS(const char *ruleName, const char *keyword = 0) {
		Ref<PreviousNode, Owner> link = new PreviousNode(ruleName, keyword);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		if (keyword) {
			link->unresolvedKeywordNext_ = unresolvedKeywordHead_;
			unresolvedKeywordHead_ = link;
		}
		return debug(link, "Previous");
	}
	inline NODE CONTEXT(const char *ruleName, NODE entry = 0) {
		Ref<ContextNode, Owner> link = new ContextNode(ruleName, entry);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Context");
	}

	typedef int (*CallBack) (Ref<Instance> self, ByteArray *media, int i, State *state);

	inline NODE CALL(CallBack callBack, Ref<Instance> self = 0) {
		if (!self) self = this;
		return debug(new CallNode(callBack, self), "Call");
	}
	inline NODE ERROR() {
		return debug(new CallNode(errorCallBack, this), "Call");
	}

	void OPTIMIZE();
	void LINK(bool optimize = true);

	//-- stateful definition interface

	inline NODE SET(const char *name, bool value) {
		return debug(new SetNode(touchFlag(name), value), "Set");
	}
	inline NODE IF(const char *name, NODE trueBranch, NODE falseBranch = 0) {
		if (!trueBranch) trueBranch = PASS();
		if (!falseBranch) falseBranch = PASS();
		return debug(new IfNode(touchFlag(name), trueBranch, falseBranch), "If");
	}
	inline NODE CAPTURE(const char *name, NODE coverage) {
		return debug(new CaptureNode(touchCapture(name), coverage), "Capture");
	}
	inline NODE REPLAY(const char *name) {
		return debug(new ReplayNode(touchCapture(name)), "Replay");
	}

	inline NODE INVOKE(DefinitionNode *definition, NODE coverage = 0) {
		return debug(new InvokeNode(definition, coverage), "Invoke");
	}
	inline NODE INVOKE(const char *definitionName, NODE coverage = 0) {
		Ref<InvokeNode, Owner> node = new InvokeNode(definitionName, coverage);
		node->unresolvedNext_ = unresolvedInvokeHead_;
		unresolvedInvokeHead_ = node;
		return debug(node, "Invoke");
	}

	//-- execution interface

	inline int numRules() const { return numRules_; }

	inline bool stateful() const { return (numFlags_ > 0) || (numCaptures_ > 0) || statefulScope_ || hasHints_; }

	State *newState(State *parent = 0) const;

	Ref<Token, Owner> find(ByteArray *media, int *i0, int *i1 = 0, Ref<TokenFactory> tokenFactory = 0) const;
	Ref<Token, Owner> match(ByteArray *media, int i0 = 0, int *i1 = 0, State *state = 0, Ref<TokenFactory> tokenFactory = 0) const;

	Ref<DefinitionNode> resolveScope(const char*& name) const;

	inline Ref<DefinitionNode> definitionByName(const char *name) const
	{
		Ref<DefinitionNode, Owner> definition;
		Ref<DefinitionNode> scope = resolveScope(name);
		if (!scope->definitionByName_->lookup(name, &definition))
			FTL_THROW(DebugException, str::cat("Undefined definition '", name, "' referenced"));
		return definition;
	}

	inline Ref<RuleNode> ruleByName(const char *name) const
	{
		Ref<DefinitionNode> scope = resolveScope(name);
		Ref<RuleNode, Owner> node;
		FTL_ASSERT(scope);
		if (!scope->ruleByName_->lookup(name, &node))
			FTL_THROW(DebugException, str::cat("Undefined rule '", name, "' referenced"));
		return node;
	}

	inline int keywordByName(const char *keyword)
	{
		int tokenType = -1;
		if (!keywordByName_->lookup(keyword, &tokenType))
			FTL_THROW(DebugException, str::cat("Undefined keyword '", keyword, "' referenced"));
		return tokenType;
	}

	inline int flagIdByName(const char *name)
	{
		int flagId = -1;
		if (!flagIdByName_->lookup(name, &flagId))
			FTL_THROW(DebugException, str::cat("Undefined state flag '", name, "' referenced"));
		return flagId;
	}

	inline int captureIdByName(const char *name)
	{
		int captureId = -1;
		if (!captureIdByName_->lookup(name, &captureId))
			FTL_THROW(DebugException, str::cat("Undefined capture '", name, "' referenced"));
		return captureId;
	}

	virtual int syntaxError(ByteArray *media, int index, State *state) const;

	inline Node *debug(Node *newNode, const char *nodeType) {
		return debugFactory_ ? debugFactory_->produce(newNode, nodeType) : newNode;
	}

private:
	friend class ftl::syntax::Debugger;
	Ref<DebugFactory, Owner> debugFactory_;

	friend class InvokeNode;

	int id_;
	const char *name_;
	bool caseSensitive_;

	typedef PrefixTree<char, Ref<DefinitionNode, Owner> > DefinitionByName;
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
	Ref<DefinitionNode, Owner> unresolvedNext_;
	bool statefulScope_;
	bool hasHints_;

	inline int touchFlag(const char *name)
	{
		int id = -1;
		if (!flagIdByName_->lookup(name, &id))
			flagIdByName_->insert(name, id = numFlags_++);
		return id;
	}

	inline int touchCapture(const char *name)
	{
		int id = -1;
		if (!captureIdByName_->lookup(name, &id))
			captureIdByName_->insert(name, id = numCaptures_++);
		return id;
	}

	int numFlags_;
	int numCaptures_;

	typedef PrefixTree<char, int> StateIdByName;

	Ref<StateIdByName, OnDemand> flagIdByName_;
	Ref<StateIdByName, OnDemand> captureIdByName_;

	static int errorCallBack(Ref<Instance> self, ByteArray *media, int index, State *state);
};

} // namespace syntax

typedef syntax::DefinitionNode SyntaxDefinitionNode;

} // namespace ftl

#endif // FTL_SYNTAX_HPP
