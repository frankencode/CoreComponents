#ifndef PONA_SCOPEGUARD_HPP
#define PONA_SCOPEGUARD_HPP

namespace pona
{

template<class Mutex>
class ScopeGuard
{
public:
	ScopeGuard(Mutex* mutex): mutex_(mutex) { mutex_->acquire(); }
	~ScopeGuard() { mutex_->release(); }
private:
	Mutex* mutex_;
};

} // namespace pona

#endif // PONA_SCOPEGUARD_HPP
