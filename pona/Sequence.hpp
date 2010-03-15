/*
 * Sequence.hpp -- interface of a sequence of items
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_SEQUENCE_HPP
#define PONA_SEQUENCE_HPP

#include "Instance.hpp"

namespace pona
{

template<class Item, class Index = int>
class Sequence: public Instance
{
public:
	virtual ~Sequence() {}
	virtual bool def(Index i) const = 0;
	virtual Item get(Index i) const = 0;
};

} // namespace pona

#endif // PONA_SEQUENCE_HPP
