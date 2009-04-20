/*
 * EventManager.cpp -- event dispatcher
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "EventManager.hpp"

namespace pona
{

EventManager::EventManager()
	: handlers_(new Handlers)
{}

void EventManager::pushBack(Ref<EventHandler> handler)
{
	acquire();
	handlers_->pushBack(handler);
	release();
}

void EventManager::pushFront(Ref<EventHandler> handler)
{
	acquire();
	handlers_->pushFront(handler);
	release();
}

void EventManager::remove(Ref<EventHandler> handler)
{
	acquire();
	int i = handlers_->find(handler);
	if (i != handlers_->length())
		handlers_->remove(i);
	release();
}

void EventManager::run()
{
	acquire();
	for (int i = 0, n = handlers_->length(); i < n; ++i)
		handlers_->get(i)->run();
	release();
}

} // namespace pona
