#ifndef RGET_SERVICE_HPP
#define RGET_SERVICE_HPP

#include <pona/stdio>
#include <pona/network>
#include <pona/event>
#include "LogFile.hpp"

namespace rget
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
	Ref<StreamSocket, Owner> openTunnel(String entity);
	void canonSession(Ref<StreamSocket> socket, String entity);
	void binarySession(Ref<StreamSocket> socket, String entity);
	
private:
	Ref<LogFile, Owner> connectLog_;
	int repeat_;
	bool loop_;
	int fileIndex_; // file index
	
	Ref<EventManager, Owner> abortEvent_;
};

} // namespace rget

#endif // RGET_SERVICE_HPP
