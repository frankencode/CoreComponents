/*
 * Event.cpp -- event dispatcher
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Event.hpp"

namespace ftl
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
	Handlers::Index i = handlers_->find(handler);
	if (handlers_->def(i))
		handlers_->remove(i);
	mutex_.release();
}

void Event::run()
{
	mutex_.acquire();
	for (Handlers::Index i = handlers_->first(); handlers_->def(i); ++i)
		handlers_->at(i)->run();
	mutex_.release();
}

} // namespace ftl
