/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYNTAX_H
#define FLUX_SYNTAX_H

#include "Crc32.h"
#include "PrefixTree.h"
#include "SyntaxNode.h"
#include "SyntaxDebugFactory.h"
#include "exceptions.h"
#include "Format.h"

namespace flux
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			char ch = text->at(i++);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			char ch = text->at(i++);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			char ch = text->at(i++);
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
	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return text->has(i) ? i + 1 : -1;
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			char ch = text->at(i++);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			char ch = text->at(i++);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			int k = 0, len = s_->size();
			while ((k < len) && (text->has(i))) {
				char ch = text->at(i++);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		if (text->has(i)) {
			int h = 0;
			int keyword = -1;
			if (map_->match(text, i, &h, &keyword, caseSensitive_)) {
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int repeatCount = 0;
		int h = i;
		while ((repeatCount < maxRepeat_) && (h != -1))
		{
			h = entry()->matchNext(text, h, parentToken, state);
			if (h == i)
				FLUX_DEBUG_ERROR("Repeated empty match, bailing out");
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
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
						j = succ->matchNext(text, j, parentToken, state);
						if (j == -1) break;
						succ = succ->succ();
					}
					rollBack(parentToken, lastChildSaved2);
				}
				if (j != -1) return h;
			}
			h = entry()->matchNext(text, h, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
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
			h = entry()->matchNext(text, h, parentToken, state);
			if (h == i)
				FLUX_DEBUG_ERROR("Repeated empty match, bailing out");
			if (h != -1) {
				++repeatCount;
				if (minRepeat_ <= repeatCount) {
					Node *succ = Node::succ();
					if (succ) {
						Token *lastChildSaved3 = 0;
						if (parentToken) lastChildSaved3 = parentToken->lastChild();
						j = h;
						while (succ) {
							j = succ->matchNext(text, j, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = entry()->matchNext(text, i, parentToken, state);
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
	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return (i == 0) ? i : -1;
	}
	inline int matchLength() const { return 0; }
};

class EoiNode: public Node
{
public:
	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		bool eoi = (!text->has(i)) && ((i == 0) || (text->has(i - 1)));
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		bool found = false;
		while (text->has(i) || text->has(i - 1)) {
			int h = entry()->matchNext(text, i, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = i;
		if (entry())
			h = entry()->matchNext(text, i, parentToken, state);

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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = i;
		if (!text->has(i - length_))
			h = -1;
		else if (entry()->matchNext(text, i - length_, parentToken, state) == -1)
			h = -1;

		if ((h == -1) ^ invert_)
			i = -1;

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
	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int h = -1;
		Node *node = Node::firstChild();
		while ((node) && (h == -1)) {
			if (state) {
				if (state->finalize_) {
					h = -1;
					break;
				}
			}
			h = node->matchNext(text, i, parentToken, state);
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
	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		Node *node = Node::firstChild();
		while ((node) && (i != -1)) {
			if (state) {
				if (state->finalize_) {
					i = -1;
					break;
				}
			}
			i = node->matchNext(text, i, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		int h = entry()->matchNext(text, i, parentToken, state);
		if (h == -1 && !state->finalize_) {
			state->hint_ = message_;
			state->hintOffset_ = i;
			state->finalize_ = true;
		}
		return h;
	}

	inline int matchLength() const { return 0; }

	inline Node *entry() const { return Node::firstChild(); }
	inline const char *message() const { return message_; }

private:
	const char *message_;
};

typedef int (*CallBack) (Object *self, ByteArray *text, int i, State *state);

class CallNode: public Node
{
public:
	CallNode(CallBack callBack, Object *self)
		: callBack_(callBack),
		  self_(self)
	{}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return callBack_(self_, text, i, state);
	}

	inline CallBack callBack() const { return callBack_; }

private:
	CallBack callBack_;
	Object *self_;
};

class SetNode: public Node
{
public:
	SetNode(DefinitionNode *scope, int flagId, bool value)
		: scope_(scope),
		  flagId_(flagId),
		  value_(value)
	{}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		state->setFlag(scope_, flagId_, value_);
		return i;
	}

	inline DefinitionNode *scope() const { return scope_; }
	inline int flagId() const { return flagId_; }
	inline bool value() const { return value_; }

private:
	DefinitionNode *scope_;
	int flagId_;
	bool value_;
};

class IfNode: public Node
{
public:
	IfNode(DefinitionNode *scope, int flagId, Node *trueBranch, Node *falseBranch)
		: scope_(scope),
		  flagId_(flagId)
	{
		appendChild(trueBranch);
		appendChild(falseBranch);
	}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return state->flag(scope_, flagId_) ?
			trueBranch()->matchNext(text, i, parentToken, state) :
			falseBranch()->matchNext(text, i, parentToken, state);
	}

	inline DefinitionNode *scope() const { return scope_; }
	inline int flagId() const { return flagId_; }
	inline Node *trueBranch() const { return Node::firstChild(); }
	inline Node *falseBranch() const { return Node::lastChild(); }

private:
	DefinitionNode *scope_;
	int flagId_;
};

class CaptureNode: public Node
{
public:
	CaptureNode(DefinitionNode *scope, int captureId, Node *coverage)
		: scope_(scope),
		  captureId_(captureId)
	{
		appendChild(coverage);
	}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Token *lastChildSaved = 0;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int i0 = i;
		i = coverage()->matchNext(text, i, parentToken, state);

		if (i == -1)
			rollBack(parentToken, lastChildSaved);
		else
			state->setCapture(scope_, captureId_, Range::create(i0, i));

		return i;
	}

	inline DefinitionNode *scope() const { return scope_; }
	inline int captureId() const { return captureId_; }
	inline Node *coverage() const { return Node::firstChild(); }

private:
	DefinitionNode *scope_;
	int captureId_;
};

class ReplayNode: public Node
{
public:
	ReplayNode(DefinitionNode *scope, int captureId)
		: scope_(scope),
		  captureId_(captureId)
	{}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Range *range = state->capture(scope_, captureId_);
		for (int j = range->i0(); (j < range->i1()) && text->has(i) && text->has(j); ++i, ++j) {
			if (text->at(i) != text->at(j)) return -1;
		}
		return i;
	}

	inline DefinitionNode *scope() const { return scope_; }
	inline int captureId() const { return captureId_; }

private:
	DefinitionNode *scope_;
	int captureId_;
};

class RefNode;
class DefinitionNode;

class RuleNode: public Node
{
public:
	RuleNode(const char *scopeName, const char *name, int scope, int id, Node *entry)
		: scopeName_(scopeName),
		  name_(name),
		  scope_(scope),
		  id_(id),
		  used_(false),
		  numberOfRefs_(-1)
	{
		appendChild(entry);
	}

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		Ref<Token> token = state->produceToken(scope_, id_, scopeName_, name_);
		if (parentToken)
			parentToken->appendChild(token);

		int i0 = i;
		i = (entry()) ? entry()->matchNext(text, i, token, state) : i;

		if (token) {
			if (i != -1)
				token->setRange(i0, i);
			else
				token->unlink();
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

	inline bool used() const { return used_; }
	inline void markUsed() { used_ = true; }

protected:
	friend class InlineNode;

	const char *scopeName_;
	const char *name_;
	int scope_;
	int id_;
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return LinkNode::rule_->matchNext(text, i, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		return LinkNode::rule_->entry()->matchNext(text, i, parentToken, state);
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
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

	virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const
	{
		int h = -1;

		if (parentToken) {
			Token *contextToken = parentToken->parent();
			if (contextToken) {
				Node *entry = (contextToken->rule() == LinkNode::rule_->id()) ? inContext() : outOfContext();
				if (entry) {
					Token *lastChildSaved = 0;
					if (parentToken) lastChildSaved = parentToken->lastChild();

					h = entry->matchNext(text, i, parentToken, state);

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
class DebugFactory;

class DefinitionNode: public RefNode
{
public:
	DefinitionNode(DebugFactory *debugFactory = 0)
		: debugFactory_(debugFactory),
		  id_(Crc32().sum()),
		  name_(0),
		  caseSensitive_(true),
		  scopeByName_(ScopeByName::create()),
		  numRules_(0),
		  numKeywords_(0),
		  ruleByName_(RuleByName::create()),
		  keywordByName_(KeywordByName::create()),
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
			FLUX_DEBUG_ERROR("Cannot import anonymous syntax definition");
		scopeByName_->insert(name, definition);
	}

	typedef Ref<Node> NODE;
	typedef Ref<RuleNode> RULE;

	inline void OPTION(const char *name, bool value) {
		if (strcasecmp(name, "caseSensitive") == 0)
			caseSensitive_ = value;
		else
			FLUX_DEBUG_ERROR(Format("Unknown option '%%'") << name);
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
		Ref<RuleNode> rule = new RuleNode(name_, ruleName, id_, numRules_++, entry);
		addRule(rule);
		return rule->id();
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

	typedef int (*CallBack) (Object *self, ByteArray *text, int i, State *state);

	inline NODE CALL(CallBack callBack, Object *self = 0) {
		if (!self) self = this;
		return debug(new CallNode(callBack, self), "Call");
	}
	inline NODE ERROR() {
		return debug(new CallNode(errorCallBack, this), "Call");
	}

	void LINK();

	//-- stateful definition interface

	inline NODE SET(const char *name, bool value) {
		return debug(new SetNode(this, touchFlag(name), value), "Set");
	}
	inline NODE IF(const char *name, NODE trueBranch, NODE falseBranch = 0) {
		if (!trueBranch) trueBranch = PASS();
		if (!falseBranch) falseBranch = PASS();
		return debug(new IfNode(this, touchFlag(name), trueBranch, falseBranch), "If");
	}
	inline NODE CAPTURE(const char *name, NODE coverage) {
		return debug(new CaptureNode(this, touchCapture(name), coverage), "Capture");
	}
	inline NODE REPLAY(const char *name) {
		return debug(new ReplayNode(this, touchCapture(name)), "Replay");
	}

	//-- execution interface

	inline int numRules() const { return numRules_; }

	inline State *createState(TokenFactory *tokenFactory = 0) const {
		return new State(this, numFlags_, numCaptures_, tokenFactory);
	}

	Ref<State> find(ByteArray *text, int i, TokenFactory *tokenFactory = 0) const;
	Ref<State> match(ByteArray *text, int i = -1, TokenFactory *tokenFactory = 0) const;

	const DefinitionNode *resolveScope(const char *&name) const;

	inline DefinitionNode *scopeByName(const char *name) const
	{
		Ref<DefinitionNode> definition;
		const DefinitionNode *scope = resolveScope(name);
		if (!scope->scopeByName_->lookup(name, &definition))
			FLUX_DEBUG_ERROR(Format("Undefined definition '%%'") << name);
		return definition;
	}

	inline RuleNode *ruleByName(const char *name) const
	{
		const DefinitionNode *scope = resolveScope(name);
		Ref<RuleNode> node;
		FLUX_ASSERT(scope);
		if (!scope->ruleByName_->lookup(name, &node))
			FLUX_DEBUG_ERROR(Format("Undefined rule '%%'") << name);
		return node;
	}

	inline int keywordByName(const char *keyword) const
	{
		int tokenType = -1;
		if (!keywordByName_->lookup(keyword, &tokenType))
			FLUX_DEBUG_ERROR(Format("Undefined keyword '%%'") <<  keyword);
		return tokenType;
	}

	inline int flagIdByName(const char *name) const
	{
		int flagId = -1;
		if (!flagIdByName_->lookup(name, &flagId))
			FLUX_DEBUG_ERROR(Format("Undefined state flag '%%'") << name);
		return flagId;
	}

	inline int captureIdByName(const char *name) const
	{
		int captureId = -1;
		if (!captureIdByName_->lookup(name, &captureId))
			FLUX_DEBUG_ERROR(Format("Undefined capture '%%'") << name);
		return captureId;
	}

	virtual int syntaxError(ByteArray *text, int index, State *state) const;

	inline Node *debug(Node *newNode, const char *nodeType) {
		return debugFactory_ ? debugFactory_->produce(newNode, nodeType) : newNode;
	}

private:
	friend class Debugger;
	Ref<DebugFactory> debugFactory_;

	int id_;
	const char *name_;
	bool caseSensitive_;

	typedef PrefixTree<char, Ref<DefinitionNode> > ScopeByName;
	Ref<ScopeByName> scopeByName_;

	typedef PrefixTree<char, Ref<RuleNode> > RuleByName;
	typedef PrefixTree<char, int> KeywordByName;

	int numRules_;
	int numKeywords_;
	Ref<RuleByName> ruleByName_;
	Ref<KeywordByName> keywordByName_;

	void addRule(RuleNode *rule)
	{
		if (!ruleByName_->insert(rule->name(), rule))
			FLUX_DEBUG_ERROR(Format("Redefinition of rule '%%'") << rule->name());
	}

	Ref<LinkNode> unresolvedLinkHead_;
	Ref<PreviousNode> unresolvedKeywordHead_;
	Ref<DefinitionNode> unresolvedNext_;
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

	static int errorCallBack(Object *self, ByteArray *text, int index, State *state);
};

} // namespace syntax

typedef syntax::DefinitionNode SyntaxDefinitionNode;

} // namespace flux

#endif // FLUX_SYNTAX_H
