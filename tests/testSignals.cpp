#include <ftl/streams>
#include <ftl/threads>
#include <ftl/events>

namespace ftl
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

int main()
{
	signalEvent(SIGINT)->pushBack(new Echo("SIGINT", SIGINT));
	signalEvent(SIGTERM)->pushBack(new Echo("SIGTERM", SIGTERM));
	signalEvent(SIGQUIT)->pushBack(new Echo("SIGQUIT", SIGQUIT));
	signalEvent(SIGHUP)->pushBack(new Echo("SIGHUP", SIGHUP));
	signalEvent(SIGTSTP)->pushBack(new Echo("SIGTSTP", SIGTSTP));
	signalEvent(SIGCONT)->pushBack(new Echo("SIGCONT", SIGCONT));
	signalEvent(SIGWINCH)->pushBack(new Echo("SIGWINCH", SIGWINCH));
	
	int n = 0;
	while (true) {
		print("%%\n", ++n);
		Thread::sleep(1);
	}
	
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
