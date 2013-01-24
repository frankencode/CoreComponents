 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PREFIXTREEWALKER_HPP
#define FTL_PREFIXTREEWALKER_HPP

namespace ftl
{

template<class Char, class Value>
class PrefixTree;

template<class Char, class Value>
class PrefixTreeWalker
{
public:
	PrefixTreeWalker() {}

	// prefix increment / decrement
	inline PrefixTreeWalker &operator++() {
		FTL_ASSERT(valid());
		while (node_) {
			node_ = node_->next();
			if (node_)
				if (node_->defined_)
					break;
		}
		return *this;
	}
	inline PrefixTreeWalker &operator+=(int delta) {
		while (delta > 0) {
			++*this;
			--delta;
		}
		while (delta < 0) {
			--*this;
			++delta;
		}
		return *this;
	}

	// postfix increment / decrement
	inline PrefixTreeWalker operator++(int) {
		PrefixTreeWalker it = *this;
		++(*this);
		return it;
	}

	inline PrefixTreeWalker operator+(int delta) const {
		PrefixTreeWalker it = *this;
		return it += delta;
	}

	inline bool operator==(const PrefixTreeWalker &b) const {
		return node_ == b.node_;
	}
	inline bool operator!=(const PrefixTreeWalker &b) const {
		return node_ != b.node_;
	}

private:
	friend class PrefixTree<Char, Value>;
	typedef PrefixTree<Char, Value> Node;

	PrefixTreeWalker(Node *node)
		: node_(node)
	{}

	inline bool valid() const { return node_; }

	Node *node_;
};

} // namespace ftl

#endif // FTL_PREFIXTREEWALKER_HPP
