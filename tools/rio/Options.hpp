/*
 * Options.hpp -- application-wide parameters
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_OPTIONS_HPP
#define RIO_OPTIONS_HPP

#include <pona/context>
#include <pona/thread>
#include <pona/network>

namespace rio
{

using namespace pona;

class Options: public pona::Options
{
	PONA_SHARED
	
public:
	static Ref<Options> instance();
	
	void read(int argc, char** argv);
	
	Ref<SocketAddress, Owner> address() const;
	
	Variant server_, client_, help_;
	Variant host_, port_, inet6_, backlog_;
	Variant canon_, editor_, eol_, ioUnit_;
	Variant quiet_, logging_, logDir_;
	Variant exec_, loop_, repeat_;
	Ref<StringList, Owner> files_;
	
	String editorPath_;
	int loggingFlags_;
	
private:
	static Mutex lock_;
	Options();
};

inline Ref<Options> options() { return Options::instance(); }

} // namespace rio

#endif // RIO_OPTIONS_HPP
