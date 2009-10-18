/*
 * Event.hpp -- event dispatcher
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_EVENT_HPP
#define PONA_EVENT_HPP

#include "atoms"
#include "Mutex.hpp"
#include "List.hpp"
#include "Action.hpp"

namespace pona
{

class PONA_API Event: public Action
{
	PONA_SHARED
	
public:
	Event();
	
	void pushBack(Ref<Action> handler);
	void pushFront(Ref<Action> handler);
	void remove(Ref<Action> handler);
	
	virtual void run();
	
private:
	typedef List< Ref<Action, Owner> > Handlers;
	Ref<Handlers, Owner> handlers_;
};

} // namespace pona

#endif // PONA_EVENT_HPP
