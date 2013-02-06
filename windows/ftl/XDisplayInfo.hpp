 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XDISPLAYINFO_HPP
#define FTL_XDISPLAYINFO_HPP

#include <ftl/types.hpp>
#include <ftl/string.hpp>
#include <ftl/Array.hpp>
#include <ftl/Map.hpp>
#include <ftl/ByteDecoder.hpp>

namespace ftl
{

class XPixmapInfo: public Instance {
public:
	uint8_t depth;
	uint8_t bpp;
	uint8_t pad;

	void read(ByteDecoder *source);
};

class XVisualInfo: public Instance {
public:
	uint32_t visualId;
	uint8_t type;
	uint8_t bitsPerComponent;
	uint16_t colormapSize;
	uint32_t redMask;
	uint32_t greenMask;
	uint32_t blueMask;

	void read(ByteDecoder *source);
};

typedef Array< Ref<XVisualInfo> > XVisualInfoArray;
typedef Map< int, Ref<XVisualInfoArray> > XVisualInfoByDepth;

class XScreenInfo: public Instance {
public:
	uint32_t rootWindowId;
	uint32_t defaultColormapId;
	uint32_t whitePixel;
	uint32_t blackPixel;
	uint32_t inputMask;
	uint16_t widthInPixels;
	uint16_t heightInPixels;
	uint16_t widthInMillimeters;
	uint16_t heightInMillimeters;
	uint16_t minInstalledMaps;
	uint16_t maxInstalledMaps;
	uint32_t rootVisualId;
	uint8_t backingStores;
	uint8_t saveUnders;
	uint8_t rootDepth;

	Ref<XVisualInfoByDepth> visualInfoByDepth;

	void read(ByteDecoder *source);
};

typedef Array< Ref<XPixmapInfo> > XPixmapInfoArray;
typedef Array< Ref<XScreenInfo> > XScreenInfoArray;

class XDisplayInfo: public Instance {
public:
	enum Endianess { LittleEndian = 0, BigEndian = 1 };

	uint16_t majorVersion;
	uint16_t minorVersion;
	uint32_t releaseNumber;
	uint32_t resourceIdBase;
	uint32_t resourceIdMask;
	uint32_t motionBufferSize;
	uint16_t maximumRequestLength;
	uint8_t imageByteOrder;
	uint8_t bitmapBitOrder;
	uint8_t bitmapScanlineUnit;
	uint8_t bitmapScanlinePad;
	uint8_t minKeyCode;
	uint8_t maxKeyCode;
	string vendor;

	Ref<XPixmapInfoArray> pixmapInfo;
	Ref<XScreenInfoArray> screenInfo;

	void read(ByteDecoder *source);
};

} // namespace ftl

#endif // FTL_XDISPLAYINFO_HPP
