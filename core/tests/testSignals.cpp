#include <unistd.h> // alarm(2)
#include <ftl/PrintDebug.hpp>
#include <ftl/Thread.hpp>
#include <ftl/Process.hpp>

using namespace ftl;

class SignalEater: public Thread {
public:
	static O<SignalEater> create() {
		return new SignalEater;
	}

private:
	SignalEater() {}

	virtual void run()
	{
		O<SignalSet> set = SignalSet::createEmpty();
		set->insert(SIGINT);
		set->insert(SIGALRM);
		set->insert(SIGWINCH);
		set->insert(SIGTSTP);
		set->insert(SIGCONT);
		unblockSignals(set);

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
					Process::unhookSignal(SIGTSTP);
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
	Process::hookSignal(SIGINT);
	Process::hookSignal(SIGALRM);
	Process::hookSignal(SIGWINCH);
	Process::hookSignal(SIGTSTP);
	Process::hookSignal(SIGCONT);
	Thread::blockSignals(SignalSet::createFull());
	::alarm(1);
	O<SignalEater> signalEater = SignalEater::create();
	signalEater->start();
	signalEater->wait();
	return 0;
}
