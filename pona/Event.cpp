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
	acquire();
	handlers_->pushBack(handler);
	release();
}

void Event::pushFront(Ref<Action> handler)
{
	acquire();
	handlers_->pushFront(handler);
	release();
}

void Event::remove(Ref<Action> handler)
{
	acquire();
	int i = handlers_->find(handler);
	if (i != handlers_->length())
		handlers_->remove(i);
	release();
}

void Event::run()
{
	acquire();
	for (int i = 0, n = handlers_->length(); i < n; ++i)
		handlers_->get(i)->run();
	release();
}

} // namespace pona
