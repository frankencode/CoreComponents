/*
 * Mutex.hpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_MUTEX_HPP
#define PONA_MUTEX_HPP

#include "atoms"

namespace pona
{

class Mutex: public Instance, public CoreMutex
{};

} // namespace pona

#endif // PONA_MUTEX_HPP
