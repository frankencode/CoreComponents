#include <pona/stdio>
#include "Options.hpp"
#include "Client.hpp"
#include "Server.hpp"

namespace nocat
{

using namespace pona;

int main(int argc, char** argv)
{
	Ref<Options, Owner> options = new Options(argc, argv);
	int ret = 0;
	
	if (options->client_) {
		ret = Client(options).connect();
	}
	else if (options->server_) {
		Server(options).listen();
	}
	else if (options->help_) {
		print(options->help());
	}
	else {
		print("Missing parameter, try '%% --help'.\n", options->toolName());
	}
	
	return ret;
}

} // namespace nocat

int main(int argc, char** argv)
{
	return nocat::main(argc, argv);
}
