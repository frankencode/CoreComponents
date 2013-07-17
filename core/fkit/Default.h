/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_DEFAULT_H
#define FKIT_DEFAULT_H

#include "Singleton.h"

namespace fkit
{

template<class T>
class Default: public T, public Singleton<T>
{
private:
	friend class Singleton<T>;
	Default() {}
};

} // namespace fkit

#endif // FKIT_DEFAULT_H
