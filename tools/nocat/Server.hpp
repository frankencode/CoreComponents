#ifndef NOCAT_SERVER_HPP
#define NOCAT_SERVER_HPP

#include <pona/stdio>
#include <pona/network>

namespace nocat
{

using namespace pona;

class Server: public StreamSocket
{
public:
	Server(Ref<Options> options);
	
	virtual void init();
	virtual void serve(Ref<SocketAddress> address, Ref<SystemStream> stream);
	
private:
	Ref<Options, Owner> options_;
	int repeat_;
	bool loop_;
	int fi_; // file index
};

} // namespace nocat

#endif // NOCAT_SERVER_HPP
