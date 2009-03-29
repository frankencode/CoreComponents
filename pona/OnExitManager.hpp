#ifndef PONA_ONEXITMANAGER_HPP
#define PONA_ONEXITMANAGER_HPP

#include "atoms"
#include "EventHandler.hpp"
#include "Mutex.hpp"
#include "context.hpp"

namespace pona
{

class OnExitManager: public Instance
{
public:
	static Ref<OnExitManager> instance();
	
	void push(Ref<EventHandler> handler);
	
private:
	OnExitManager();
	~OnExitManager();
	
	pid_t pid_;
	Ref<EventHandler> head_;
	Mutex mutex_;
};

inline Ref<OnExitManager> onExit() { return OnExitManager::instance(); }

} // namespace pona

#endif // PONA_ONEXITMANAGER_HPP
