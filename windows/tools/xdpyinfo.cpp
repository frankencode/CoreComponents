#include <ftl/XClient.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	
	Ref<XClient, Owner> client = new XClient;
	print("protocol version: %%.%%\n", client->displayInfo()->majorVersion, client->displayInfo()->minorVersion);
	print("vendor: %%\n", client->displayInfo()->vendor);
	Ref<XDisplayInfo::PixmapFormats> pixmapFormats = client->displayInfo()->pixmapFormats;
	for (int i = 0; i < pixmapFormats->length(); ++i) {
		Ref<XPixmapFormatInfo> format = pixmapFormats->at(i);
		print("pixmap format: depth: %2.%, bpp: %2.%, padding: %2.%\n", format->depth, format->bpp, format->padding);
	}
	
	return 0;
}
