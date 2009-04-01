/*
 * EventHandler.hpp -- event handlers
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_EVENTHANDLER_HPP
#define PONA_EVENTHANDLER_HPP

#include "atoms"

namespace pona
{

class EventHandler: public Instance
{
public:
	virtual void run() = 0;
	Ref<EventHandler, Owner> sibling_;
};

} // namespace pona

#endif // PONA_EVENTHANDLER_HPP
