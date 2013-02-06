 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_EVENT_HPP
#define FTL_EVENT_HPP

#include "List.hpp"

namespace ftl
{

class Event: public Action
{
public:
	inline static Ref<Event> create() {
		return new Event;
	}

	void pushBack(Action *handler);
	void pushFront(Action *handler);
	void remove(Action *handler);

	virtual void run();

protected:
	Event();

private:
	SpinLock mutex_;
	typedef List< Ref<Action> > Handlers;
	Ref<Handlers> handlers_;
};

} // namespace ftl

#endif // FTL_EVENT_HPP
