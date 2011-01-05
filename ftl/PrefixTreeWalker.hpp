/*
 * PrefixTreeWalker.hpp -- prefix tree walker
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	// prefix increment / decrement
	inline PrefixTreeWalker& operator++() {
		check(valid());
		while (node_) {
			node_ = node_->next();
			if (node_)
				if (node_->defined_)
					break;
		}
		return *this;
	}
	inline PrefixTreeWalker& operator--() {
		check(valid());
		while (node_) {
			node_ = node_->previous();
			if (node_)
				if (node_->defined_)
					break;
		}
		return *this;
	}
	inline PrefixTreeWalker& operator+=(int delta) {
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
	inline PrefixTreeWalker& operator-=(int delta) {
		return (*this) += -delta;
	}
	
	// postfix increment / decrement
	inline PrefixTreeWalker operator++(int) {
		PrefixTreeWalker it = *this;
		++(*this);
		return it;
	}
	inline PrefixTreeWalker operator--(int) {
		PrefixTreeWalker it = *this;
		--(*this);
		return it;
	}
	
	inline PrefixTreeWalker operator+(int delta) const {
		PrefixTreeWalker it = *this;
		return it += delta;
	}
	inline PrefixTreeWalker operator-(int delta) const {
		PrefixTreeWalker it = *this;
		return it -= delta;
	}
	
	inline bool operator==(const PrefixTreeWalker& b) const {
		return node_ == b.node_;
	}
	inline bool operator!=(const PrefixTreeWalker& b) const {
		return node_ != b.node_;
	}
	
private:
	friend class PrefixTree<Char, Value>;
	typedef PrefixTree<Char, Value> Node;
	
	PrefixTreeWalker(Ref<Node> node)
		: node_(node)
	{}
	
	inline bool valid() const { return node_; }
	
	Ref<Node> node_;
};

} // namespace ftl

#endif // FTL_PREFIXTREEWALKER_HPP
