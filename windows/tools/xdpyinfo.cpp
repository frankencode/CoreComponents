#include <ftl/XClient.hpp>
#include <ftl/streams>

int main()
{
	using namespace ftl;
	
	Ref<XClient, Owner> client = new XClient;
	print("protocol version: %%.%%\n", client->majorVersion(), client->minorVersion());
	print("vendor: %%\n", client->vendor());
	
	return 0;
}
