#include <ftl/PrintDebug.hpp>
#include <ftl/Thread.hpp>
#include <ftl/Process.hpp>

using namespace ftl;

class SignalEater: public Thread {
public:
	static hook<SignalEater> create() {
		return new SignalEater;
	}

private:
	SignalEater() {}

	virtual void run()
	{
		hook<SignalSet> set = SignalSet::createEmpty();
		set->insert(SIGINT);
		set->insert(SIGALRM);
		set->insert(SIGWINCH);
		set->insert(SIGTSTP);
		set->insert(SIGCONT);
		unblockSignals(set);

		while (true) {
			try {
				string line = input()->readLine();
				if (line == "") break;
			}
			catch (Interrupt& ex) {
				if (ex.signal() == SIGINT) {
					print("[SIGINT]\n");
					break;
				}
				else if (ex.signal() == SIGALRM) {
					print("[SIGALRM]\n");
					Process::alarm(1);
				}
				else if (ex.signal() == SIGWINCH) {
					print("[SIGWINCH]\n");
				}
				else if (ex.signal() == SIGTSTP) {
					print("[SIGTSTP]\n");
					Process::raise(SIGSTOP);
					print("\n[SIGCONT]\n");
					Process::alarm(1);
				}
				else if (ex.signal() == SIGCONT) {
					print("\n[SIGCONT] (2)\n");
				}
				else
					print("[%%]\n", ex.signal());
			}
		}
	}
};

int main(int argc, char **argv)
{
	Process::throwSignal(SIGINT);
	Process::throwSignal(SIGALRM);
	Process::throwSignal(SIGWINCH);
	Process::throwSignal(SIGTSTP);
	Process::throwSignal(SIGCONT);
	Thread::blockSignals(SignalSet::createFull());
	Process::alarm(1);
	hook<SignalEater> signalEater = SignalEater::create();
	signalEater->start();
	signalEater->wait();
	return 0;
}
