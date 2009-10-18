/*
 * Alien.hpp -- taking references to objects not inherited from 'Instance'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ALIEN_HPP
#define PONA_ALIEN_HPP

#include "visibility.hpp"
#include "Instance.hpp"

namespace pona
{

/** Wrapper class to allow taking reference to classes not inherited
  * from pona::Instance. Please note, that it is impossible to reference
  * the same alien object by two different alien wrappers.
  */
template<class T>
class PONA_API Alien: public Instance
{
public:
	typedef void (*DelFunc)(T*);

	Alien(T* external, DelFunc delFunc = 0)
		: external_(external),
		  delFunc_(delFunc)
	{}
	
	~Alien()
	{
		if (external_ != 0)
		{
			if (delFunc_) delFunc_(external_);
			else delete external_;
			external_ = 0;
		}
	}
	
	inline T* get() const { return external_; }
	
private:
	mutable T* external_;
	DelFunc delFunc_;
};


} // namespace pona

#endif // PONA_ALIEN_HPP
