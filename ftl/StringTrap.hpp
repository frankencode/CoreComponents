/*
 * StringTrap.hpp -- retain copies of strings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_STRINGTRAP_HPP
#define FTL_STRINGTRAP_HPP

#include "atoms"

namespace ftl
{

class StringTrap: public Instance
{
public:
	inline const char* capture(Ref<ByteArray> s) const {
		return capture(s->constData(), s->size());
	}
	
	inline const char* capture(Ref<ByteArray> s, int i0, int i1) const {
		check((0 <= i0) && (i0 < s->size()));
		check((0 <= i1) && (i1 <= s->size()));
		check(i0 <= i1);
		return capture(s->constData() + i0, i1 - i0);
	}
	
	inline const char* capture(const char* s, int len = -1) const {
		if (len < 0) len = str::len(s);
		if (!head_) head_ = tail_ = new Node(s, len);
		else tail_ = tail_->next_ = new Node(s, len);
		return tail_->s_.data();
	}
	
private:
	class Node: public Instance {
	public:
		Node(const char* s, int len): s_(s, len) {}
		ByteArray s_;
		Ref<Node, Owner> next_;
	};
	mutable Ref<Node, Owner> head_;
	mutable Ref<Node> tail_;
};

} // namespace ftl

#endif // FTL_STRINGTRAP_HPP
