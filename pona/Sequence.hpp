/*
 * Sequence.hpp -- interface of a sequence of items
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_SEQUENCE_HPP
#define PONA_SEQUENCE_HPP

#include "Instance.hpp"

namespace pona
{

template<class T>
class Sequence: public Instance
{
public:
	typedef T Element;
	
	virtual ~Sequence() {}
	virtual bool def(int i) const = 0;
	virtual Element get(int i) const = 0;
	virtual void set(int i, Element e) = 0;
};

template<class T>
class Sequence<const T>: public Instance
{
public:
	typedef T Element;
	
	virtual ~Sequence() {}
	virtual bool def(int i) const = 0;
	virtual const Element get(int i) const = 0;
};

} // namespace pona

#endif // PONA_SEQUENCE_HPP
