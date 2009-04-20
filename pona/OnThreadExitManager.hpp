/*
 * OnThreadExitManager.hpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ONTHREADEXITMANAGER_HPP
#define PONA_ONTHREADEXITMANAGER_HPP

#include "atoms"
#include "ThreadLocalOwner.hpp"
#include "EventManager.hpp"
#include "context.hpp"

namespace pona
{

class OnThreadExitManager: public EventManager
{
public:
	static Ref<OnThreadExitManager> instance();
	
private:
	OnThreadExitManager();
	~OnThreadExitManager();
	
	pid_t pid_;
};

inline Ref<OnThreadExitManager> onThreadExit() { return OnThreadExitManager::instance(); }

} // namespace pona

#endif // PONA_ONTHREADEXITMANAGER_HPP
