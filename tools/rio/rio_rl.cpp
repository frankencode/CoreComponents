/*
 * rio_rl.cpp -- default line editor using the readline library
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <pona/stdio>
#include <pona/process>
#include <pona/threads>
#include <pona/events>
#include <termios.h> // termios
#include <unistd.h> // tcgetattr, tcsetattr
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

namespace rio_rl
{

using namespace pona;

class LineForwarder: public Thread
{
public:
	LineForwarder(Ref<SystemStream> source)
		: source_(source),
		  lineSource_(new LineSource(source)),
		  done_(false),
		  finished_(false),
		  failed_(false)
	{}
	
	virtual void run()
	{
		failed_ = false;
		try {
			while (!done_)
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
					if (eoi || done_) break;
					#if RL_VERSION_MAJOR >= 5
					rl_forced_update_display();
					#else
					#warning(Using unsupported version of the readline library.)
					#endif
				}
			}
		}
		catch (AnyException& ex) {
			printTo(pona::error(), "(rio_rl) LineForwarder: %%\n", ex.what());
			failed_ = true;
		}
		finished_ = true;
	}
	void finish() { done_ = true; }
	bool finished() const { return finished_; }
	bool failed() const { return failed_; }
private:
	Ref<SystemStream, Owner> source_;
	Ref<LineSource, Owner> lineSource_;
	bool done_;
	bool finished_;
	bool failed_;
};

int main(int argc, char** argv)
{
	Variant fdr = -1;
	Variant fds = 1;
	
	Options options;
	options.define(0, "fdr", &fdr, "Receiving file descriptor");
	options.define(0, "fds", &fds, "Sending file descriptor");
	options.read(argc, argv);
	
	Ref<LineForwarder, Owner> forwarder = new LineForwarder(new SystemStream(fdr));
	forwarder->start();
	
	Ref<SystemStream, Owner> rawOutput = new SystemStream(fds);
	
	while (true) {
		char* buf = readline("");
		if ((!buf) || forwarder->finished()) break;
		
		int len = strlen(buf);
		buf[len] = '\n';
		
		rawOutput->write(buf, len + 1);
		
		::free(buf);
	}
	
	forwarder->wait();
	
	return 0;
}

} // namespace rio_rl

int main(int argc, char** argv)
{
	int ret = -1;
	#ifdef NDEBUG
	try {
	#endif
		ret = rio_rl::main(argc, argv);
	#ifdef NDEBUG
	}
	catch (pona::AnyException& ex) {
		pona::printTo(pona::error(), "(%%) %%\n", pona::Path(argv[0]).fileName(), ex.what());
	}
	#endif
	return ret;
}
