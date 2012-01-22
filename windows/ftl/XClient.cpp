#include <ftl/Process.hpp>
#include <ftl/StreamSocket.hpp>
#include <ftl/ByteEncoder.hpp>
#include <ftl/ByteDecoder.hpp>
#include <ftl/Format.hpp>
#include <ftl/streams> // DEBUG
#include "XAuthFile.hpp"
#include "XClient.hpp"

namespace ftl
{

void readPadding(Ref<ByteDecoder> source) {
	while ((source->numBytesRead() & 3) != 0)
		source->readUInt8();
}

void writePadding(Ref<ByteEncoder> sink) {
	while ((sink->numBytesWritten() & 3) != 0)
		sink->writeUInt8(0);
}

XClient::XClient(String host, int display, int screen)
{
	Ref<SocketAddress, Owner> address;
	String authProtocol, authData;
	{
		Ref<StringList, Owner> parts = Process::env("DISPLAY").split(":");
		if (parts->length() == 2) {
			host = parts->get(0);
			parts = parts->get(1).split(".");
			display = parts->get(0).toInt();
			if (parts->length() == 2)
				screen = parts->get(1).toInt();
		}
		if (host == "") {
			String path = Format("/tmp/.X11-unix/X%%") << display;
			address = new SocketAddress(AF_LOCAL, path);
		}
		else {
			Ref<SocketAddressList, Owner> list = SocketAddress::resolve(host, "x11");
			if (list->length() == 0) FTL_THROW(XException, "Unknown host");
			address = list->get(0);
			
			if (host == "") host = SocketAddress::hostName();
			XAuthFile file;
			Ref<XAuthRecords> records = file.records();
			for (int i = 0, n = records->length(); i < n; ++i) {
				Ref<XAuthRecord> record = records->get(i);
				if ( (record->family == address->family()) &&
				     (record->host == host) &&
				     (record->display == display) )
				{
					authProtocol = record->protocol;
					authData = record->data;
					break;
				}
			}
		}
	}
	
	socket_ = new StreamSocket(address);
	socket_->connect();
	
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket_);
	
	sink->writeUInt8((sink->endian() == LittleEndian) ? 'l' : 'B');
	sink->writeUInt8(0); // unused
	sink->writeUInt16(11); // protocol-major-version
	sink->writeUInt16(0); // protocol-minor-version
	sink->writeUInt16(authProtocol->length());
	sink->writeUInt16(authData->length());
	sink->writeUInt16(0); // unused
	sink->write(authProtocol);
	writePadding(sink);
	sink->write(authData);
	writePadding(sink);
	sink->flush();
	
	Ref<ByteDecoder, Owner> source = new ByteDecoder(socket_);
	uint8_t response = source->readUInt8();
	uint8_t reasonLength = source->readUInt8();
	majorVersion_ = source->readUInt16();
	minorVersion_ = source->readUInt16();
	
	if (majorVersion_ != 11)
		FTL_THROW(XException, "Unsupported protocol version");
	
	uint16_t additionalDataLength = source->readUInt16();
	
	if (response != 1) {
		if ((response == 0) || (response == 2)) {
			String reason = source->read(reasonLength);
			FTL_THROW(XException, reason->constData());
		}
		FTL_THROW(XException, "Protocol error");
	}
	else if (response == 1) {
		releaseNumber_ = source->readUInt32();
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
		vendor_ = source->read(vendorLength);
		readPadding(source);
	}
}

} // namespace ftl
