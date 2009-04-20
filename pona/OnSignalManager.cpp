/*
 * OnSignalManager.cpp -- signal handling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // getpid
#include "OnSignalManager.hpp"

namespace pona
{

OnSignalManager::SignalMaskInitializer OnSignalManager::signalMaskInitializer_;

Ref<OnSignalManager> OnSignalManager::instance()
{
	static Mutex mutex_;
	mutex_.acquire();
	static Ref<OnSignalManager, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new OnSignalManager;
	mutex_.release();
	return instance_;
}

OnSignalManager::OnSignalManager()
	: signalListener_(new SignalListener),
	  managerBySignal_(new ManagerBySignal)
{}

OnSignalManager::~OnSignalManager()
{
	signalListener_->stopListener_ = true;
	pthread_kill(signalListener_->tid(), SIGUSR1);
}

Ref<EventManager> OnSignalManager::managerBySignal(int signal)
{
	Ref<EventManager> manager = managerBySignal_->get(signal);
	if (!manager) {
		acquire();
		manager = managerBySignal_->get(signal);
		if (!manager) {
			Ref<EventManager, Owner> newManager = new EventManager;
			managerBySignal_->set(signal, newManager);
			manager = newManager;
		}
		release();
	}
	return manager;
}

void OnSignalManager::startListener()
{
	signalListener_->start();
}

bool OnSignalManager::relay(int signal)
{
	bool relayed = false;
	acquire();
	Ref<EventManager> manager = managerBySignal_->get(signal);
	if (manager) {
		manager->run();
		relayed = true;
	}
	release();
	return relayed;
}

OnSignalManager::SignalMaskInitializer::SignalMaskInitializer()
{
	sigset_t mask;
	sigfillset(&mask);
	if (pthread_sigmask(SIG_SETMASK, &mask, 0) != 0)
		PONA_THROW(SystemException, "pthread_sigmask() failed");
	OnSignalManager::instance()->startListener();
}

OnSignalManager::SignalListener::SignalListener()
	: stopListener_(false)
{}

int OnSignalManager::SignalListener::run()
{
	while (true)
	{
		int signal = -1;
		sigset_t set;
		sigfillset(&set);
		if (sigwait(&set, &signal) != 0)
			PONA_THROW(SystemException, "sigwait() failed");
		
		if (stopListener_) break;
		
		bool relayed = OnSignalManager::instance()->relay(signal);
		
		if ((!relayed) || (signal == SIGTSTP)) {
			sigset_t set, old;
			sigaddset(&set, signal);
			if (pthread_sigmask(SIG_UNBLOCK, &set, &old) != 0)
				PONA_THROW(SystemException, "pthread_sigmask() failed");
			kill(getpid(), signal);
			if (pthread_sigmask(SIG_SETMASK, &old, 0) != 0)
				PONA_THROW(SystemException, "pthread_sigmask() failed");
		}
	}
	
	return 0;
}

Ref<EventManager> onSignal(int signal)
{
	return OnSignalManager::instance()->managerBySignal(signal);
}

} // namespace pona
