/*
 * Action.hpp -- some action to be taken
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ACTION_HPP
#define PONA_ACTION_HPP

#include "atoms"

namespace pona
{

class Action: public virtual Instance
{
public:
	virtual void run() = 0;
};

} // namespace pona

#endif // PONA_ACTION_HPP
