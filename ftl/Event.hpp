/*
 * Event.hpp -- event dispatcher
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_EVENT_HPP
#define FTL_EVENT_HPP

#include "atoms"
#include "List.hpp"

namespace ftl
{

class Event: public Action
{
public:
	Event();
	
	void pushBack(Ref<Action> handler);
	void pushFront(Ref<Action> handler);
	void remove(Ref<Action> handler);
	
	virtual void run();
	
private:
	CoreMutex mutex_;
	typedef List< Ref<Action, Owner> > Handlers;
	Ref<Handlers, Owner> handlers_;
};

} // namespace ftl

#endif // FTL_EVENT_HPP
