/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_THREADLOCALSINGLETON_H
#define FKIT_THREADLOCALSINGLETON_H

#include "Object.h"
#include "Ref.h"
#include "ThreadLocalRef.h"
#include "LocalStatic.h"

namespace fkit
{

template<class SubClass>
class ThreadLocalSingleton
{
public:
	static SubClass *instance()
	{
		ThreadLocalRef<SubClass> &instance_ = localStatic< ThreadLocalRef<SubClass>, ThreadLocalSingleton<SubClass> >();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace fkit

#endif // FKIT_THREADLOCALSINGLETON_H
