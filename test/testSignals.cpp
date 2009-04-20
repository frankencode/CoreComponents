#include <pona/stdio>
#include <pona/thread>
#include <pona/event>

namespace pona
{

class TestHandler: public EventHandler
{
public:
	TestHandler(String message)
		: message_(message)
	{}
private:
	virtual void run() { output()->writeLine(message_); }
	String message_;
};

class FinishHandler: public EventHandler
{
public:
	FinishHandler(bool* done)
		: done_(done)
	{}
	virtual void run() { *done_ = true; }
private:
	bool* done_;
};

int main()
{
	bool done = false;
	onSignal(SIGINT)->pushBack(new FinishHandler(&done));
	onSignal(SIGTERM)->pushBack(new FinishHandler(&done));
	
	onSignal(SIGINT)->pushBack(new TestHandler("[SIGINT]"));
	onSignal(SIGTERM)->pushBack(new TestHandler("[SIGTERM]"));
	onSignal(SIGQUIT)->pushBack(new TestHandler("[SIGQUIT]"));
	onSignal(SIGHUP)->pushBack(new TestHandler("[SIGHUP]"));
	onSignal(SIGTSTP)->pushBack(new TestHandler("[SIGTSTP]"));
	onSignal(SIGCONT)->pushBack(new TestHandler("[SIGCONT]"));
	onSignal(SIGWINCH)->pushBack(new TestHandler("[SIGWINCH]"));
	
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
