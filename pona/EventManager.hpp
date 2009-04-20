/*
 * EventManager.hpp -- event dispatcher
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_EVENTMANAGER_HPP
#define PONA_EVENTMANAGER_HPP

#include "atoms"
#include "Mutex.hpp"
#include "List.hpp"
#include "EventHandler.hpp"

namespace pona
{

class EventManager: public EventHandler
{
	PONA_SHARED
	
public:
	EventManager();
	
	void pushBack(Ref<EventHandler> handler);
	void pushFront(Ref<EventHandler> handler);
	void remove(Ref<EventHandler> handler);
	
	virtual void run();
	
private:
	typedef List< Ref<EventHandler, Owner> > Handlers;
	Ref<Handlers, Owner> handlers_;
};

} // namespace pona

#endif // PONA_EVENTMANAGER_HPP
