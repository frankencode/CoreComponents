#ifndef PONA_SPINGUARD_HPP
#define PONA_SPINGUARD_HPP

#include "SpinMutex.hpp"

namespace pona
{

class SpinGuard
{
public:
	SpinGuard(SpinMutex* mutex): mutex_(mutex) { mutex_->acquire(); }
	~SpinGuard() { mutex_->release(); }
private:
	SpinMutex* mutex_;
};

} // namespace pona


#endif // PONA_SPINGUARD_HPP
