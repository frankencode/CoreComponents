/*
 * SignalManager.hpp -- signal handling
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SIGNALMANAGER_HPP
#define FTL_SIGNALMANAGER_HPP

#include <signal.h>
#include "atoms"
#include "Singleton.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "Process.hpp"
#include "Map.hpp"
#include "Event.hpp"

namespace ftl
{

class SignalInitializer {
public:
	SignalInitializer();
private:
	static int count_;
};

namespace { SignalInitializer signalInitializer_; }

class SignalListener: public Thread {
public:
	void run();
};

class SignalManager: public Instance, public Singleton<SignalManager>
{
public:
	Ref<Event> signalEvent(int signal);
	static void defaultAction(int signal);
	
private:
	friend class Singleton<SignalManager>;
	friend class SignalInitializer;
	friend class SignalListener;
	friend class Thread;
	
	SignalManager();
	~SignalManager();
	
	void startListener();
	void relay(int signal);
	
	Ref<SignalListener, Owner> signalListener_;
	int pid_;
	
	typedef Map<int, Ref<Event, Owner> > SignalEvents;
	Ref<SignalEvents, Owner> signalEvents_;
	
	Mutex mutex_;
};

Ref<Event> signalEvent(int signal);

} // namespace ftl

#endif // FTL_SIGNALMANAGER_HPP
