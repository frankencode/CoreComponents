/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_LOCALSTATIC_H
#define FLUX_LOCALSTATIC_H

#include <flux/GlobalCoreMutex>
#include <flux/Guard>

namespace flux {

/** Initialize a local static variable in a thread-safe manner
  */
template<class T, class Scope = None>
class LocalStatic
{
public:
	static T &instance()
	{
		Guard<SpinLock> guard(globalCoreMutex());
		static T instance_;
		return instance_;
	}
};

template<class T, class Scope>
inline T &localStatic() { return LocalStatic<T, Scope>::instance(); }

template<class T>
inline T &localStatic() { return LocalStatic<T, None>::instance(); }

} // namespace flux

#endif // FLUX_LOCALSTATIC_H
