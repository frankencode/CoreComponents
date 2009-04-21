#ifndef RGET_BINARYFORWARDER_HPP
#define RGET_BINARYFORWARDER_HPP

#include <pona/stdio>
#include <pona/thread>
#include <pona/event>
#include "Options.hpp"
#include "LogFile.hpp"

namespace rget
{

using namespace pona;

class BinaryForwarder: public Thread
{
public:
	BinaryForwarder(
		Ref<SystemStream> source,
		Ref<SystemStream> sink,
		Ref<LogFile> recvLog,
		Ref<EventManager> abortEvent
	);
	~BinaryForwarder();
	
	virtual int run();
	void abort();
	
	uint64_t bytesTransferred() const;
	
private:
	Ref<SystemStream, Owner> source_, sink_;
	Ref<LogFile, Owner> recvLog_;
	bool abort_;
	Ref<EventManager, Owner>  abortEvent_;
	Ref<EventHandler, Owner> abortHandler_;
	Array<uint8_t> buf_;
	uint64_t bytesTransferred_;
};

} // namespace rget

#endif // RGET_BINARYFORWARDER_HPP
