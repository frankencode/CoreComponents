/*
 * ListObserver.hpp -- tracking changes on a 'List'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LISTOBSERVER_HPP
#define PONA_LISTOBSERVER_HPP

#include "atom"

namespace pona
{

class ListObserver: public virtual Instance
{
public:
	virtual void beforePush(int i, int n) {}
	virtual void afterPush(int i, int n) {}
	
	virtual void beforePop(int i, int n) {}
	virtual void afterPop(int i, int n) {}
	
	virtual void beforeWrite(int i, int n) {}
	virtual void afterWrite(int i, int n) {}
};

} // namespace pona

#endif // PONA_LISTOBSERVER_HPP
