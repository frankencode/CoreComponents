#ifndef PONA_GUARD_HPP
#define PONA_GUARD_HPP

#include "Mutex.hpp"

namespace pona
{

class Guard: public Instance
{
public:
	Guard(Ref<Mutex> mutex): mutex_(mutex) { mutex_->acquire(); }
	~Guard() { mutex_->release(); }
private:
	Ref<Mutex> mutex_;
};

} // namespace pona

#endif // PONA_GUARD_HPP
