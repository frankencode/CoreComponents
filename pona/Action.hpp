/*
 * Action.hpp -- event handlers
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_ACTION_HPP
#define PONA_ACTION_HPP

#include "atom"

namespace pona
{

class Action: public Instance
{
public:
	virtual void run() = 0;
};

} // namespace pona

#endif // PONA_ACTION_HPP
