/*
 * OnExitManager.cpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "OnExitManager.hpp"

namespace pona
{

Ref<OnExitManager> OnExitManager::instance()
{
	static Mutex mutex_;
	mutex_.acquire();
	static Ref<OnExitManager, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new OnExitManager;
	mutex_.release();
	return instance_;
}

OnExitManager::OnExitManager()
	: pid_(pid())
{}

OnExitManager::~OnExitManager()
{
	if (pid() == pid_) run();
}

} // namespace pona
