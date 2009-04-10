#ifndef PONA_ONSIGNALMANAGER_HPP
#define PONA_ONSIGNALMANAGER_HPP

#include <signal.h>
#include "atoms"
#include "EventHandler.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Map.hpp"
#include "EventHandler.hpp"

namespace pona
{

class OnSignalManager: public Mutex
{
public:
	static Ref<OnSignalManager> instance();
	
	void push(int signal, Ref<EventHandler> handler);
	
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
	static Mutex mutex_;
	
	Ref<SignalListener, Owner> signalListener_;
	
	typedef Map<int, Ref<EventHandler, Owner> > HandlerBySignal;
	Ref<HandlerBySignal, Owner> handlerBySignal_;
};

Ref<OnSignalManager> onSignal();

} // namespace pona

#endif // PONA_ONSIGNALMANAGER_HPP
