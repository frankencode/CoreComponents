#include <pona/Core.hpp>

namespace pona
{

int main(int argc, char** argv)
{
	const char* hostname = "localhost";

	if (argc == 2)
		hostname = argv[1];

	Ref<InetAddressList, Owner> list = InetAddress::query(hostname, "http");
	
	for (int i = 0; i < list->length(); ++i)
	{
		Ref<InetAddress> address = list->get(i);
		output()->write(
			format("%% : %% : %% : %% : %% : %%\n")
			% address->familyString()
			% address->addressString()
			% address->port()
			% address->canonicalName()
			% address->socketTypeString()
			% address->protocolString()
		);
	}
	
#ifdef PONA_WINDOWS
	output()->write("\nPress <RETURM> to continue...\n");
	input()->readLine();
#endif
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
