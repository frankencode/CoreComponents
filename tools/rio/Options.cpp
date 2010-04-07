/*
 * Options.cpp -- application-wide parameters
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */

#include <pona/process>
#include "LogFile.hpp"
#include "Options.hpp"

namespace rio
{

Options::Options()
	: loggingFlags_(0)
{
	String cwd = Process::cwd();
	
	server_  = define('s', "server",  false,    "Server mode");
	client_  = define('c', "client",  false,    "Client mode");
	host_    = define('t', "host",    "*",      "Host name or address");
	port_    = define('p', "port",    7373,     "Port number");
	inet6_   = define('6', "inet6",   false,    "Prefer IPv6");
	backlog_ = define('b', "backlog", 8,        "Backlog length of listening socket");
	canon_   = define('a', "canon",   false,    "Line-vise I/O mode");
	editor_  = define('e', "editor",  "rio_rl", "Line editor");
	eol_     = define('n', "eol",     "crlf",   "Line ending style ('crlf' or 'nl')");
	ioUnit_  = define('u', "io_unit", 0x8000,   "I/O buffer size");
	quiet_   = define('q', "quiet",   false,    "Keep quiet");
	logging_ = define('g', "logging", "",       "Logging flags (connect,recv,send,merged)");
	logDir_  = define('d', "log_dir", cwd,      "Target directory for log files");
	exec_    = define('x', "exec",    false,    "Execute each [PROGRAM]");
	loop_    = define('l', "loop",    false,    "Endless repeat serving connections");
	repeat_  = define('r', "repeat",  1,        "Repeat serving exactly n connections");
	help_    = define('h', "help",    false,    "Print help");
	
	entity("FILE|PROGRAM");
}

void Options::read(int argc, char** argv)
{
	files_ = CommandLine::read(argc, argv);
	
	if (help_->value()) {
		logDir_->setDefaultValue("<CWD>");
	}
	else {
		int modeSum = int(bool(client_->value())) + int(bool(server_->value()));
		if (modeSum == 0)
			PONA_THROW(Exception, "Missing option (--server or --client).");
		else if (modeSum > 1)
			PONA_THROW(Exception, "Contradicting option (--server or --client).");
		
		String eol = eol_->value();
		if (eol == "nl") eol_->setValue("\012");
		else if (eol == "crlf") eol_->setValue("\015\012");
		
		if (canon_->value()) {
			Ref<StringList, Owner> dirs = Process::env("PATH").split(":");
			if (!dirs->contains(execDir()))
				dirs->append(execDir());
			editorPath_ = Path::lookup(dirs, editor_->value());
			if (editorPath_ == "")
				PONA_THROW(Exception, "Editor program could not be found.");
		}
		
		loggingFlags_ = 0;
		Ref<StringList, Owner> logs = String(logging_->value()).split(",");
		for (StringList::Index i = logs->first(); logs->def(i); ++i) {
			String log = logs->at(i);
			if (log == "");
			else if (log == "connect") loggingFlags_ |= LogFile::Connect;
			else if (log == "recv")    loggingFlags_ |= LogFile::Recv;
			else if (log == "send")    loggingFlags_ |= LogFile::Send;
			else if (log == "merged")  loggingFlags_ |= LogFile::Merged;
			else
				PONA_THROW(Exception, "Incorrect '--logging=' syntax.");
		}
		
		if (exec() && (files_->length() == 0))
			PONA_THROW(Exception, "No file passed to execute.");
	}
}

Ref<SocketAddress, Owner> Options::address() const
{
	Ref<SocketAddressList, Owner> choice = SocketAddress::resolve(host(), Format("%%") << port(), inet6() ? AF_INET6 : AF_UNSPEC, SOCK_STREAM);
	return choice->at(0);
}

} // namespace rio
