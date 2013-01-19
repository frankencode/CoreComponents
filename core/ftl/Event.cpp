 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "Guard.hpp"
#include "Event.hpp"

namespace ftl
{

Event::Event()
	: handlers_(Handlers::create())
{}

void Event::pushBack(Ref<Action> handler)
{
	Guard<SpinLock> guard(&mutex_);
	handlers_->pushBack(handler);
}

void Event::pushFront(Ref<Action> handler)
{
	Guard<SpinLock> guard(&mutex_);
	handlers_->pushFront(handler);
}

void Event::remove(Ref<Action> handler)
{
	Guard<SpinLock> guard(&mutex_);
	int i = handlers_->find(handler);
	if (i < handlers_->length())
		handlers_->remove(i);
}

void Event::run()
{
	Guard<SpinLock> guard(&mutex_);
	for (int i = 0; i < handlers_->length(); ++i)
		handlers_->at(i)->run();
}

} // namespace ftl
