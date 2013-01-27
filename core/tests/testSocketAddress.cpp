#include <ftl/stdio>
#include <ftl/sockets>

namespace ftl
{

String familyToString(int family)
{
	String s("UNKNOWN");
	if (family == AF_INET) s = "INET";
	else if (family == AF_INET6) s = "INET6";
	else if (family == AF_UNSPEC) s = "UNSPEC";
	return s;
}

String socketTypeToString(int type)
{
	String s("UNKNOWN");
	if (type == SOCK_DGRAM) s = "DGRAM";
	else if (type == SOCK_STREAM) s = "STREAM";
	return s;
}

String protocolToString(int protocol)
{
	String s("UNKNOWN");
	if (protocol == IPPROTO_TCP) s = "TCP";
	else if (protocol == IPPROTO_UDP) s = "UDP";
	else s = Format("<%%>") << protocol;
	return s;
}

int main(int argc, char **argv)
{
	String hostName = SocketAddress::hostName();

	if (argc == 2)
		hostName = argv[1];

	print("hostName = \"%%\"\n", hostName);

	String canonicalName;
	O<SocketAddressList> list = SocketAddress::resolve(hostName, "", AF_UNSPEC, SOCK_STREAM, &canonicalName);

	print("canonicalName = \"%%\"\n", canonicalName);

	for (int i = 0; i < list->length(); ++i)
	{
		SocketAddress *address = list->at(i);
		bool failed;

		print("%% : %% : %% : %% : %% : %% : %%\n"
			, familyToString(address->family())
			, address->toString()
			, address->port()
			, socketTypeToString(address->socketType())
			, protocolToString(address->protocol())
			, address->lookupHostName(&failed)
			, address->lookupServiceName()
		);
	}

	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
