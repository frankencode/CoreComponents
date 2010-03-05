/*
 * Options.hpp -- application-wide parameters
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_OPTIONS_HPP
#define RIO_OPTIONS_HPP

#include <pona/process>
#include <pona/threads>
#include <pona/network>

namespace rio
{

using namespace pona;

class Options: public pona::Options
{
public:
	static Ref<Options> instance();
	
	void read(int argc, char** argv);
	
	Ref<SocketAddress, Owner> address() const;
	
	Variant server_, client_, help_;
	Variant host_, port_, inet6_, backlog_;
	Variant canon_, editor_, eol_, ioUnit_;
	Variant quiet_, logging_, logDir_;
	Variant exec_, loop_, repeat_;
	Ref<UStringList, Owner> files_;
	
	UString editorPath_;
	int loggingFlags_;
	
private:
	static Mutex lock_;
	Options();
};

inline Ref<Options> options() { return Options::instance(); }

} // namespace rio

#endif // RIO_OPTIONS_HPP
