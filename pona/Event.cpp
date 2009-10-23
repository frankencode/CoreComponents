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
	mutex_.acquire();
	handlers_->pushBack(handler);
	mutex_.release();
}

void Event::pushFront(Ref<Action> handler)
{
	mutex_.acquire();
	handlers_->pushFront(handler);
	mutex_.release();
}

void Event::remove(Ref<Action> handler)
{
	mutex_.acquire();
	int i = handlers_->find(handler);
	if (i != handlers_->length())
		handlers_->remove(i);
	mutex_.release();
}

void Event::run()
{
	mutex_.acquire();
	for (int i = 0, n = handlers_->length(); i < n; ++i)
		handlers_->get(i)->run();
	mutex_.release();
}

} // namespace pona
