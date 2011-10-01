/*
 * Event.cpp -- event dispatcher
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Guard.hpp"
#include "Event.hpp"

namespace ftl
{

Event::Event()
	: handlers_(new Handlers)
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
