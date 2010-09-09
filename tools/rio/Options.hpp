/*
 * Options.hpp -- application-wide parameters
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_OPTIONS_HPP
#define RIO_OPTIONS_HPP

#include <ftl/stdio>
#include <ftl/threads>
#include <ftl/network>

namespace rio
{

using namespace ftl;

class Options: public CommandLine, public Singleton<Options>
{
public:
	void read(int argc, char** argv);
	
	Ref<SocketAddress, Owner> address() const;
	
	inline bool   server()  const { return server_->value(); }
	inline bool   client()  const { return client_->value(); }
	inline bool   help()    const { return help_->value(); }
	inline String host()    const { return host_->value(); }
	inline int    port()    const { return port_->value(); }
	inline bool   inet6()   const { return inet6_->value(); }
	inline int    backlog() const { return backlog_->value(); }
	inline bool   canon()   const { return canon_->value(); }
	inline String editor()  const { return editorPath_; }
	inline String eol()     const { return eol_->value(); }
	inline int    ioUnit()  const { return ioUnit_->value(); }
	inline bool   quiet()   const { return quiet_->value(); }
	inline int    logging() const { return loggingFlags_; }
	inline String logDir()  const { return logDir_->value(); }
	inline bool   exec()    const { return exec_->value(); }
	inline bool   loop()    const { return loop_->value(); }
	inline int    repeat()  const { return repeat_->value(); }
	
	inline Ref<StringList> files() const { return files_; }
	
private:
	friend class Singleton<Options>;
	
	Options();
	
	Ref<CommandOption> server_, client_, help_;
	Ref<CommandOption> host_, port_, inet6_, backlog_;
	Ref<CommandOption> canon_, editor_, eol_, ioUnit_;
	Ref<CommandOption> quiet_, logging_, logDir_;
	Ref<CommandOption> exec_, loop_, repeat_;
	Ref<StringList, Owner> files_;
	
	String editorPath_;
	int loggingFlags_;
};

inline Ref<Options> options() { return Options::instance(); }

} // namespace rio

#endif // RIO_OPTIONS_HPP
