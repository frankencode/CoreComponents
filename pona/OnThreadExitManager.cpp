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

void OnThreadExitManager::push(Ref<EventHandler> handler)
{
	handler->sibling_ = head_;
	head_ = handler;
}

OnThreadExitManager::OnThreadExitManager()
	: pid_(pid())
{}

OnThreadExitManager::~OnThreadExitManager()
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
