#include <unistd.h> // getpid
#include "OnSignalManager.hpp"

namespace pona
{

OnSignalManager::SignalMaskInitializer OnSignalManager::signalMaskInitializer_;
Mutex OnSignalManager::mutex_;

Ref<OnSignalManager> OnSignalManager::instance()
{
	static Ref<OnSignalManager, Owner> instance_ = 0;
	if (!instance_) {
		mutex_.acquire();
		if (!instance_)
			instance_ = new OnSignalManager;
		mutex_.release();
	}
	return instance_;
}

OnSignalManager::OnSignalManager()
	: signalListener_(new SignalListener),
	  handlerBySignal_(new HandlerBySignal)
{}

OnSignalManager::~OnSignalManager()
{
	signalListener_->stopListener_ = true;
	pthread_kill(signalListener_->tid(), SIGUSR1);
}

void OnSignalManager::push(int signal, Ref<EventHandler> handler)
{
	mutex_.acquire();
	handler->sibling_ = handlerBySignal_->get(signal);
	handlerBySignal_->set(signal, handler);
	mutex_.release();
}

void OnSignalManager::startListener()
{
	signalListener_->start();
}

bool OnSignalManager::relay(int signal)
{
	bool relayed = false;
	mutex_.acquire();
	{
		Ref<EventHandler> handler = handlerBySignal_->get(signal);
		while (handler) {
			handler->run();
			handler = handler->sibling_;
			relayed = true;
		}
	}
	mutex_.release();
	return relayed;
}

OnSignalManager::SignalMaskInitializer::SignalMaskInitializer()
{
	sigset_t mask;
	sigfillset(&mask);
	if (pthread_sigmask(SIG_SETMASK, &mask, 0) != 0)
		PONA_THROW(SystemException, "pthread_sigmask() failed");
	onSignal()->startListener();
}

OnSignalManager::SignalListener::SignalListener()
	: stopListener_(false)
{}

int OnSignalManager::SignalListener::run()
{
	while (true) {
		int signal = -1;
		sigset_t set;
		sigfillset(&set);
		if (sigwait(&set, &signal) != 0)
			PONA_THROW(SystemException, "sigwait() failed");
		
		if (stopListener_) break;
		
		bool relayed = onSignal()->relay(signal);
		
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

Ref<OnSignalManager> onSignal() { return OnSignalManager::instance(); }

} // namespace pona
