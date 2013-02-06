 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_STRINGTRAP_HPP
#define FTL_STRINGTRAP_HPP

namespace ftl
{

class StringTrap: public Instance
{
public:
	inline static Ref<StringTrap> create() {
		return new StringTrap;
	}

	inline const char *capture(ByteArray *s) const {
		return capture(s->constData(), s->size());
	}

	inline const char *capture(ByteArray *s, int i0, int i1) const {
		FTL_ASSERT((0 <= i0) && (i0 < s->size()));
		FTL_ASSERT((0 <= i1) && (i1 <= s->size()));
		FTL_ASSERT(i0 <= i1);
		return capture(s->constData() + i0, i1 - i0);
	}

	inline const char *capture(const char *s, int len = -1) const {
		if (len < 0) len = str::len(s);
		if (!head_) head_ = tail_ = new Node(s, len);
		else tail_ = tail_->next_ = new Node(s, len);
		return tail_->s_->data();
	}

private:
	StringTrap(): head_(0), tail_(0) {}

	class Node: public Instance {
	public:
		Node(const char *s, int len): s_(ByteArray::copy(s, len)) {}
		Ref<ByteArray> s_;
		Ref<Node> next_;
	};
	mutable Ref<Node> head_;
	mutable Node *tail_;
};

} // namespace ftl

#endif // FTL_STRINGTRAP_HPP
