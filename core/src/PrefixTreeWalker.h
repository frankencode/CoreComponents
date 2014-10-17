/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PREFIXTREEWALKER_H
#define FLUX_PREFIXTREEWALKER_H

namespace flux {

template<class Char, class Value>
class PrefixTree;

template<class Char, class Value>
class PrefixTreeWalker
{
public:
	PrefixTreeWalker() {}

	// prefix increment / decrement
	inline PrefixTreeWalker &operator++() {
		FLUX_ASSERT(valid());
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

} // namespace flux

#endif // FLUX_PREFIXTREEWALKER_H
