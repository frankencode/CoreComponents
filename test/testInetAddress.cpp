#include <pona/Core.hpp>

namespace pona
{

int main(int argc, char** argv)
{
	String hostName = SocketAddress::hostName();

	if (argc == 2)
		hostName = argv[1];
	
	String canonicalName;
	Ref<InetAddressList, Owner> list = SocketAddress::query(hostName, "http", AF_UNSPEC, 0, &canonicalName);
	
	output()->write(format("hostName, canonicalName = \"%%\", \"%%\"\n") % hostName % canonicalName);
	
	for (int i = 0; i < list->length(); ++i)
	{
		Ref<SocketAddress> address = list->get(i);
		bool failed;
		
		output()->write(
			format("%% : %% : %% : %% : %% : %% : %%\n")
			% address->familyString()
			% address->addressString()
			% address->port()
			% address->socketTypeString()
			% address->protocolString()
			% address->lookupHostName(&failed)
			% address->lookupServiceName()
		);
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
