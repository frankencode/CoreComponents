/*
 * StringTrap.hpp -- retain copies of strings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_STRINGTRAP_HPP
#define FTL_STRINGTRAP_HPP

#include "atoms"

namespace ftl
{

class StringTrap: public Instance
{
public:
	inline static Ref<StringTrap, Owner> create() {
		return new StringTrap;
	}

	inline const char *capture(Ref<ByteArray> s) const {
		return capture(s->constData(), s->size());
	}

	inline const char *capture(Ref<ByteArray> s, int i0, int i1) const {
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
	StringTrap() {}

	class Node: public Instance {
	public:
		Node(const char *s, int len): s_(ByteArray::create(s, len)) {}
		Ref<ByteArray, Owner> s_;
		Ref<Node, Owner> next_;
	};
	mutable Ref<Node, Owner> head_;
	mutable Ref<Node> tail_;
};

} // namespace ftl

#endif // FTL_STRINGTRAP_HPP
