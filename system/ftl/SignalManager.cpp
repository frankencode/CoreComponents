/*
 * SignalManager.cpp -- signal handling
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
		sigset_t set;
		sigfillset(&set);
		int ret = pthread_sigmask(SIG_SETMASK, &set, 0/*oset*/);
		if (ret != 0)
			FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
		SignalManager::instance()->startListener();
		Thread::enableSignal(SIGUSR2);
	}
}

void SignalListener::run()
{
	while (true)
	{
		int signal = -1;
		sigset_t set;
		sigfillset(&set);
		sigwait(&set, &signal);

		if (signal == SIGUSR1) break;
		SignalManager::instance()->relay(signal);
	}
}

SignalManager::SignalManager()
	: signalListener_(new SignalListener),
	  pid_(Process::currentId()),
	  signalEvents_(SignalEvents::newInstance()),
	  mutex_(Mutex::newInstance())
{}

SignalManager::~SignalManager()
{
	if (pid_ == Process::currentId()) {
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
	#ifndef NDEBUG
		FTL_SYSTEM_EXCEPTION;
	#else
		;
	#endif
	switch(signal) {
		case SIGURG:
		case SIGCONT:
		case SIGCHLD:
		case SIGIO:
		case SIGWINCH:
		#ifdef SIGINFO
		case SIGINFO:
		#endif
		case SIGUSR2: // Thread::kill() default signal
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
	Guard<Mutex> guard(mutex_);
	Ref<Event, Owner> event;
	if (!signalEvents_->lookup(signal, &event)) {
		event = Event::newInstance();
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
	Guard<Mutex> guard(mutex_);
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
