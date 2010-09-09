/*
 * SignalManager.cpp -- signal handling
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // getpid
#include "Condition.hpp"
#include "SignalManager.hpp"

namespace ftl
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
			FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
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
	mem::clr(&action, sizeof(action));
	mem::clr(&actionSaved, sizeof(actionSaved));
	action.sa_handler = SIG_DFL;
	if (::sigaction(signal, &action, &actionSaved) == -1)
		FTL_SYSTEM_EXCEPTION;
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
			FTL_SYSTEM_EXCEPTION;
	}
}

Ref<Event> SignalManager::signalEvent(int signal)
{
	ScopeGuard<Mutex> guard(&mutex_);
	Ref<Event, Owner> event;
	if (!signalEvents_->lookup(signal, &event)) {
		event = new Event;
		signalEvents_->insert(signal, event);
	}
	return event;
}

void SignalManager::startListener()
{
	signalListener_->start();
}

void SignalManager::relay(int signal)
{
	ScopeGuard<Mutex> guard(&mutex_);
	Ref<Event, Owner> event;
	if (signalEvents_->lookup(signal, &event))
		event->run();
	else
		defaultAction(signal);
}

Ref<Event> signalEvent(int signal)
{
	return SignalManager::instance()->signalEvent(signal);
}

} // namespace ftl
