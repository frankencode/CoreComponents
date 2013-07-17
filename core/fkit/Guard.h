/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_GUARD_H
#define FKIT_GUARD_H

namespace fkit
{

template<class Mutex>
class Guard
{
public:
	Guard(Mutex *mutex): mutex_(mutex) { mutex_->acquire(); }
	~Guard() { mutex_->release(); }
private:
	Mutex *mutex_;
};

} // namespace fkit

#endif // FKIT_GUARD_H
