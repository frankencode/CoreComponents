#include "time.hpp"
#include "Random.hpp"

namespace pona
{

Random::Random(int seed)
	: x_((seed >= 0) ? seed : now().nanoSecondsPart())
{
	if (x_ == 0)
		x_ = 1;
}

} // namespace pona
