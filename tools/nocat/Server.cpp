#include "Shell.hpp"
#include "Server.hpp"

namespace nocat
{

Server::Server(Ref<Options> options)
	: StreamSocket(options->address(), 1, options->backlog_),
	  options_(options),
	  repeat_(options->repeat_),
	  loop_(options->loop_),
	  fi_(0)
{}

void Server::init()
{
	print(
		"(%%) Listening on %%:%% (press Ctrl-C to exit)\n",
		options_->toolName(), localAddress()->addressString(), localAddress()->port(),
		error()
	);
}

void Server::serve(Ref<SocketAddress> address, Ref<SystemStream> stream)
{
	print(
		"(%%) Serving %%:%%\n",
		options_->toolName(), address->addressString(), address->port(),
		error()
	);
	
	if (options_->files_->length() == 0)
	{
		if (rawInput()->interactive())
			Shell(options_).session(stream);
		else
			Shell(options_).transfer(stream);
	}
		
	repeat_ -= (repeat_ > 0);
	
	if ((repeat_ == 0) && (!loop_))
		close();
}

} // namespace nocat

