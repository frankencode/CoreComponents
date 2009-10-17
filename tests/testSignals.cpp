#include <pona/stdio>
#include <pona/threads>
#include <pona/events>

namespace pona
{

class Echo: public Action
{
public:
	Echo(String signalName, int signal)
		: signalName_(signalName),
		  signal_(signal)
	{}
private:
	virtual void run() {
		print("[%%]\n", signalName_);
		SignalManager::defaultAction(signal_);
	}
	String signalName_;
	int signal_;
};

class Alarm: public Thread
{
public:
	Alarm()
		: count_(0)
	{}
private:
	virtual void run() {
		enableSignal(SIGALRM);
		while (true) sleep(1000);
	}
	virtual void signalHandler(int signal) {
		print("%%\n", ++count_);
	}
	int count_;
};

int main()
{
	signalEvent(SIGINT)->pushBack(new Echo("SIGINT", SIGINT));
	signalEvent(SIGTERM)->pushBack(new Echo("SIGTERM", SIGTERM));
	signalEvent(SIGQUIT)->pushBack(new Echo("SIGQUIT", SIGQUIT));
	signalEvent(SIGHUP)->pushBack(new Echo("SIGHUP", SIGHUP));
	signalEvent(SIGTSTP)->pushBack(new Echo("SIGTSTP", SIGTSTP));
	signalEvent(SIGCONT)->pushBack(new Echo("SIGCONT", SIGCONT));
	signalEvent(SIGWINCH)->pushBack(new Echo("SIGWINCH", SIGWINCH));
	
	Ref<Thread, Owner> alarm = new Alarm();
	alarm->start(Thread::Detached);
	while (true) {
		Thread::sleep(1);
		alarm->kill(SIGALRM);
	}
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
