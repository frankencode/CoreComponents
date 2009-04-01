#include <pona/stdio> // DEBUG
#include "Options.hpp"

namespace nocat
{

Options::Options(int argc, char** argv)
	: server_(false), client_(false), help_(false),
	  host_("*"), port_(7373), inet6_(false),
	  exec_(false),
	  pool_(3), backlog_(8), ioUnit_(0x8000),
	  loop_(false), repeat_(1),
	  timeout_(0),
	  bench_(false)
{
	define('s', "server", &server_, "Start in server mode");
	define('c', "client", &client_, "Start in client mode");
	define(0, "host", &host_, "Host name or address");
	define(0, "port", &port_, "Port number");
	define('6', "inet6", &inet6_, "Use IPv6");
	define('e', "exec", &exec_, "Execute each file");
	define(0, "pool", &pool_, "Maximum number of concurrent connections");
	define(0, "backlog", &backlog_, "Backlog length of listening socket");
	define(0, "io_unit", &ioUnit_, "I/O buffer size");
	define('l', "loop", &loop_, "Endless repeat sending/serving the files/programs list");
	define(0, "repeat", &repeat_, "Send/serve the files/programs exactly n times");
	define(0, "timeout", &timeout_, "Termination timeout in seconds");
	define('b', "bench", &bench_, "Print performance statistics");
	define('h', "help", &help_, "Print help");
	files_ = ToolOptions::read(argc, argv);
}

Ref<SocketAddress, Owner> Options::address() const
{
	Ref<SocketAddressList, Owner> choice = SocketAddress::resolve(String(host_), Format("%%") << int(port_), bool(inet6_) ? AF_INET6 : AF_UNSPEC);
	return choice->first();
}

} // namespace nocat
