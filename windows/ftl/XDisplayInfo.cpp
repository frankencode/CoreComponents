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

void XPixmapInfo::read(Ref<ByteDecoder> source)
{
	depth = source->readUInt8();
	bpp = source->readUInt8();
	pad = source->readUInt8();
	for (int j = 0; j < 5; ++j) source->readUInt8(); // unused
}

void XVisualInfo::read(Ref<ByteDecoder> source)
{
	visualId = source->readUInt32();
	type = source->readUInt8();
	bitsPerComponent = source->readUInt8();
	colormapSize = source->readUInt16();
	redMask = source->readUInt32();
	greenMask = source->readUInt32();
	blueMask = source->readUInt32();
	source->readInt32(); // unused
}

void XScreenInfo::read(Ref<ByteDecoder> source)
{
	rootWindowId = source->readUInt32();
	defaultColormapId = source->readUInt32();
	whitePixel = source->readUInt32();
	blackPixel = source->readUInt32();
	inputMask = source->readUInt32();
	widthInPixels = source->readUInt16();
	heightInPixels = source->readUInt16();
	widthInMillimeters = source->readUInt16();
	heightInMillimeters = source->readUInt16();
	minInstalledMaps = source->readUInt16();
	maxInstalledMaps = source->readUInt16();
	rootVisualId = source->readUInt32();
	backingStores = source->readUInt8();
	saveUnders = source->readUInt8();
	rootDepth = source->readUInt8();
	visualInfoByDepth = XVisualInfoByDepth::newInstance();
	for (int i = 0, n = source->readUInt8(); i < n; ++i) {
		int depth = source->readUInt8();
		source->readUInt8(); // unused
		Ref<XVisualInfoArray, Owner> visualInfoArray = XVisualInfoArray::newInstance(source->readUInt16());
		source->readUInt32(); // unused
		for (int j = 0; j < visualInfoArray->length(); ++j) {
			Ref<XVisualInfo, Owner> visualInfo = new XVisualInfo;
			visualInfo->read(source);
			visualInfoArray->set(j, visualInfo);
		}
		visualInfoByDepth->insert(depth, visualInfoArray);
	}
}

void XDisplayInfo::read(Ref<ByteDecoder> source)
{
	uint8_t response = source->readUInt8();
	uint8_t reasonLength = source->readUInt8();
	majorVersion = source->readUInt16();
	minorVersion = source->readUInt16();

	if (majorVersion != 11)
		FTL_THROW(XException, "Unsupported protocol version");

	source->readUInt16(); // additional data length

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
	screenInfo = XScreenInfoArray::newInstance(source->readUInt8());
	pixmapInfo = XPixmapInfoArray::newInstance(source->readUInt8());
	imageByteOrder = source->readUInt8();
	bitmapBitOrder = source->readUInt8();
	bitmapScanlineUnit = source->readUInt8();
	bitmapScanlinePad = source->readUInt8();
	minKeyCode = source->readUInt8();
	maxKeyCode = source->readUInt8();
	source->readUInt32(); // unused
	vendor = source->read(vendorLength);
	source->skipPad(4);

	for (int i = 0; i < pixmapInfo->length(); ++i) {
		Ref<XPixmapInfo, Owner> format = new XPixmapInfo;
		format->read(source);
		pixmapInfo->set(i, format);
	}

	for (int i = 0; i < screenInfo->length(); ++i) {
		Ref<XScreenInfo, Owner> screen = new XScreenInfo;
		screen->read(source);
		screenInfo->set(i, screen);
	}
}

} // namespace ftl
