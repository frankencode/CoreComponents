/*
 * SignalManager.cpp -- signal handling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // getpid
#include "SignalManager.hpp"

namespace pona
{

int SignalInitializer::count_ = 0;

SignalInitializer::SignalInitializer()
{
	if (count_ == 0) {
		++count_;
		sigset_t mask;
		sigfillset(&mask);
		if (pthread_sigmask(SIG_SETMASK, &mask, 0) != 0)
			PONA_THROW(SystemException, "pthread_sigmask() failed");
		SignalManager::instance()->startListener();
	}
}

SignalListener::SignalListener()
	: stopListener_(false)
{}

int SignalListener::run()
{
	while (true)
	{
		int signal = -1;
		sigset_t set;
		sigfillset(&set);
		if (sigwait(&set, &signal) != 0)
			PONA_THROW(SystemException, "sigwait() failed");
		
		if (stopListener_) break;
		
		bool relayed = SignalManager::instance()->relay(signal);
		
		if ((!relayed) || (signal == SIGTSTP)) {
			sigset_t set, old;
			sigemptyset(&set);
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

Ref<SignalManager> SignalManager::instance()
{
	static Mutex mutex_;
	mutex_.acquire();
	static Ref<SignalManager, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new SignalManager;
	mutex_.release();
	return instance_;
}

SignalManager::SignalManager()
	: signalListener_(new SignalListener),
	  pid_(Process::currentProcessId()),
	  managerBySignal_(new ManagerBySignal)
{}

SignalManager::~SignalManager()
{
	if (pid_ == Process::currentProcessId()) {
		signalListener_->stopListener_ = true;
		pthread_kill(signalListener_->tid(), SIGUSR1);
		signalListener_->wait();
	}
}

Ref<Event> SignalManager::managerBySignal(int signal)
{
	Ref<Event> manager = managerBySignal_->get(signal);
	if (!manager) {
		beginCritical();
		manager = managerBySignal_->get(signal);
		if (!manager) {
			Ref<Event, Owner> newManager = new Event;
			managerBySignal_->set(signal, newManager);
			manager = newManager;
		}
		endCritical();
	}
	return manager;
}

void SignalManager::startListener()
{
	signalListener_->start();
}

bool SignalManager::relay(int signal)
{
	bool relayed = false;
	beginCritical();
	Ref<Event> manager = managerBySignal_->get(signal);
	if (manager) {
		manager->run();
		relayed = true;
	}
	endCritical();
	return relayed;
}

Ref<Event> signalEvent(int signal)
{
	return SignalManager::instance()->managerBySignal(signal);
}

} // namespace pona
