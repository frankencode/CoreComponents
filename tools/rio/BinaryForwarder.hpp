/*
 * BinaryForwarder.hpp -- binary I/O transfer thread
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_BINARYFORWARDER_HPP
#define RIO_BINARYFORWARDER_HPP

#include <pona/stdio>
#include <pona/thread>
#include <pona/event>
#include "Options.hpp"
#include "LogFile.hpp"

namespace rio
{

using namespace pona;

class BinaryForwarder: public Thread
{
public:
	BinaryForwarder(
		Ref<SystemStream> source,
		Ref<SystemStream> sink,
		Ref<LogFile> recvLog,
		Ref<Event> cancelEvent
	);
	
	void finish();
	uint64_t bytesTransferred() const;
	
private:
	virtual int run();
	
	Ref<SystemStream, Owner> source_, sink_;
	Ref<LogFile, Owner> recvLog_;
	Ref<Event, Owner> cancelEvent_;
	Ref<Action, Owner> finishAction_;
	bool done_;
	Array<uint8_t> buf_;
	uint64_t bytesTransferred_;
};

} // namespace rio

#endif // RIO_BINARYFORWARDER_HPP
