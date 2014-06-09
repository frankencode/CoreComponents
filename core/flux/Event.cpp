/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Guard.h"
#include "Event.h"

namespace flux
{

Event::Event()
	: handlers_(Handlers::create())
{}

void Event::pushBack(Action *handler)
{
	Guard<SpinLock> guard(&mutex_);
	handlers_->pushBack(handler);
}

void Event::pushFront(Action *handler)
{
	Guard<SpinLock> guard(&mutex_);
	handlers_->pushFront(handler);
}

void Event::remove(Action *handler)
{
	Guard<SpinLock> guard(&mutex_);
	int i = handlers_->find(handler);
	if (i < handlers_->count())
		handlers_->remove(i);
}

void Event::run()
{
	Guard<SpinLock> guard(&mutex_);
	for (int i = 0; i < handlers_->count(); ++i)
		handlers_->at(i)->run();
}

} // namespace flux
