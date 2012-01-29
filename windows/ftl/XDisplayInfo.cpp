/*
 * XDisplayInfo.cpp -- X11 protocol connection information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "XClient.hpp"
#include "XDisplayInfo.hpp"

namespace ftl
{

void XPixmapFormatInfo::read(Ref<ByteDecoder> source)
{
	depth = source->readUInt8();
	bpp = source->readUInt8();
	padding = source->readUInt8();
	for (int j = 0; j < 5; ++j) source->readUInt8(); // unused
}

void XDisplayInfo::read(Ref<ByteDecoder> source)
{
	uint8_t response = source->readUInt8();
	uint8_t reasonLength = source->readUInt8();
	majorVersion = source->readUInt16();
	minorVersion = source->readUInt16();
	
	if (majorVersion != 11)
		FTL_THROW(XException, "Unsupported protocol version");
	
	uint16_t additionalDataLength = source->readUInt16();
	
	if (response != 1) {
		if ((response == 0) || (response == 2)) {
			String reason = source->read(reasonLength);
			FTL_THROW(XException, reason->constData());
		}
		FTL_THROW(XException, "Protocol error");
	}
	
	releaseNumber = source->readUInt32();
	resourceIdBase = source->readUInt32();
	resourceIdMask = source->readUInt32();
	motionBufferSize = source->readUInt32();
	uint16_t vendorLength = source->readUInt16();
	maximumRequestLength = source->readUInt16();
	uint8_t numberOfRoots = source->readUInt8();
	pixmapFormats = new PixmapFormats(source->readUInt8());
	imageEndian = source->readUInt8();
	bitmapBitOrder = source->readUInt8();
	bitmapScanlineUnit = source->readUInt8();
	bitmapScanlinePad = source->readUInt8();
	minKeyCode = source->readUInt8();
	maxKeyCode = source->readUInt8();
	source->readUInt32(); // unused
	vendor = source->read(vendorLength);
	source->skipPad(4);
	
	for (int i = 0; i < pixmapFormats->length(); ++i) {
		Ref<XPixmapFormatInfo, Owner> format = new XPixmapFormatInfo;
		format->read(source);
		pixmapFormats->set(i, format);
	}
}

} // namespace ftl
