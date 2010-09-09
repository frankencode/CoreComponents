#ifndef FTL_SCOPEGUARD_HPP
#define FTL_SCOPEGUARD_HPP

namespace ftl
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

} // namespace ftl

#endif // FTL_SCOPEGUARD_HPP
