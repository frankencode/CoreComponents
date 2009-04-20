/*
 * OnExitManager.hpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ONEXITMANAGER_HPP
#define PONA_ONEXITMANAGER_HPP

#include "atoms"
#include "Mutex.hpp"
#include "EventManager.hpp"
#include "context.hpp"

namespace pona
{

class OnExitManager: public EventManager
{
public:
	static Ref<OnExitManager> instance();
	
private:
	OnExitManager();
	~OnExitManager();
	
	pid_t pid_;
};

inline Ref<OnExitManager> onExit() { return OnExitManager::instance(); }

} // namespace pona

#endif // PONA_ONEXITMANAGER_HPP
