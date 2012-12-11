/*
 * Event.cpp -- event dispatcher
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
