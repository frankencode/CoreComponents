/*
 * Event.hpp -- event dispatcher
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	inline static Ref<Event, Owner> newInstance() {
		return new Event;
	}

	void pushBack(Ref<Action> handler);
	void pushFront(Ref<Action> handler);
	void remove(Ref<Action> handler);

	virtual void run();

protected:
	Event();

private:
	SpinLock mutex_;
	typedef List< Ref<Action, Owner> > Handlers;
	Ref<Handlers, Owner> handlers_;
};

} // namespace ftl

#endif // FTL_EVENT_HPP
