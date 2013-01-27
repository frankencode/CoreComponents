 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_THREADLOCALSINGLETON_HPP
#define FTL_THREADLOCALSINGLETON_HPP

#include "Instance.hpp"
#include "O.hpp"
#include "TLO.hpp"
#include "LocalStatic.hpp"

namespace ftl
{

template<class SubClass>
class ThreadLocalSingleton
{
public:
	static SubClass *instance()
	{
		TLO<SubClass> &instance_ = localStatic< TLO<SubClass>, ThreadLocalSingleton<SubClass> >();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace ftl

#endif // FTL_THREADLOCALSINGLETON_HPP
