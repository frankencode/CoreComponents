#include <pona/stdio>
#include <pona/context>
#include <pona/thread>
#include <pona/event>
#include <termios.h> // termios
#include <unistd.h> // tcgetattr, tcsetattr
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

namespace rget_rl
{

using namespace pona;

class LineForwarder: public Thread
{
public:
	LineForwarder(Ref<SystemStream> source)
		: source_(source),
		  lineSource_(new LineSource(source)),
		  abort_(false),
		  finished_(false)
	{}
	virtual int run() {
		int ret = 0;
		try {
			while (!abort_)
			{
				if (source_->readyRead(1))
				{
					bool eoi = false;
					lineSource_->readAvail(&eoi);
					if (lineSource_->cachedLines() > 0)
						print("\r\033[K");
					while (lineSource_->cachedLines() > 0) {
						String line = lineSource_->readLine();
						output()->writeLine(line);
					}
					if (eoi || abort_) break;
					#if RL_VERSION_MAJOR >= 5
					rl_forced_update_display();
					#else
					#warning(Using unsupported version of the readline library.)
					#endif
				}
			}
		}
		catch (AnyException& ex) {
			printTo(error(), "(rget_rl) LineForwarder: %%\n", ex.what());
			ret = 1;
		}
		finished_ = true;
		return ret;
	}
	void abort() { abort_ = true; }
	bool finished() const { return finished_; }
private:
	Ref<SystemStream, Owner> source_;
	Ref<LineSource, Owner> lineSource_;
	bool abort_;
	bool finished_;
};

int main(int argc, char** argv)
{
	Variant fdr = -1;
	Variant fds = 1;
	
	ToolOptions options;
	options.define(0, "fdr", &fdr, "Receiving file descriptor");
	options.define(0, "fds", &fds, "Sending file descriptor");
	options.read(argc, argv);
	
	Ref<LineForwarder, Owner> forwarder = new LineForwarder(new SystemStream(fdr));
	forwarder->start();
	
	Ref<SystemStream, Owner> rawOutput = new SystemStream(fds);
	
	while (true) {
		char* buf = readline("");
		if ((!buf) || (forwarder->finished())) break;
		
		int len = strlen(buf);
		buf[len] = '\n';
		
		rawOutput->write(buf, len + 1);
		
		::free(buf);
	}
	
	forwarder->wait();
	
	return 0;
}

} // namespace rget_rl

int main(int argc, char** argv)
{
	return rget_rl::main(argc, argv);
}
