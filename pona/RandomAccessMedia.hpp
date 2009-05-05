/*
 * RandomAccessMedia.hpp -- interface of random access containers
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_RANDOMACCESSMEDIA_HPP
#define PONA_RANDOMACCESSMEDIA_HPP

#include "atom"

namespace pona
{

template<class T>
class RandomAccessMedia: public virtual Instance
{
public:
	typedef T Element;
	
	virtual ~RandomAccessMedia() {}
	virtual T get(int i) const = 0;
	virtual void set(int i, T e) = 0;
	virtual int length() const = 0;
};

} // namespace pona

#endif // PONA_RANDOMACCESSMEDIA_HPP

