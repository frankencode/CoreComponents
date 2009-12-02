#ifndef PONA_GLOBALSPINMUTEX_HPP
#define PONA_GLOBALSPINMUTEX_HPP

#include "Instance.hpp"
#include "SpinMutex.hpp"
#include "Ref.hpp"

namespace pona
{

class GlobalSpinMutex;

class GlobalSpinMutexInitializer
{
public:
	GlobalSpinMutexInitializer();
private:
	static int count_;
};

namespace { GlobalSpinMutexInitializer globalSpinMutexInitializer; }

/** Process global spin mutex initialized at application startup time.
  */
class GlobalSpinMutex: public Instance, public SpinMutex
{
public:
	static Ref<GlobalSpinMutex> instance();
	
private:
	GlobalSpinMutex() {}
};

inline Ref<GlobalSpinMutex> globalSpinMutex() { return GlobalSpinMutex::instance(); }

} // namespace pona

#endif // PONA_GLOBALSPINMUTEX_HPP
