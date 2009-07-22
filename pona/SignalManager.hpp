/*
 * SignalManager.hpp -- signal handling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SIGNALMANAGER_HPP
#define PONA_SIGNALMANAGER_HPP

#include <signal.h>
#include "atom"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Process.hpp"
#include "Map.hpp"
#include "Event.hpp"

namespace pona
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
	SignalListener();
	int run();
	bool stopListener_;
};

class SignalManager: public Instance
{
	PONA_SHARED
	
public:
	static Ref<SignalManager> instance();
	
	Ref<Event> managerBySignal(int signal);
	
private:
	friend class SignalInitializer;
	friend class SignalListener;
	
	SignalManager();
	~SignalManager();
	
	void startListener();
	bool relay(int signal);
	
	Ref<SignalListener, Owner> signalListener_;
	int pid_;
	
	typedef Map<int, Ref<Event, Owner> > ManagerBySignal;
	Ref<ManagerBySignal, Owner> managerBySignal_;
};

Ref<Event> signalEvent(int signal);

} // namespace pona

#endif // PONA_SIGNALMANAGER_HPP
