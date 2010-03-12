#ifndef PONA_LOCALSTATIC_HPP
#define PONA_LOCALSTATIC_HPP

#include "GlobalCoreMutex.hpp"
#include "Mutex.hpp"
#include "ScopeGuard.hpp"
#include "Guard.hpp"

namespace pona
{

/** Initialize a local static variable in a thread-safe manner
  * (without requiring static local initialization to be supported by the compiler).
  * Limitation: Does no good job in initializing a sequence of local statics.
  * Related thought: Maybe the compiler should warn about local statics instead of
  * throwing in fancy locking logic.
  */
template<class T, class Scope = None>
class LocalStatic
{
public:
	static T& instance()
	{
		Guard<CoreMutex> guard1(globalCoreMutex());
		static Mutex mutex_;
		guard1.dismiss();
		ScopeGuard<Mutex> guard2(&mutex_);
		static T instance_;
		return instance_;
	}
};

template<class T, class Scope>
inline T& localStatic() { return LocalStatic<T, Scope>::instance(); }

template<class T>
inline T& localStatic() { return LocalStatic<T, None>::instance(); }

} // namespace pona

#endif // PONA_LOCALSTATIC_HPP
