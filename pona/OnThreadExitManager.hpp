#ifndef PONA_ONTHREADEXITMANAGER_HPP
#define PONA_ONTHREADEXITMANAGER_HPP

#include "atoms"
#include "ThreadLocalOwner.hpp"
#include "EventHandler.hpp"
#include "context.hpp"

namespace pona
{

class OnThreadExitManager: public Instance
{
public:
	static Ref<OnThreadExitManager> instance();
	
	void push(Ref<EventHandler> handler);
	
private:
	OnThreadExitManager();
	~OnThreadExitManager();
	
	pid_t pid_;
	Ref<EventHandler, Owner> head_;
};

inline Ref<OnThreadExitManager> onThreadExit() { return OnThreadExitManager::instance(); }

} // namespace pona

#endif // PONA_ONTHREADEXITMANAGER_HPP
