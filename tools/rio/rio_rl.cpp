/*
 * rio_rl.cpp -- default line editor using the readline library
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <ftl/stdio>
#include <ftl/process>
#include <ftl/threads>
#include <ftl/events>
#include <termios.h> // termios
#include <unistd.h> // tcgetattr, tcsetattr
#include <stdio.h>
#include <stdlib.h> // free
#include <readline/readline.h>
#include <readline/history.h>

namespace rio_rl
{

using namespace ftl;

class LineForwarder: public Thread
{
public:
	LineForwarder(Ref<SystemStream> source)
		: source_(source.get()),
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
					bool eoi = !lineSource_->readAvail();
					if (lineSource_->cachedLines() > 0)
						print("\r\033[K");
					while (lineSource_->cachedLines() > 0)
						output()->writeLine(lineSource_->readLine());
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
			printTo(ftl::error(), "(rio_rl) LineForwarder: %%\n", ex.what());
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

#if RL_VERSION_MAJOR >= 5
class ReadlineCleanup: public Action {
public:
	ReadlineCleanup(int signal)
		: signal_(signal)
	{}
private:
	virtual void run() {
		rl_free_line_state();
		rl_cleanup_after_signal();
		rl_catch_sigwinch = 1;
		rl_set_signals();
		Process::raise(SIGALRM);
	}
	int signal_;
};
#endif

int main(int argc, char** argv)
{
#if RL_VERSION_MAJOR >= 5
	signalEvent(SIGINT)->pushBack(new ReadlineCleanup(SIGINT));
	signalEvent(SIGQUIT)->pushBack(new ReadlineCleanup(SIGQUIT));
	signalEvent(SIGTERM)->pushBack(new ReadlineCleanup(SIGTERM));
	signalEvent(SIGTSTP)->pushBack(new ReadlineCleanup(SIGTSTP));
	signalEvent(SIGTTIN)->pushBack(new ReadlineCleanup(SIGTTIN));
	signalEvent(SIGTTOU)->pushBack(new ReadlineCleanup(SIGTTOU));
#endif
	
	CommandLine options;
	Ref<CommandOption> fdr = options.define('r', "fdr", -1, "Receiving file descriptor");
	Ref<CommandOption> fds = options.define('s', "fds", 1, "Sending file descriptor");
	options.read(argc, argv);
	
	Ref<LineForwarder, Owner> forwarder = new LineForwarder(new SystemStream(fdr->value()));
	forwarder->start();
	
	Ref<SystemStream, Owner> rawOutput = new SystemStream(fds->value());
	
	while (true) {
		char* buf = readline("");
		if ((!buf) || forwarder->finished()) break;
		
		int len = str::len(buf);
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
	catch (ftl::AnyException& ex) {
		ftl::printTo(ftl::error(), "(%%) %%\n", ftl::Path(argv[0]).fileName(), ex.what());
	}
	#endif
	return ret;
}
