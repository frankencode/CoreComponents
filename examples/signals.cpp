#include <flux/stdio>
#include <flux/Thread>
#include <flux/Process>

using namespace flux;

class SignalEater: public Thread {
public:
    static Ref<SignalEater> create() {
        return new SignalEater;
    }

private:
    SignalEater() {}

    virtual void run()
    {
        Ref<SignalSet> set = SignalSet::createEmpty();
        set->insert(SIGINT);
        set->insert(SIGALRM);
        set->insert(SIGWINCH);
        set->insert(SIGTSTP);
        set->insert(SIGCONT);
        unblockSignals(set);

        while (true) {
            try {
                String line;
                lineInput()->read(&line);
                if (line == "") break;
            }
            catch (Interrupt& ex) {
                if (ex.signal() == SIGINT) {
                    fout("[SIGINT]\n");
                    break;
                }
                else if (ex.signal() == SIGALRM) {
                    fout("[SIGALRM]\n");
                    Process::alarm(1);
                }
                else if (ex.signal() == SIGWINCH) {
                    fout("[SIGWINCH]\n");
                }
                else if (ex.signal() == SIGTSTP) {
                    fout("[SIGTSTP]\n");
                    Process::raise(SIGSTOP);
                    fout("\n[SIGCONT]\n");
                    Process::alarm(1);
                }
                else if (ex.signal() == SIGCONT) {
                    fout("\n[SIGCONT] (2)\n");
                }
                else
                    fout("[%%]\n") << ex.signal();
            }
        }
    }
};

int main()
{
    Process::enableInterrupt(SIGINT);
    Process::enableInterrupt(SIGALRM);
    Process::enableInterrupt(SIGWINCH);
    Process::enableInterrupt(SIGTSTP);
    Process::enableInterrupt(SIGCONT);
    Thread::blockSignals(SignalSet::createFull());
    Process::alarm(1);
    Ref<SignalEater> signalEater = SignalEater::create();
    signalEater->start();
    signalEater->wait();
    return 0;
}
