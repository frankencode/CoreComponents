/*
 * Service.hpp -- serving connections
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_SERVICE_HPP
#define RIO_SERVICE_HPP

#include <pona/stdio>
#include <pona/network>
#include <pona/events>
#include "LogFile.hpp"

namespace rio
{

using namespace pona;

class Service: public StreamSocket
{
public:
	Service();
	
protected:
	virtual void init();
	virtual void idle();
	virtual void serve(Ref<StreamSocket> socket);
	virtual void cleanup();
	
	Ref<Process, Owner> exec(String entity);
	void canonSession(Ref<StreamSocket> socket, String entity);
	void binarySession(Ref<StreamSocket> socket, String entity);
	
private:
	Ref<LogFile, Owner> connectLog_;
	int repeat_;
	bool loop_;
	int fileIndex_; // file index
	
	Ref<Event, Owner> ioCancelEvent_;
	Ref<Event, Owner> cancelEvent_;
};

} // namespace rio

#endif // RIO_SERVICE_HPP
