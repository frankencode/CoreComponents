#include "Shell.hpp"
#include "Client.hpp"

namespace nocat
{

Client::Client(Ref<Options> options)
	: options_(options)
{}

int Client::connect()
{
	int ret = 0;
	try {
		print(
			"(%%) Connecting to %%:%% (press Ctrl-C to exit)\n",
			options_->toolName(), options_->host_, options_->port_,
			error()
		);
		if (options_->files_->length() == 0)
		{
			Ref<SystemStream, Owner> stream = StreamSocket::connect(options_->address());
			
			if (rawInput()->interactive())
				Shell(options_).session(stream);
			else
				Shell(options_).transfer(stream);
		}
		else
		{}
	}
	catch (std::Exception& ex) {
		print("(%%) %%\n", options_->toolName(), ex.what(), error());
		ret = 1;
	}
	return ret;
}

} // namespace nocat
