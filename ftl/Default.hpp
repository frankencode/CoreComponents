/*
 * Default.hpp -- default instances provided by singletons
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
