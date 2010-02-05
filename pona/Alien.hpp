/*
 * Alien.hpp -- taking references to objects not inherited from 'Instance'
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ALIEN_HPP
#define PONA_ALIEN_HPP

#include "Instance.hpp"

namespace pona
{

/** Wrapper class to allow taking reference to classes not inherited
  * from pona::Instance. Please note, that it is impossible to reference
  * the same alien object by two different alien wrappers.
  */
template<class T>
class Alien: public Instance
{
public:
	typedef void (*DelFunc)(T*);

	Alien(T* guest, DelFunc delFunc = 0)
		: guest_(guest),
		  delFunc_(delFunc)
	{}
	
	~Alien()
	{
		if (guest_ != 0)
		{
			if (delFunc_) delFunc_(guest_);
			else delete guest_;
			guest_ = 0;
		}
	}
	
	inline T* get() const { return guest_; }
	
private:
	mutable T* guest_;
	DelFunc delFunc_;
};


} // namespace pona

#endif // PONA_ALIEN_HPP
