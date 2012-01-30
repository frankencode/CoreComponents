#include <ftl/XClient.hpp>
#include <ftl/stdio>

int main()
{
	using namespace ftl;
	
	Ref<XClient, Owner> client = new XClient;
	
	Ref<XDisplayInfo> displayInfo = client->displayInfo();
	
	print("protocol version: %%.%%\n", displayInfo->majorVersion, displayInfo->minorVersion);
	print("vendor: %%\n", displayInfo->vendor);
	if (displayInfo->vendor->contains("X.Org"))
		print("X.Org version: %%.%%.%%\n",
			displayInfo->releaseNumber / 10000000,
			(displayInfo->releaseNumber / 100000) % 100,
			(displayInfo->releaseNumber / 1000) % 100
		);
	else
		print("release number: %%\n", displayInfo->releaseNumber);
	print("resource ID base: 0x%hex:8.:'0'%\n", displayInfo->resourceIdBase);
	print("resource ID mask: 0x%hex:8.:'0'%\n", displayInfo->resourceIdMask);
	print("motion buffer size: %%\n", displayInfo->motionBufferSize);
	print("maximum request length: %%\n", int(displayInfo->maximumRequestLength) * 4);
	print("image byte order: %%\n", (displayInfo->imageByteOrder == XDisplayInfo::LittleEndian) ? "little endian" : "big endian");
	print("bitmap bit order: %%\n", (displayInfo->bitmapBitOrder == XDisplayInfo::LittleEndian) ? "little endian" : "big endian");
	print("bitmap scanline unit: %%\n", displayInfo->bitmapScanlineUnit);
	print("bitmap scanline pad: %%\n", displayInfo->bitmapScanlinePad);
	print("min keycode: %%\n", displayInfo->minKeyCode);
	print("max keycode: %%\n", displayInfo->maxKeyCode);
	
	Ref<XPixmapInfoArray> pixmapInfo = displayInfo->pixmapInfo;
	for (int i = 0; i < pixmapInfo->length(); ++i) {
		Ref<XPixmapInfo> format = pixmapInfo->at(i);
		print("pixmap format: depth: %2.%, bpp: %2.%, pad: %2.%\n", format->depth, format->bpp, format->pad);
	}
	
	Ref<XScreenInfoArray> screenInfoArray = displayInfo->screenInfo;
	for (int i = 0; i < screenInfoArray->length(); ++i) {
		Ref<XScreenInfo> screenInfo = screenInfoArray->at(i);
		print("screen %%:\n", i);
		print("  root window ID: %%\n", screenInfo->rootWindowId);
		print("  root visual ID: %%\n", screenInfo->rootVisualId);
		print("  default colormap ID: %%\n", screenInfo->defaultColormapId);
		print("  width : %% px / %% mm (%.3% dpi)\n",
			screenInfo->widthInPixels,
			screenInfo->widthInMillimeters,
			25.4 * screenInfo->widthInPixels / screenInfo->widthInMillimeters
		);
		print("  height: %% px / %% mm (%.3% dpi) \n",
			screenInfo->heightInPixels,
			screenInfo->heightInMillimeters,
			25.4 * screenInfo->heightInPixels / screenInfo->heightInMillimeters
		);
		print("  whitePixel: 0x%hex:8.:'0'%\n", screenInfo->whitePixel);
		print("  blackPixel: 0x%hex:8.:'0'%\n", screenInfo->blackPixel);
		print("  inputMask: %%\n", screenInfo->inputMask);
		print("  min installed maps: %%\n", screenInfo->minInstalledMaps);
		print("  max installed maps: %%\n", screenInfo->maxInstalledMaps);
		print("  backing stores: %%\n", screenInfo->backingStores);
		print("  save unders: %%\n", screenInfo->saveUnders);
		print("  root depth: %%\n", screenInfo->rootDepth);
		print("  supported depths: ");
		Ref<XVisualInfoByDepth> visualInfoByDepth = screenInfo->visualInfoByDepth;
		for (int j = 0, m = visualInfoByDepth->length(); j < m; ++j) {
			print("%%", visualInfoByDepth->at(j)->key());
			if (j != m - 1) print(", ");
		}
		print("\n");
		for (int j = 0; j < visualInfoByDepth->length(); ++j) {
			Ref<XVisualInfoArray> visualInfoArray = visualInfoByDepth->at(j)->value();
			for (int k = 0; k < visualInfoArray->length(); ++k) {
				Ref<XVisualInfo> visualInfo = visualInfoArray->at(k);
				String typeName = "unknown";
				if (visualInfo->type == 0) typeName = "static gray";
				else if (visualInfo->type == 1) typeName = "gray scale";
				else if (visualInfo->type == 2) typeName = "static color";
				else if (visualInfo->type == 3) typeName = "pseudo color";
				else if (visualInfo->type == 4) typeName = "true color";
				else if (visualInfo->type == 5) typeName = "direct color";
				print("  %% visual at depth %%:\n", typeName, visualInfoByDepth->at(j)->key());
				print("    visual ID: %% %%\n",
					visualInfo->visualId,
					(screenInfo->rootVisualId == visualInfo->visualId) ? "[ROOT]" : ""
				);
				print("    colormap size: %%\n", visualInfo->colormapSize);
				print("    bits per component: %%\n", visualInfo->bitsPerComponent);
				print("    color masks: red: 0x%hex:8.:'0'%, green: 0x%hex:8.:'0'%, blue: 0x%hex:8.:'0'%\n",
					visualInfo->redMask, visualInfo->greenMask, visualInfo->blueMask
				);
			}
		}
	}
	
	return 0;
}
