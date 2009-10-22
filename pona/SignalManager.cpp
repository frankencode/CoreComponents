/*
 * SignalManager.cpp -- signal handling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // getpid
#include <string.h> // memset
#include "Condition.hpp"
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
		int ret = pthread_sigmask(SIG_SETMASK, &mask, 0/*oset*/);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_sigmask", ret);
		SignalManager::instance()->startListener();
	}
}

SignalListener::SignalListener()
{
	done_.acquire();
}

void SignalListener::run()
{
	while (true)
	{
		int signal = -1;
		sigset_t set;
		sigfillset(&set);
		sigwait(&set, &signal);
		
		if (done_.tryAcquire()) {
			done_.release();
			break;
		}
		
		SignalManager::instance()->relay(signal);
	}
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
	  signalEvents_(new SignalEvents)
{}

SignalManager::~SignalManager()
{
	if (pid_ == Process::currentProcessId()) {
		signalListener_->done_.release();
		signalListener_->kill(SIGUSR1);
		signalListener_->wait();
	}
}

void SignalManager::defaultAction(int signal)
{
	struct sigaction action, actionSaved;
	::memset(&action, 0, sizeof(action));
	action.sa_handler = SIG_DFL;
	if (::sigaction(signal, &action, &actionSaved) == -1)
		PONA_SYSTEM_EXCEPTION;
	switch(signal) {
		case SIGURG:
		case SIGCONT:
		case SIGCHLD:
		case SIGIO:
		case SIGWINCH:
		#ifdef SIGINFO
		case SIGINFO:
		#endif
			break;
		default:
			if (signal == SIGTSTP) signal = SIGSTOP;
			sigset_t mask, maskSaved;
			sigfillset(&mask);
			pthread_sigmask(SIG_SETMASK, &mask, &maskSaved);
			pthread_kill(pthread_self(), signal);
			sigdelset(&mask, signal);
			if (signal != SIGSTOP)
				::sigsuspend(&mask);
			pthread_sigmask(SIG_SETMASK, &maskSaved, 0);
			break;
	};
	if (actionSaved.sa_handler) {
		if (::sigaction(signal, &actionSaved, 0) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
}

Ref<Event> SignalManager::signalEvent(int signal)
{
	Ref<Event> event = signalEvents_->get(signal);
	if (!event) {
		beginCritical();
		event = signalEvents_->get(signal);
		if (!event) {
			Ref<Event, Owner> newEvent = new Event;
			signalEvents_->set(signal, newEvent);
			event = newEvent;
		}
		endCritical();
	}
	return event;
}

void SignalManager::startListener()
{
	signalListener_->start();
}

void SignalManager::relay(int signal)
{
	bool relayed = false;
	beginCritical();
	Ref<Event> event = signalEvents_->get(signal);
	if (event) {
		event->run();
		relayed = true;
	}
	endCritical();
	if (!relayed)
		defaultAction(signal);
}

Ref<Event> signalEvent(int signal)
{
	return SignalManager::instance()->signalEvent(signal);
}

} // namespace pona
