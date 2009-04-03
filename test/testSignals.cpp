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
	onSignal()->push(SIGINT, new FinishHandler(&done));
	onSignal()->push(SIGTERM, new FinishHandler(&done));
	
	onSignal()->push(SIGINT, new TestHandler("[SIGINT]"));
	onSignal()->push(SIGTERM, new TestHandler("[SIGTERM]"));
	onSignal()->push(SIGQUIT, new TestHandler("[SIGQUIT]"));
	onSignal()->push(SIGHUP, new TestHandler("[SIGHUP]"));
	onSignal()->push(SIGTSTP, new TestHandler("[SIGTSTP]"));
	onSignal()->push(SIGCONT, new TestHandler("[SIGCONT]"));
	onSignal()->push(SIGWINCH, new TestHandler("[SIGWINCH]"));
	
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
