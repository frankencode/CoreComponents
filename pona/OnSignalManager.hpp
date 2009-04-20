/*
 * OnSignalManager.hpp -- signal handling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ONSIGNALMANAGER_HPP
#define PONA_ONSIGNALMANAGER_HPP

#include <signal.h>
#include "atoms"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Map.hpp"
#include "EventManager.hpp"

namespace pona
{

class OnSignalManager: public Instance
{
	PONA_SHARED
	
public:
	static Ref<OnSignalManager> instance();
	
	Ref<EventManager> managerBySignal(int signal);
	
private:
	OnSignalManager();
	~OnSignalManager();
	
	void startListener();
	bool relay(int signal);
	
	class SignalMaskInitializer {
	public:
		SignalMaskInitializer();
	};
	
	class SignalListener: public Thread {
	public:
		SignalListener();
		int run();
		bool stopListener_;
	};
	
	friend class SignalMaskInitializer;
	friend class SignalListener;
	
	static SignalMaskInitializer signalMaskInitializer_;
	
	Ref<SignalListener, Owner> signalListener_;
	
	typedef Map<int, Ref<EventManager, Owner> > ManagerBySignal;
	Ref<ManagerBySignal, Owner> managerBySignal_;
};

Ref<EventManager> onSignal(int signal);

} // namespace pona

#endif // PONA_ONSIGNALMANAGER_HPP
