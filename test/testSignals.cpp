#include <pona/stdio>
#include <pona/thread>
#include <pona/event>

namespace pona
{

class Echo: public Action
{
public:
	Echo(String message)
		: message_(message)
	{}
private:
	virtual void run() { output()->writeLine(message_); }
	String message_;
};

class Finish: public Action
{
public:
	Finish(bool* done)
		: done_(done)
	{}
	virtual void run() { *done_ = true; }
private:
	bool* done_;
};

int main()
{
	bool done = false;
	signalEvent(SIGINT)->pushBack(new Finish(&done));
	signalEvent(SIGTERM)->pushBack(new Finish(&done));
	
	signalEvent(SIGINT)->pushBack(new Echo("[SIGINT]"));
	signalEvent(SIGTERM)->pushBack(new Echo("[SIGTERM]"));
	signalEvent(SIGQUIT)->pushBack(new Echo("[SIGQUIT]"));
	signalEvent(SIGHUP)->pushBack(new Echo("[SIGHUP]"));
	signalEvent(SIGTSTP)->pushBack(new Echo("[SIGTSTP]"));
	signalEvent(SIGCONT)->pushBack(new Echo("[SIGCONT]"));
	signalEvent(SIGWINCH)->pushBack(new Echo("[SIGWINCH]"));
	
	int n = 0;
	while (!done) {
		Thread::sleep(1);
		print("%%\n", ++n);
	}
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
