#include <pona/stdio>
#include <pona/network>

namespace pona
{

int main(int argc, char** argv)
{
	String hostName = SocketAddress::hostName();

	if (argc == 2)
		hostName = argv[1];
	
	print("hostName = \"%%\"\n", hostName);
	
	String canonicalName;
	Ref<InetAddressList, Owner> list = SocketAddress::query(hostName, "ssh", AF_UNSPEC, 0, &canonicalName);
	
	print("canonicalName = \"%%\"\n", canonicalName);
	
	for (int i = 0; i < list->length(); ++i)
	{
		Ref<SocketAddress> address = list->get(i);
		bool failed;
		
		print("%% : %% : %% : %% : %% : %% : %%\n"
			, address->familyString()
			, address->addressString()
			, address->port()
			, address->socketTypeString()
			, address->protocolString()
			, address->lookupHostName(&failed)
			, address->lookupServiceName()
		);
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
