/*
 * Options.cpp -- application-wide parameters
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <pona/stdio>
#include "LogFile.hpp"
#include "Options.hpp"

namespace rio
{

Mutex Options::lock_;

Ref<Options> Options::instance()
{
	lock_.acquire();
	static Ref<Options, Owner> instance = 0;
	if (!instance)
		instance = new Options;
	lock_.release();
	return instance;
}

Options::Options()
	: server_(false), client_(false), help_(false),
	  host_("*"), port_(7373), inet6_(false), backlog_(8),
	  canon_(false), editor_("rio_rl"), eol_("crlf"), ioUnit_(0x8000),
	  quiet_(false), logging_(""), logDir_(cwd()),
	  exec_(false), loop_(false), repeat_(1),
	  loggingFlags_(0)
{
	define('s', "server", &server_, "Server mode");
	define('c', "client", &client_, "Client mode");
	define('t', "host", &host_, "Host name or address");
	define('p', "port", &port_, "Port number");
	define('6', "inet6", &inet6_, "Prefer IPv6");
	define('b', "backlog", &backlog_, "Backlog length of listening socket");
	define('a', "canon", &canon_, "Line-vise I/O mode");
	define('e', "editor", &editor_, "Line editor");
	define('n', "eol", &eol_, "Line ending style ('crlf' or 'nl')");
	define('u', "io_unit", &ioUnit_, "I/O buffer size");
	define('q', "quiet", &quiet_, "Keep quiet");
	define('g', "logging", &logging_, "Logging flags (connect,recv,send,merged)");
	define('d', "log_dir", &logDir_, "Target directory for log files");
	define('x', "exec", &exec_, "Execute each [PROGRAM]");
	define('l', "loop", &loop_, "Endless repeat serving connections");
	define('r', "repeat", &repeat_, "Repeat serving exactly n connections");
	define('h', "help", &help_, "Print help");
	entity("FILE|PROGRAM");
}

void Options::read(int argc, char** argv)
{
	files_ = pona::Options::read(argc, argv);
	
	if (help_) {
		logDir_ = "<CWD>";
	}
	else {
		int modeSum = int(bool(client_)) + int(bool(server_));
		if (modeSum == 0)
			PONA_THROW(Exception, "Missing option (--server or --client).");
		else if (modeSum > 1)
			PONA_THROW(Exception, "Contradicting option (--server or --client).");
		
		String eol = eol_;
		if (eol == "nl") { eol_ = "\012"; }
		else if (eol == "crlf") eol_ = "\015\012";
		
		if (canon_) {
			Ref<StringList, Owner> dirs = env("PATH") / ':';
			if (!dirs->contains(execDir()))
				dirs->append(execDir());
			debug("execDir = %%\n", execDir());
			debug("dirs = %%\n", dirs * ':');
			editorPath_ = lookupPath(dirs, editor_);
			if (editorPath_ == "")
				PONA_THROW(Exception, "Editor program could not be found.");
			debug("editorPath_ = %%\n", editorPath_);
		}
		
		loggingFlags_ = 0;
		Ref<StringList, Owner> logs = String(options()->logging_) / ',';
		for (int i = 0; i < logs->length(); ++i) {
			String log = logs->get(i);
			if (log == "");
			else if (log == "connect") loggingFlags_ |= LogFile::Connect;
			else if (log == "recv") loggingFlags_ |= LogFile::Recv;
			else if (log == "send") loggingFlags_ |= LogFile::Send;
			else if (log == "merged") loggingFlags_ |= LogFile::Merged;
			else
				PONA_THROW(Exception, "Incorrect '--logging=' syntax.");
		}
		
		if (exec_ && (files_->length() == 0))
			PONA_THROW(Exception, "No file passed to execute.");
	}
}

Ref<SocketAddress, Owner> Options::address() const
{
	Ref<SocketAddressList, Owner> choice = SocketAddress::resolve(String(host_), Format("%%") << int(port_), bool(inet6_) ? AF_INET6 : AF_UNSPEC, SOCK_STREAM);
	return choice->first();
}

} // namespace rio
