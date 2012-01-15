#include <ftl/binary>
#include <ftl/streams>
#include <ftl/sockets>

namespace displayInfo
{

using namespace ftl;

void readPadding(Ref<ByteDecoder> source) {
	while ((source->numBytesRead() & 3) != 0)
		source->readUInt8();
}

void writePadding(Ref<ByteEncoder> sink) {
	while ((sink->numBytesWritten() & 3) != 0)
		sink->writeUInt8(0);
}

struct PixmapFormat {
	uint8_t depth_;
	uint8_t bitsPerPixel_;
	uint8_t scanlinePadding_;
};

struct Depth;
struct Visual;

struct Screen {
	uint32_t rootWindow_;
	uint32_t colorMap_;
	uint32_t whitePixel_;
	uint32_t blackPixel_;
	uint32_t inputMask_;
	uint16_t widthInPixels_;
	uint16_t heightInPixels_;
	uint16_t widthInMillimeters_;
	uint16_t heightInMillimeters_;
	uint16_t minInstalledMaps_;
	uint16_t maxInstalledMaps_;
	uint32_t rootVisual_;
	uint8_t backingStores_;
	uint8_t saveUnders_;
	uint8_t rootDepth_;
	uint8_t numberOfAllowedDepths_;
	Array<Depth> allowedDepths_;
};

struct Depth {
	uint8_t depth_;
	uint16_t numberOfVisuals_;
	Array<Visual> visuals_;
};

struct Visual {
	uint32_t identifier_;
	uint8_t type_;
	uint8_t bitsPerComponent_;
	uint16_t colorMapSize_;
	uint32_t redMask_;
	uint32_t greenMask_;
	uint32_t blueMask_;
};

int main() {
	Ref<SocketAddress, Owner> address = new SocketAddress(AF_LOCAL, "/tmp/.X11-unix/X0");
	Ref<StreamSocket, Owner> socket = new StreamSocket(address);
	socket->connect();
	
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket);
	sink->writeUInt8((sink->endian() == LittleEndian) ? 'l' : 'B');
	sink->writeUInt8(0); // unused
	sink->writeUInt16(11); // protocol-major-version
	sink->writeUInt16(0); // protocol-minor-version
	sink->writeUInt16(0); // length of authorization-protocol-name
	sink->writeUInt16(0); // length of authorization-protocol-data
	sink->writeUInt16(0); // unused
	sink->flush();
	
	print("connection setup, %% bytes written\n", sink->numBytesWritten());
	
	Ref<ByteDecoder, Owner> source = new ByteDecoder(socket);
	uint8_t response = source->readUInt8();
	
	print("response = %%\n", response);
	if (response != 1) return 1;
	
	source->readUInt8(); // unused
	uint16_t majorVersion = source->readUInt16();
	uint16_t minorVersion = source->readUInt16();
	uint16_t additionalDataLength = source->readUInt16();
	uint32_t releaseNumber = source->readUInt32();
	uint32_t resourceIdentifierBase = source->readUInt32();
	uint32_t resourceIdentifierMask = source->readUInt32();
	uint32_t motionBufferSize = source->readUInt32();
	uint16_t vendorLength = source->readUInt16();
	uint16_t maximumRequestLength = source->readUInt16();
	uint8_t numberOfRoots = source->readUInt8();
	uint8_t numberOfPixmapFormats = source->readUInt8();
	uint8_t imageByteOrder = source->readUInt8();
	uint8_t bitmapBitOrder = source->readUInt8();
	uint8_t bitmapScanlineUnit = source->readUInt8();
	uint8_t bitmapScanlinePad = source->readUInt8();
	uint8_t minKeyCode = source->readUInt8();
	uint8_t maxKeyCode = source->readUInt8();
	source->readUInt32(); // unused
	String vendor = String::uninitialized(vendorLength);
	for (unsigned i = 0, n = vendorLength; i < n; ++i)
		vendor->set(i, source->readUInt8());
	readPadding(source);
	Array<PixmapFormat> pixmapFormats(numberOfPixmapFormats);
	for (unsigned i = 0, n = numberOfPixmapFormats; i < n; ++i) {
		PixmapFormat *format = pixmapFormats.pointerAt(i);
		format->depth_ = source->readUInt8();
		format->bitsPerPixel_ = source->readUInt8();
		format->scanlinePadding_ = source->readUInt8();
		for (int j = 0; j < 5; ++j) source->readUInt8(); // unused
	}
	Array<Screen> roots(numberOfRoots);
	for (unsigned i = 0, n = numberOfRoots; i < n; ++i) {
		Screen *screen = roots.pointerAt(i);
		screen->rootWindow_ = source->readUInt32();
		screen->colorMap_ = source->readUInt32();
		screen->whitePixel_ = source->readUInt32();
		screen->blackPixel_ = source->readUInt32();
		screen->inputMask_ = source->readUInt32();
		screen->widthInPixels_ = source->readUInt16();
		screen->heightInPixels_ = source->readUInt16();
		screen->widthInMillimeters_ = source->readUInt16();
		screen->heightInMillimeters_ = source->readUInt16();
		screen->minInstalledMaps_ = source->readUInt16();
		screen->maxInstalledMaps_ = source->readUInt16();
		screen->rootVisual_ = source->readUInt32();
		screen->backingStores_ = source->readUInt8();
		screen->saveUnders_ = source->readUInt8();
		screen->rootDepth_ = source->readUInt8();
		screen->numberOfAllowedDepths_ = source->readUInt8();
		screen->allowedDepths_.reset(screen->numberOfAllowedDepths_);
		for (unsigned j = 0, m = screen->numberOfAllowedDepths_; j < m; ++j) {
			Depth *depth = screen->allowedDepths_.pointerAt(j);
			depth->depth_ = source->readUInt8();
			source->readUInt8(); // unused
			depth->numberOfVisuals_ = source->readUInt16();
			source->readUInt32(); // unused
			depth->visuals_.reset(depth->numberOfVisuals_);
			for (unsigned k = 0, u = depth->numberOfVisuals_; k < u; ++k) {
				Visual *visual = depth->visuals_.pointerAt(k);
				visual->identifier_ = source->readUInt32();
				visual->type_ = source->readUInt8();
				visual->bitsPerComponent_ = source->readUInt8();
				visual->colorMapSize_ = source->readUInt16();
				visual->redMask_ = source->readUInt32();
				visual->greenMask_ = source->readUInt32();
				visual->blueMask_ = source->readUInt32();
				source->readInt32(); // unused
			}
		}
	}
	
