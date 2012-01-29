#include <ftl/XAuthFile.hpp>
#include <ftl/stdio>

int main() {
	using namespace ftl;
	
	Ref<XAuthFile, Owner> authFile = new XAuthFile;
	Ref<XAuthRecords> records = authFile->records();
	for (int i = 0, n = records->length(); i < n; ++i) {
		Ref<XAuthRecord> record = records->get(i);
		print(record->host);
		if (record->family == AF_INET)
			print("/inet");
		else if (record->family == AF_INET6)
			print("/inet6");
		else if (record->family == AF_LOCAL)
			print("/unix");
		print(":%%  %%  ", record->display, record->protocol);
		for (int i = 0, n = record->data->length(); i < n; ++i)
			print("%hex:2:'0'%", uint8_t(record->data->get(i)));
		print("\n");
	}
	
	return 0;
}
