 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_DEFAULT_HPP
#define FTL_DEFAULT_HPP

#include "Singleton.hpp"

namespace ftl
{

template<class T>
class Default: public T, public Singleton<T>
{
private:
	friend class Singleton<T>;
	Default() {}
};

} // namespace ftl

#endif // FTL_DEFAULT_HPP
