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

Mutex OnExitManager::mutex_;

Ref<OnExitManager> OnExitManager::instance()
{
	static Ref<OnExitManager, Owner> instance_ = 0;
	if (!instance_) {
		mutex_.acquire();
		if (!instance_)
			instance_ = new OnExitManager;
		mutex_.release();
	}
	return instance_;
}

void OnExitManager::push(Ref<EventHandler> handler)
{
	mutex_.acquire();
	handler->sibling_ = head_;
	head_ = handler;
	mutex_.release();
}

OnExitManager::OnExitManager()
	: pid_(pid())
{}

OnExitManager::~OnExitManager()
{
	if (pid() == pid_)
	{
		while (head_) {
			head_->run();
			head_ = head_->sibling_;
		}
	}
	else
		head_ = 0;
}

} // namespace pona
