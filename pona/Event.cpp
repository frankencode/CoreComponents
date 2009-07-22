/*
 * Event.cpp -- event dispatcher
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Event.hpp"

namespace pona
{

Event::Event()
	: handlers_(new Handlers)
{}

void Event::pushBack(Ref<Action> handler)
{
	beginCritical();
	handlers_->pushBack(handler);
	endCritical();
}

void Event::pushFront(Ref<Action> handler)
{
	beginCritical();
	handlers_->pushFront(handler);
	endCritical();
}

void Event::remove(Ref<Action> handler)
{
	beginCritical();
	int i = handlers_->find(handler);
	if (i != handlers_->length())
		handlers_->remove(i);
	endCritical();
}

void Event::run()
{
	beginCritical();
	for (int i = 0, n = handlers_->length(); i < n; ++i)
		handlers_->get(i)->run();
	endCritical();
}

} // namespace pona
