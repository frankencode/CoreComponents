/*
 * Default.hpp -- default instances provided by singletons
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
