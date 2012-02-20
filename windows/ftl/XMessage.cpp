/*
 * XMessage.cpp -- X11 messages
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <ftl/ByteDecoder.hpp>
#include "XMessage.hpp"

namespace ftl
{

XError::XError(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  errorCode(source->readUInt8()),
	  sequenceNumber(source->readUInt16()),
	  badSomething(source->readUInt32()),
	  minorOpcode(source->readUInt16()),
	  majorOpcode(source->readUInt8())
{}

XInputEvent::XInputEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  detail(source->readUInt8()),
	  sequenceNumber(source->readUInt16()),
	  time(source->readUInt32()),
	  rootWindowId(source->readUInt32()),
	  eventWindowId(source->readUInt32()),
	  childWindowId(source->readUInt32()),
	  rootX(source->readUInt16()),
	  rootY(source->readUInt16()),
	  eventX(source->readUInt16()),
	  eventY(source->readUInt16()),
	  state(source->readUInt16())
{
	if ((XMessage::KeyPress <= messageCode) && (messageCode <= XMessage::MotionNotify)) {
		mode = 0;
		focus = false;
		sameScreen = source->readUInt8();
	}
	else {
		mode = source->readUInt8();
		uint8_t flag = source->readUInt8();
		focus = flag & 1;
		sameScreen = flag & 2;
	}
}

XFocusEvent::XFocusEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  detail(source->readUInt8()),
	  sequenceNumber(source->readUInt16()),
	  eventWindowId(source->readUInt32()),
	  mode(source->readUInt8())
{}

XExposeEvent::XExposeEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  windowId(source->readUInt32()),
	  x(source->readUInt16()),
	  y(source->readUInt16()),
	  width(source->readUInt16()),
	  height(source->readUInt16()),
	  count(source->readUInt16())
{}

XGraphicsExposureEvent::XGraphicsExposureEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  drawableId(source->readUInt32()),
	  x(source->readUInt16()),
	  y(source->readUInt16()),
	  width(source->readUInt16()),
	  height(source->readUInt16()),
	  minorOpcode(source->readUInt16()),
	  count(source->readUInt16()),
	  majorOpcode(source->readUInt8())
{}

XNoExposureEvent::XNoExposureEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  drawableId(source->readUInt16()),
	  minorOpcode(source->readUInt16()),
	  majorOpcode(source->readUInt8())
{}

} // namespace ftl
