#include <ftl/XClient.hpp>
#include <ftl/streams>

int main()
{
	using namespace ftl;
	
	Ref<XClient, Owner> client = new XClient;
	print("protocol version: %%.%%\n", client->majorVersion(), client->minorVersion());
	print("vendor: %%\n", client->vendor());
	Ref< Array<XPixmapFormat> > pixmapFormats = client->pixmapFormats();
	for (int i = 0, n = pixmapFormats->length(); i < n; ++i) {
		XPixmapFormat* format = pixmapFormats->pointerAt(i);
		print("pixmap format: depth: %2.%, bpp: %2.%, padding: %2.%\n", format->depth, format->bpp, format->padding);
	}
	
	return 0;
}
