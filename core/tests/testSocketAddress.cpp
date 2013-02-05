#include <ftl/PrintDebug.hpp>
#include <ftl/SocketAddress.hpp>

namespace ftl
{

string familyToString(int family)
{
	string s("UNKNOWN");
	if (family == AF_INET) s = "INET";
	else if (family == AF_INET6) s = "INET6";
	else if (family == AF_UNSPEC) s = "UNSPEC";
	return s;
}

string socketTypeToString(int type)
{
	string s("UNKNOWN");
	if (type == SOCK_DGRAM) s = "DGRAM";
	else if (type == SOCK_STREAM) s = "STREAM";
	return s;
}

string protocolToString(int protocol)
{
	string s("UNKNOWN");
	if (protocol == IPPROTO_TCP) s = "TCP";
	else if (protocol == IPPROTO_UDP) s = "UDP";
	else s = format("<%%>") << protocol;
	return s;
}

int main(int argc, char **argv)
{
	string hostName = SocketAddress::hostName();

	if (argc == 2)
		hostName = argv[1];

	print("hostName = \"%%\"\n", hostName);

	string canonicalName;
	hook<SocketAddressList> list = SocketAddress::resolve(hostName, "", AF_UNSPEC, SOCK_STREAM, &canonicalName);

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
