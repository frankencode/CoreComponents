/*
 * OnThreadExitManager.cpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "OnThreadExitManager.hpp"

namespace pona
{

Ref<OnThreadExitManager> OnThreadExitManager::instance()
{
	static Ref<OnThreadExitManager, ThreadLocalOwner> instance_ = 0;
	
	if (!instance_)
		instance_ = new OnThreadExitManager;
	return instance_;
}

OnThreadExitManager::OnThreadExitManager()
	: pid_(pid())
{}

OnThreadExitManager::~OnThreadExitManager()
{
	if (pid() == pid_) run();
}

} // namespace pona
