/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SYNTAX_H
#define FKIT_SYNTAX_H

#include "Crc32.h"
#include "PrefixTree.h"
#include "SyntaxNode.h"
#include "SyntaxDebugFactory.h"

namespace fkit
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
			char ch = media->at(i++);
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
			char ch = media->at(i++);
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
			char ch = media->at(i++);
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
			char ch = media->at(i++);
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
		: s_(ByteArray::copy(s)),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			char ch = media->at(i++);
			int k = 0, len = s_->size();
			while (k < len) {
				if (s_->at(k) == ch) break;
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
	Ref<ByteArray> s_;
	int invert_;
};

class StringNode: public Node
{
public:
	StringNode(const char *s, bool caseSensitive)
		: s_(ByteArray::copy(s)),
		  caseSensitive_(caseSensitive)
	{
		if (!caseSensitive) s_->downcaseInsitu();
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		if (media->has(i)) {
			int k = 0, len = s_->size();
			while ((k < len) && (media->has(i))) {
				char ch = media->at(i++);
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

	inline int matchLength() const { return s_->size(); }

	inline const ByteArray &s() const { return *s_; }

private:
	Ref<ByteArray> s_;
	bool caseSensitive_;
};

typedef PrefixTree<char, int> KeywordMap;

class KeywordNode: public Node
{
public:
	KeywordNode(KeywordMap *map, bool caseSensitive)
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

	inline KeywordMap *map() const { return map_; }

private:
	Ref<KeywordMap> map_;
	bool caseSensitive_;
};

class RepeatNode: public Node
{
public:
	RepeatNode(int minRepeat, int maxRepeat, Node *entry)
		: minRepeat_(minRepeat),
		  maxRepeat_(maxRepeat)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int repeatCount = 0;
		int h = i;
		while ((repeatCount < maxRepeat_) && (h != -1))
		{
			h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
			if (h == i)
				FKIT_THROW(DebugException, "Repeated empty match, bailing out");
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
	inline Node *entry() const { return Node::firstChild(); }

private:
	int minRepeat_;
	int maxRepeat_;
};

class LazyRepeatNode: public Node
{
public:
	LazyRepeatNode(int minRepeat, Node *entry)
		: minRepeat_(minRepeat)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int repeatCount = 0;
		int h = i;

		while (h != -1)
		{
			if (minRepeat_ <= repeatCount) {
				int j = h;
				Node *succ = Node::succ();
				if (succ) {
					Token *lastChildSaved2 = 0;
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
			h = entry()->matchNext(media, h, tokenFactory, parentToken, state);
			repeatCount += (h != -1);
		}

		rollBack(parentToken, lastChildSaved);
		return -1;
	}

	inline int matchLength() const { return -1; }

	inline int minRepeat() const { return minRepeat_; }
	inline Node *entry() const { return Node::firstChild(); }

private:
	int minRepeat_;
};

class GreedyRepeatNode: public Node
{
public:
	GreedyRepeatNode(int minRepeat, int maxRepeat, Node *entry)
		: minRepeat_(minRepeat),
		  maxRepeat_(maxRepeat)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0, *lastChildSaved2 = 0;
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
				FKIT_THROW(DebugException, "Repeated empty match, bailing out");
			if (h != -1) {
				++repeatCount;
				if (minRepeat_ <= repeatCount) {
					Node *succ = Node::succ();
					if (succ) {
						Token *lastChildSaved3 = 0;
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
	inline Node *entry() const { return Node::firstChild(); }

private:
	int minRepeat_;
	int maxRepeat_;
};

class LengthNode: public Node
{
public:
	LengthNode(int minLength, int maxLength, Node *entry)
		: minLength_(minLength),
		  maxLength_(maxLength)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
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
	inline Node *entry() const { return Node::firstChild(); }

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
	FindNode(Node *entry)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
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

	inline Node *entry() const { return Node::firstChild(); }
};

class AheadNode: public Node
{
public:
	AheadNode(Node *entry, int invert)
		: invert_(invert)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = i;
		if (entry())
			h = entry()->matchNext(media, i, tokenFactory, parentToken, state);

		if ((h == -1) ^ invert_)
			i = -1;

		rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const { return 0; }

	inline Node *entry() const { return Node::firstChild(); }
	inline int invert() const { return invert_; }

private:
	int invert_;
};

class BehindNode: public Node
{
public:
	BehindNode(Node *entry, int invert)
		: invert_(invert),
		  length_(entry->matchLength())
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = i;
		if (!media->has(i - length_))
			h = -1;
		else if (entry()->matchNext(media, i - length_, tokenFactory, parentToken, state) == -1)
			h = -1;

		i = ((h == -1) ^ invert_) ? h : i;

		rollBack(parentToken, lastChildSaved);

		return i;
	}

	inline int matchLength() const { return 0; }

	inline Node *entry() const { return Node::firstChild(); }
	inline int invert() const { return invert_; }
	inline int size() const { return length_; }

private:
	int invert_;
	int length_;
};

class ChoiceNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = -1;
		Node *node = Node::firstChild();
		while ((node) && (h == -1)) {
			if (state) {
				if (state->finalize()) {
					h = -1;
					break;
				}
			}
			h = node->matchNext(media, i, tokenFactory, parentToken, state);
			if (h == -1)
				rollBack(parentToken, lastChildSaved);
			node = node->nextSibling();
		}

		if (h == -1)
			rollBack(parentToken, lastChildSaved);

		return h;
	}

	virtual Node *succ(Node *node) const
	{
		return Node::parent() ? Node::parent()->succ(Node::self()) : null<Node>();
	}

	virtual int matchLength() const
	{
		int len = -1;
		for (Node *node = Node::firstChild(); node; node = Node::nextSibling()) {
			int len2 = node->matchLength();
			if ((len != -1) && (len2 != len))
				return -1;
			len = len2;
		}
		return len;
	}

	inline Node *firstChoice() const { return Node::firstChild(); }
	inline Node *lastChoice() const { return Node::lastChild(); }
};

class GlueNode: public Node
{
public:
	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		Node *node = Node::firstChild();
		while ((node) && (i != -1)) {
			if (state) {
				if (state->finalize()) {
					i = -1;
					break;
				}
			}
			i = node->matchNext(media, i, tokenFactory, parentToken, state);
			node = node->nextSibling();
		}

		if (i == -1)
			rollBack(parentToken, lastChildSaved);

		return i;
	}

	virtual Node *succ(Node *node) const
	{
		Node *succ = node->nextSibling();
		if ((!succ) && (Node::parent())) succ = Node::parent()->succ(Node::self());
		return succ;
	}

	virtual int matchLength() const
	{
		int len = 0;
		for (Node *node = Node::firstChild(); node; node = node->nextSibling()) {
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

class ExpectNode: public Node
{
public:
	ExpectNode(const char *message, Node *entry)
		: message_(message)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		int h = entry()->matchNext(media, i, tokenFactory, parentToken, state);
		if (h == -1 && !state->finalize()) {
			state->setHint(message_);
			state->setHintOffset(i);
			state->setFinalize();
		}
		return h;
	}

	inline int matchLength() const { return 0; }

	inline Node *entry() const { return Node::firstChild(); }
	inline const char *message() const { return message_; }

private:
	const char *message_;
};

typedef int (*CallBack) (Object *self, ByteArray *media, int i, State *state);

class CallNode: public Node
{
public:
	CallNode(CallBack callBack, Object *self)
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
	Object *self_;
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
	IfNode(int flagId, Node *trueBranch, Node *falseBranch)
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
	inline Node *trueBranch() const { return Node::firstChild(); }
	inline Node *falseBranch() const { return Node::lastChild(); }

private:
	int flagId_;
};

class CaptureNode: public Node
{
public:
	CaptureNode(int captureId, Node *coverage)
		: captureId_(captureId)
	{
		appendChild(coverage);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int i0 = i;
		i = coverage()->matchNext(media, i, tokenFactory, parentToken, state);

		if (i != -1) {
			rollBack(parentToken, lastChildSaved);
			state->setCapture(captureId_, Range::create(i0, i));
		}

		return i;
	}

	inline int captureId() const { return captureId_; }
	inline Node *coverage() const { return Node::firstChild(); }

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
		Range *range = state->capture(captureId_);
		for (int j = range->i0(); (j < range->i1()) && media->has(i) && media->has(j); ++i, ++j) {
			if (media->at(i) != media->at(j)) return -1;
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
	RuleNode(int definitionId, const char *name, int ruleId, Node *entry, bool isVoid = false)
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
		Ref<Token> token;
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

	inline int matchLength() const { return entry()->matchLength(); }

	int numberOfRefs() {
		if (numberOfRefs_ == -1) {
			numberOfRefs_ = 0;
			for (Node *node = Node::first(); node; node = node->next())
				if (cast<RefNode>(node)) ++numberOfRefs_;
		}
		return numberOfRefs_;
	}

	inline int id() const { return id_; }
	inline const char *name() const { return name_; }

	inline Node *entry() const { return Node::firstChild(); }
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

	LinkNode(RuleNode *rule)
		: ruleName_(rule->name()),
		  rule_(rule)
	{}

	inline const char *ruleName() const { return ruleName_; }
	inline RuleNode *rule() const { return rule_; }

	inline int matchLength() const { return rule_->matchLength(); }

protected:
	friend class DefinitionNode;
	friend class Debugger;

	const char *ruleName_;
	RuleNode *rule_;
	Ref<LinkNode> unresolvedNext_;
};

class RefNode: public LinkNode
{
public:
	RefNode(const char *ruleName = 0)
		: LinkNode(ruleName)
	{}

	RefNode(RuleNode *rule)
		: LinkNode(rule)
	{}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
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

	InlineNode(RuleNode *rule)
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
			Token *sibling = parentToken->previousSibling();
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
	Ref<PreviousNode> unresolvedKeywordNext_;
};

class ContextNode: public LinkNode
{
public:
	ContextNode(const char *ruleName, Node *inContext, Node *outOfContext)
		: LinkNode(ruleName)
	{
		appendChild(inContext);
		appendChild(outOfContext);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
	{
		int h = -1;

		if (parentToken) {
			Token *contextToken = parentToken->parent();
			if (contextToken) {
				Node *entry = (contextToken->rule() == LinkNode::rule_->id()) ? inContext() : outOfContext();
				if (entry) {
					Token *lastChildSaved = 0;
					if (parentToken) lastChildSaved = parentToken->lastChild();

					h = entry->matchNext(media, i, tokenFactory, parentToken, state);

					if (h == -1)
						rollBack(parentToken, lastChildSaved);
				}
			}
		}

		return h;
	}

	inline Node *inContext() const { return Node::firstChild(); }
	inline Node *outOfContext() const { return Node::lastChild(); }
};

class DefinitionNode;
class InvokeNode;
class DebugFactory;

class InvokeNode: public Node
{
public:
	InvokeNode(DefinitionNode *definition, Node *coverage)
		: definition_(definition)
	{
		if (coverage) appendChild(coverage);
	}

	InvokeNode(const char *name, Node *coverage)
		: definitionName_(name)
	{
		if (coverage) appendChild(coverage);
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const;

	inline const char *definitionName() const { return definitionName_; }
	inline Node *coverage() const { return Node::firstChild(); }

private:
	friend class DefinitionNode;

	const char *definitionName_;
	DefinitionNode *definition_;
	Ref<InvokeNode> unresolvedNext_;
};

class DefinitionNode: public RefNode
{
public:
	DefinitionNode(DebugFactory *debugFactory = 0)
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

	inline DebugFactory *debugFactory() const { return debugFactory_; }

	inline int id() const { return id_; }
	inline const char *name() const { return name_; }

	//-- stateless definition interface

	inline void SYNTAX(const char *name) {
		id_ = crc32(name);
		name_ = name;
	}

	inline void IMPORT(DefinitionNode *definition, const char *name = 0) {
		if (!name) name = definition->name();
		if (!name)
			FKIT_THROW(DebugException, "Cannot import anonymous syntax definition");
		definitionByName_->insert(name, definition);
	}

	typedef Ref<Node> NODE;
	typedef Ref<RuleNode> RULE;

	inline void OPTION(const char *name, bool value) {
		if (strcasecmp(name, "caseSensitive") == 0)
			caseSensitive_ = value;
		else
			FKIT_THROW(DebugException, strcat("Unknown option '", name, "'"));
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

	#include "SyntaxSugar.h"

	inline NODE EXPECT(const char *text, NODE entry) {
		hasHints_ = true;
		return debug(new ExpectNode(text, entry), "Expect");
	}

	inline int DEFINE(const char *ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode> rule = new RuleNode(id_, ruleName, numRules_++, entry);
		addRule(rule);
		return rule->id();
	}
	inline void DEFINE_VOID(const char *ruleName, NODE entry = 0) {
		if (!entry) entry = PASS();
		Ref<RuleNode> rule = new RuleNode(id_, ruleName, numRules_++, entry, true);
		addRule(rule);
	}
	inline void ENTRY(const char *ruleName) {
		LinkNode::ruleName_ = ruleName;
	}

	inline NODE REF(const char *ruleName) {
		Ref<RefNode> link = new RefNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Ref");
	}
	inline NODE INLINE(const char *ruleName) {
		Ref<InlineNode> link = new InlineNode(ruleName);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Inline");
	}
	inline NODE PREVIOUS(const char *ruleName, const char *keyword = 0) {
		Ref<PreviousNode> link = new PreviousNode(ruleName, keyword);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		if (keyword) {
			link->unresolvedKeywordNext_ = unresolvedKeywordHead_;
			unresolvedKeywordHead_ = link;
		}
		return debug(link, "Previous");
	}
	inline NODE CONTEXT(const char *ruleName, NODE inContext = 0, NODE outOfContext = 0) {
		if (!inContext) inContext = PASS();
		if (!outOfContext) outOfContext = FAIL();
		Ref<ContextNode> link = new ContextNode(ruleName, inContext, outOfContext);
		link->unresolvedNext_ = unresolvedLinkHead_;
		unresolvedLinkHead_ = link;
		return debug(link, "Context");
	}

	typedef int (*CallBack) (Object *self, ByteArray *media, int i, State *state);

	inline NODE CALL(CallBack callBack, Object *self = 0) {
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
		Ref<InvokeNode> node = new InvokeNode(definitionName, coverage);
		node->unresolvedNext_ = unresolvedInvokeHead_;
		unresolvedInvokeHead_ = node;
		return debug(node, "Invoke");
	}

	//-- execution interface

	inline int numRules() const { return numRules_; }

	inline bool stateful() const { return (numFlags_ > 0) || (numCaptures_ > 0) || statefulScope_ || hasHints_; }

	State *newState(State *parent = 0) const;

	Ref<Token> find(ByteArray *media, int *i0, int *i1 = 0, TokenFactory *tokenFactory = 0) const;
	Ref<Token> match(ByteArray *media, int i0 = 0, int *i1 = 0, State *state = 0, TokenFactory *tokenFactory = 0) const;

	const DefinitionNode *resolveScope(const char*& name) const;

	inline DefinitionNode *definitionByName(const char *name) const
	{
		Ref<DefinitionNode> definition;
		const DefinitionNode *scope = resolveScope(name);
		if (!scope->definitionByName_->lookup(name, &definition))
			FKIT_THROW(DebugException, strcat("Undefined definition '", name, "' referenced"));
		return definition;
	}

	inline RuleNode *ruleByName(const char *name) const
	{
		const DefinitionNode *scope = resolveScope(name);
		Ref<RuleNode> node;
		FKIT_ASSERT(scope);
		if (!scope->ruleByName_->lookup(name, &node))
			FKIT_THROW(DebugException, strcat("Undefined rule '", name, "' referenced"));
		return node;
	}

	inline int keywordByName(const char *keyword) const
	{
		int tokenType = -1;
		if (!keywordByName_->lookup(keyword, &tokenType))
			FKIT_THROW(DebugException, strcat("Undefined keyword '", keyword, "' referenced"));
		return tokenType;
	}

	inline int flagIdByName(const char *name) const
	{
		int flagId = -1;
		if (!flagIdByName_->lookup(name, &flagId))
			FKIT_THROW(DebugException, strcat("Undefined state flag '", name, "' referenced"));
		return flagId;
	}

	inline int captureIdByName(const char *name) const
	{
		int captureId = -1;
		if (!captureIdByName_->lookup(name, &captureId))
			FKIT_THROW(DebugException, strcat("Undefined capture '", name, "' referenced"));
		return captureId;
	}

	virtual int syntaxError(ByteArray *media, int index, State *state) const;

	inline Node *debug(Node *newNode, const char *nodeType) {
		return debugFactory_ ? debugFactory_->produce(newNode, nodeType) : newNode;
	}

private:
	friend class Debugger;
	Ref<DebugFactory> debugFactory_;

	friend class InvokeNode;

	int id_;
	const char *name_;
	bool caseSensitive_;

	typedef PrefixTree<char, Ref<DefinitionNode> > DefinitionByName;
	Ref<DefinitionByName> definitionByName_;

	typedef PrefixTree<char, Ref<RuleNode> > RuleByName;
	typedef PrefixTree<char, int> KeywordByName;

	int numRules_;
	int numKeywords_;
	Ref<RuleByName> ruleByName_;
	Ref<KeywordByName> keywordByName_;

	void addRule(RuleNode *rule)
	{
		if (!ruleByName_->insert(rule->name(), rule))
			FKIT_THROW(DebugException, strcat("Redefinition of rule '", rule->name(), "'"));
	}

	Ref<LinkNode> unresolvedLinkHead_;
	Ref<PreviousNode> unresolvedKeywordHead_;
	Ref<InvokeNode> unresolvedInvokeHead_;
	Ref<DefinitionNode> unresolvedNext_;
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

	Ref<StateIdByName> flagIdByName_;
	Ref<StateIdByName> captureIdByName_;

	static int errorCallBack(Object *self, ByteArray *media, int index, State *state);
};

} // namespace syntax

typedef syntax::DefinitionNode SyntaxDefinitionNode;

} // namespace fkit

#endif // FKIT_SYNTAX_H
