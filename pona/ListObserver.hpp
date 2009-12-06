/*
 * ListObserver.hpp -- tracking changes on a 'List'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LISTOBSERVER_HPP
#define PONA_LISTOBSERVER_HPP

#include "atoms"

namespace pona
{

template<class T>
class ListObserver: public virtual Instance
{
public:
	virtual bool beforePush(int i, int n, const T* v) { return true; }
	virtual void afterPush(int i, int n) {}
	
	virtual bool beforePop(int i, int n) { return true; }
	virtual void afterPop(int i, int n) {}
	
	virtual bool beforeWrite(int i, int n, const T* v) { return true; }
	virtual void afterWrite(int i, int n) {}
};

} // namespace pona

#endif // PONA_LISTOBSERVER_HPP
