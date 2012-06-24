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

#include "PrefixTree.hpp"
#include "SyntaxNode.hpp"

namespace ftl
{

namespace syntax
{

typedef SyntaxNode Node;

class CharNode: public Node
{
public:
	CharNode(char ch, int invert)
		: ch_(ch),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	RangeExplicitNode(const char* s, int invert)
		: s_(s),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
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

		return i;
	}

	inline int matchLength() const { return 1; }

	inline const ByteArray& s() const { return s_; }
	inline int invert() const { return invert_; }

private:
	ByteArray s_;
	int invert_;
};

class StringNode: public Node
{
public:
	StringNode(const char* s, bool caseSensitive)
		: s_(s),
		  caseSensitive_(caseSensitive)
	{
		if (!caseSensitive) {
			for (int i = 0, n = s_.size(); i < n; ++i)
				s_.set(i, ToLower<char>::map(s_.at(i)));
		}
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		if (media->has(i)) {
			int k = 0, len = s_.length();
			while ((k < len) && (media->has(i))) {
				char ch = media->get(i++);
				if (!caseSensitive_)
					ch = ToLower<char>::map(ch);
				if (s_.at(k) != ch) break;
				++k;
			}
			if (k != len)
				i = -1;
		}
		else
			i = -1;

		return i;
	}

	inline int matchLength() const { return s_.length(); }

	inline const ByteArray& s() const { return s_; }

private:
	ByteArray s_;
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		return (i == 0) ? i : -1;
	}
	inline int matchLength() const { return 0; }
};

class EoiNode: public Node
{
public:
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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
	HintNode(const char* text)
		: text_(text)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		if ((!text_) || (!state->hint())) {
			state->setHint(text_);
			state->setHintOffset(i);
		}
		return i;
	}

	inline int matchLength() const { return 0; }

	inline const char* text() const { return text_; }

private:
	const char* text_;
};

typedef int (*CallBack) (Ref<Instance> self, ByteArray* media, int i, SyntaxState* state);

class CallNode: public Node
{
public:
	CallNode(CallBack callBack, Ref<Instance> self)
		: callBack_(callBack),
		  self_(self)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
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

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		if (media->has(i))
			*state->character(charId_) = media->get(i++);
		else
			i = -1;

		return i;
	}

	inline int charId() const { return charId_; }

private:
	int charId_;
};

class SetCharNode: public Node
{
public:
	SetCharNode(int charId, char value)
		: charId_(charId),
		  value_(value)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		*state->character(charId_) = value_;
		return i;
	}

	inline int charId() const { return charId_; }
	inline char value() const { return value_; }

private:
	int charId_;
	char value_;
};

class VarCharNode: public Node
{
public:
	VarCharNode(int charId, int invert)
		: charId_(charId),
		  invert_(invert)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		if (media->has(i)) {
			char ch = media->get(i++);
			if ((ch != *state->character(charId_)) ^ invert_)
				i = -1;
		}
		else
			i = -1;

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
		: stringId_(stringId)
	{
		appendChild(coverage);
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int i0 = i;
		i = coverage()->matchNext(media, i, 0, parentToken, state);

		if (i != -1)
		{
			rollBack(parentToken, lastChildSaved);

			ByteArray* string = state->string(stringId_);
			*string = ByteArray(i - i0);
			for (int k = i0; k < i; ++k)
				string->set(k - i0, media->get(k));
		}

		return i;
	}

	inline int stringId() const { return stringId_; }
	inline Ref<Node> coverage() const { return Node::firstChild(); }

private:
	int stringId_;
};

class SetStringNode: public Node
{
public:
	SetStringNode(int stringId, const char* s)
		: stringId_(stringId),
		  s_(str::len(s))
	{
		str::cpy(s_.data(), s, s_.size());
	}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		*state->string(stringId_) = s_;
		return i;
	}

	inline int stringId() const { return stringId_; }
	inline const ByteArray& value() const { return s_; }

private:
	int stringId_;
	ByteArray s_;
};

class VarStringNode: public Node
{
public:
	VarStringNode(int stringId)
		: stringId_(stringId)
	{}

	virtual int matchNext(ByteArray* media, int i, TokenFactory* tokenFactory, Token* parentToken, SyntaxState* state) const
	{
		ByteArray* string = state->string(stringId_);
		int k = 0, m = string->length();
		while (media->has(i) && (k < m)) {
			if (media->get(i) != string->get(k))
				break;
			++i;
			++k;
		}

		if (k != m)
			i = -1;

		return i;
	}

	inline int stringId() const { return stringId_; }

private:
	int stringId_;
};

} // namespace syntax

} // namespace ftl

#endif // FTL_SYNTAX_HPP