	print("protocol version = %%.%%\n", majorVersion, minorVersion);
	print("additional data length = %% bytes\n", int(additionalDataLength) * 4);
	print("release number = %%\n", releaseNumber);
	print("resource identifier base = %%\n", resourceIdentifierBase);
	print("resource identifier mask = %%\n", resourceIdentifierMask);
	print("motion buffer size = %%\n", motionBufferSize);
	print("vendor length = %%\n", vendorLength);
	print("maximum request length = %% bytes\n", int(maximumRequestLength) * 4);
	print("number of roots = %%\n", numberOfRoots);
	print("number of pixmap formats = %%\n", numberOfPixmapFormats);
	print("image byte order = %% (%%)\n", imageByteOrder, (imageByteOrder == 0) ? "little endian" : "big endian");
	print("bitmap bit order = %% (%%)\n", bitmapBitOrder, (bitmapBitOrder == 0) ? "little endian" : "big endian");
	print("bitmap scanline unit = %%\n", bitmapScanlineUnit);
	print("bitmap scanline pad = %%\n", bitmapScanlinePad);
	print("minimum key code = %%\n", minKeyCode);
	print("maximum key code = %%\n", maxKeyCode);
	print("vendor = \"%%\"\n", vendor);
	for (unsigned i = 0, n = numberOfPixmapFormats; i < n; ++i) {
		PixmapFormat *format = pixmapFormats.pointerAt(i);
		print("pixmap format: depth = %%, bits per pixel = %%, scanline padding = %%\n", format->depth_, format->bitsPerPixel_, format->scanlinePadding_);
	}
	for (unsigned i = 0, n = numberOfRoots; i < n; ++i) {
		Screen *screen = roots.pointerAt(i);
		print("screen %%:\n", i);
		print("  window = %%\n", screen->rootWindow_);
		print("  color map = %%\n", screen->colorMap_);
		print("  white pixel = %%\n", screen->whitePixel_);
		print("  black pixel = %%\n", screen->blackPixel_);
		print("  width in px = %%\n", screen->widthInPixels_);
		print("  height in px = %%\n", screen->heightInPixels_);
		print("  width in mm = %%\n", screen->widthInMillimeters_);
		print("  height in mm = %%\n", screen->heightInMillimeters_);
		print("  minimum installed maps = %%\n", screen->minInstalledMaps_);
		print("  maximum installed maps = %%\n", screen->maxInstalledMaps_);
		print("  root visual = %%\n", screen->rootVisual_);
		{
			String s = "?";
			if (screen->backingStores_ == 0) s = "never";
			else if (screen->backingStores_ == 1) s = "when mapped";
			else if (screen->backingStores_ == 2) s = "always";
			print("  backing stores = %% (%%)\n", screen->backingStores_, s);
		}
		print("  save unders = %%\n", screen->saveUnders_);
		print("  root depth = %%\n", screen->rootDepth_);
		print("  number of allowed depths = %%\n", screen->numberOfAllowedDepths_);
		for (unsigned j = 0, m = screen->numberOfAllowedDepths_; j < m; ++j) {
			Depth *depth = screen->allowedDepths_.pointerAt(j);
			print("  depth %%:\n", depth->depth_);
			for (unsigned k = 0, u = depth->numberOfVisuals_; k < u; ++k) {
				Visual *visual = depth->visuals_.pointerAt(k);
				print("    visual %%:\n", visual->identifier_);
				{
					String s = "?";
					if (visual->type_ == 0) s = "static gray";
					else if (visual->type_ == 1) s = "gray scale";
					else if (visual->type_ == 2) s = "static color";
					else if (visual->type_ == 3) s = "pseudo color";
					else if (visual->type_ == 4) s = "true color";
					else if (visual->type_ == 5) s = "direct color";
					print("      type = %% (%%)\n", visual->type_, s);
				}
				print("      bits per color component = %%\n", visual->bitsPerComponent_);
				print("      color map size = %%\n", visual->colorMapSize_);
				print("      red mask = 0x%hex%\n", visual->redMask_);
				print("      green mask = 0x%hex%\n", visual->greenMask_);
				print("      blue mask = 0x%hex%\n", visual->blueMask_);
			}
		}
	}
	
	return 0;
}

} // namespace displayInfo

int main() { return displayInfo::main(); }
