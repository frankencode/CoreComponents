#include "GlobalSpinMutex.hpp"

namespace pona
{

int GlobalSpinMutexInitializer::count_ = 0;

GlobalSpinMutexInitializer::GlobalSpinMutexInitializer()
{
	if (count_ == 0) {
		++count_;
		GlobalSpinMutex::instance();
	}
}

Ref<GlobalSpinMutex> GlobalSpinMutex::instance()
{
	static Ref<GlobalSpinMutex, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new GlobalSpinMutex;
	return instance_;
}

} // namespace pona
