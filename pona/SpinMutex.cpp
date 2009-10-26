#include <sched.h>
#include "SpinMutex.hpp"

namespace pona
{

void SpinMutex::threadYield() { ::sched_yield(); }

} // namespace pona
