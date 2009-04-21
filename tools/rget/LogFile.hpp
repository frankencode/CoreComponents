#ifndef RGET_LOGFILE_HPP
#define RGET_LOGFILE_HPP

#include <pona/stdio>
#include <pona/time>
#include <pona/SocketAddress.hpp>

namespace rget
{

using namespace pona;

class LogFile: public Instance
{
public:
	enum Type { Connect = 1, Recv = 2, Send = 4, Merged = 8 };
	
	LogFile(Ref<SocketAddress> address, int type, Time t0 = now(), Ref<LogFile> merged = 0);
	
	void writeLine(Ref<SocketAddress> address, String data);
	void writeLine(String data);
	
	void write(uint8_t* buf, int bufFill);
	
protected:
	Ref<SocketAddress, Owner> address_;
	int type_;
	Ref<File, Owner> file_;
	Ref<LineSink, Owner> lineSink_;
	Ref<LogFile, Owner> merged_;
};

} // namespace rget

#endif // RGET_LOGFILE_HPP
