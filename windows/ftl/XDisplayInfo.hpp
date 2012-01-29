/*
 * XDisplayInfo.hpp -- X11 protocol connection information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XDISPLAYINFO_HPP
#define FTL_XDISPLAYINFO_HPP

#include <ftl/types.hpp>
#include <ftl/String.hpp>
#include <ftl/Array.hpp>
#include <ftl/Map.hpp>
#include <ftl/ByteDecoder.hpp>

namespace ftl
{

class XPixmapFormatInfo: public Instance {
public:
	uint8_t depth;
	uint8_t bpp;
	uint8_t padding;
	
	void read(Ref<ByteDecoder> source);
};

class XVisualInfo: public Instance {
public:
	uint32_t visualId;
	uint8_t type;
	uint8_t bitsPerComponent;
	uint16_t colorMapSize;
	uint32_t redMask;
	uint32_t greenMask;
	uint32_t blueMask;
	
	void read(Ref<ByteDecoder> source);
};

class XScreenInfo: public Instance {
public:
	uint32_t rootWindowId;
	uint32_t colorMap;
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
	
	typedef Array< Ref< XVisualInfo, Owner > > Visuals;
	typedef Map< int, Ref<Visuals, Owner> > VisualsByDepth;
	
	Ref<VisualsByDepth, Owner> visualsByDepth;
	
	void read(Ref<ByteDecoder> source);
};

class XDisplayInfo: public Instance {
public:
	uint16_t majorVersion;
	uint16_t minorVersion;
	uint32_t releaseNumber;
	uint32_t resourceIdBase;
	uint32_t resourceIdMask;
	uint32_t motionBufferSize;
	uint16_t maximumRequestLength;
	uint8_t imageEndian;
	uint8_t bitmapBitOrder;
	uint8_t bitmapScanlineUnit;
	uint8_t bitmapScanlinePad;
	uint8_t minKeyCode;
	uint8_t maxKeyCode;
	String vendor;
	
	typedef Array< Ref<XPixmapFormatInfo, Owner> > PixmapFormats;
	typedef Array< Ref<XScreenInfo, Owner> > Screens;
	
	Ref<PixmapFormats, Owner> pixmapFormats;
	Ref<Screens, Owner> screens;
	
	void read(Ref<ByteDecoder> source);
};

} // namespace ftl

#endif // FTL_XDISPLAYINFO_HPP
