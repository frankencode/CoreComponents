#include <unistd.h> // alarm(2)
#include <ftl/PrintDebug.hpp>
#include <ftl/Thread.hpp>

using namespace ftl;

class SignalEater: public Thread {
public:
	static Ref<SignalEater, Owner> create() {
		return new SignalEater;
	}

private:
	SignalEater() {}

	virtual void run()
	{
		hookSignal(SIGINT);
		hookSignal(SIGALRM);
		hookSignal(SIGWINCH);
		hookSignal(SIGTSTP);
		hookSignal(SIGCONT);
		unblockSignal(SIGINT);
		unblockSignal(SIGALRM);
		unblockSignal(SIGWINCH);
		unblockSignal(SIGTSTP);
		unblockSignal(SIGCONT);

		while (true) {
			try {
				String line = input()->readLine();
				if (line == "") break;
			}
			catch (Interrupt& ex) {
				if (ex.signal() == SIGINT) {
					print("[SIGINT]\n");
					break;
				}
				else if (ex.signal() == SIGALRM) {
					print("[SIGALRM]\n");
					::alarm(1);
				}
				else if (ex.signal() == SIGWINCH) {
					print("[SIGWINCH]\n");
				}
				else if (ex.signal() == SIGTSTP) {
					print("[SIGTSTP]\n");
					unhookSignal(SIGTSTP);
					kill(SIGTSTP);
				}
				else if (ex.signal() == SIGCONT) {
					print("[SIGCONT]\n");
				}
				else
					print("[%%]\n", ex.signal());
			}
		}
	}
};

int main(int argc, char **argv)
{
	auto signalEater = SignalEater::create();
	Thread::blockAllSignals();
	::alarm(1);
	signalEater->start();
	signalEater->wait();
	return 0;
}
