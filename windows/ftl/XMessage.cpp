 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <ftl/ByteDecoder.hpp>
#include <ftl/PrintDebug.hpp>
#include "XMessage.hpp"

namespace ftl
{

const char *XMessage::messageName() const
{
	const char *names[] = {
		"error",             // 0
		"reply",             // 1
		"key press",         // 2
		"key release",       // 3
		"button press",      // 4
		"button release",    // 5
		"motion notify",     // 6
		"enter notify",      // 7
		"leave notify",      // 8
		"focus in",          // 9
		"focus out",         // 10
		"keymap notify",     // 11
		"expose",            // 12
		"graphics exposure", // 13
		"no exposure",       // 14
		"visibility notify", // 15
		"create notify",     // 16
		"destroy notify",    // 17
		"unmap notify",      // 18
		"map notify",        // 19
		"map request",       // 20
		"reparent notify",   // 21
		"configure notify",  // 22
		"configure request", // 23
		"gravity notify",    // 24
		"resize request",    // 25
		"circulate notify",  // 26
		"circulate request", // 27
		"property notify",   // 28
		"selection clear",   // 29
		"selection request", // 30
		"selection notify",  // 31
		"colormap notify",   // 32
		"client event",    // 33
		"mapping notify"     // 34
	};

	return ((0 <= messageCode) && (messageCode <= 34)) ? names[messageCode] : "unknown";
}

void XMessage::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 %% (%%)\n", messageName(), messageCode);
}

XError::XError(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  errorCode(source->readUInt8()),
	  sequenceNumber(source->readUInt16()),
	  badSomething(source->readUInt32()),
	  minorOpcode(source->readUInt16()),
	  majorOpcode(source->readUInt8())
{}

void XError::printTo(LineSink *sink) const
{
	const char* errorNames[] = {
		"unknown", "request", "value", "window", "pixmap", "atom", "cursor", "font",
		"match", "drawable", "access", "alloc", "colormap", "graphics context",
		"id choice", "name", "length", "implementation"
	};
	const char* errorName = errorNames[((1 <= errorCode) && (errorCode <= 17)) ? int(errorCode) : 0];

	if (errorCode == 2) {
		ftl::printTo(sink, "X11 %% error (opcode: %%.%%, bad value: %%) [%%]\n",
			errorName, majorOpcode, minorOpcode, badSomething, sequenceNumber
		);
	}
	else if ( ((3 <= errorCode) && (errorCode <= 7)) ||
		      (errorCode == 9) || (errorCode == 12) || (errorCode == 13) || (errorCode == 14) ) {
		ftl::printTo(sink, "X11 %% error (opcode: %%.%%, bad resource id: %%) [%%]\n",
			errorName, majorOpcode, minorOpcode, badSomething, sequenceNumber
		);
	}
	else {
		ftl::printTo(sink, "X11 %% error (opcode: %%.%%) [%%]\n",
	      errorName, majorOpcode, minorOpcode, sequenceNumber
		);
	}
}


XInputEvent::XInputEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
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

void XInputEvent::printTo(LineSink *sink) const
{
	string detailString;
	if ((messageCode == XMessage::KeyPress) || (messageCode == XMessage::KeyRelease)) {
		detailString = Format("keycode: %%") << keyCode;
	}
	else if ((messageCode == XMessage::ButtonPress) || (messageCode == XMessage::ButtonRelease)) {
		detailString = Format("button: %%") << button;
	}
	else if (messageCode == XMessage::MotionNotify) {
		detailString = (detail == XInputEvent::Hint) ? "hint" : "normal";
	}
	else if ((messageCode == XMessage::EnterNotify) || (messageCode == XMessage::LeaveNotify)) {
		if      (detail == XInputEvent::Ancestor)         detailString = "ancestor";
		else if (detail == XInputEvent::Virtual)          detailString = "virtual";
		else if (detail == XInputEvent::Inferior)         detailString = "inferior";
		else if (detail == XInputEvent::Nonlinear)        detailString = "nonlinear";
		else if (detail == XInputEvent::NonlinearVirtual) detailString = "nonlinear virtual";
	}

	string stateString;
	{
		hook<StringList> bitNames = StringList::create();
		if      (state & XInputEvent::Shift)   bitNames->append("Shift");
		else if (state & XInputEvent::Lock)    bitNames->append("Lock");
		else if (state & XInputEvent::Control) bitNames->append("Control");
		else if (state & XInputEvent::Mod1)    bitNames->append("Mod1");
		else if (state & XInputEvent::Mod2)    bitNames->append("Mod2");
		else if (state & XInputEvent::Mod3)    bitNames->append("Mod3");
		else if (state & XInputEvent::Mod4)    bitNames->append("Mod4");
		else if (state & XInputEvent::Mod5)    bitNames->append("Mod5");
		else if (state & XInputEvent::Button1) bitNames->append("Button1");
		else if (state & XInputEvent::Button2) bitNames->append("Button2");
		else if (state & XInputEvent::Button3) bitNames->append("Button3");
		else if (state & XInputEvent::Button4) bitNames->append("Button4");
		else if (state & XInputEvent::Button5) bitNames->append("Button5");
		stateString = bitNames->join("|");
	}

	ftl::printTo(sink, "X11 %% event (\n"
		"  %%, time: %%,\n"
		"  root window id: %%,\n"
		"  event window id: %%,\n"
		"  child window id: %%,\n"
		"  rootX, rootY: %%, %%\n"
		"  eventX, eventY: %%, %%\n"
		"  same screen: %%\n"
		") [%%]\n",
		messageName(),
		detailString, time,
		rootWindowId,
		eventWindowId,
		childWindowId,
		rootX, rootY,
		eventX, eventY,
		sameScreen,
		sequenceNumber
	);
}

XFocusEvent::XFocusEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  detail(source->readUInt8()),
	  sequenceNumber(source->readUInt16()),
	  eventWindowId(source->readUInt32()),
	  mode(source->readUInt8())
{}

void XFocusEvent::printTo(LineSink *sink) const
{
	string detailString = "none";
	if      (detail == XFocusEvent::Ancestor)         detailString = "ancestor";
	else if (detail == XFocusEvent::Virtual)          detailString = "virtual";
	else if (detail == XFocusEvent::Inferior)         detailString = "inferior";
	else if (detail == XFocusEvent::Nonlinear)        detailString = "nonlinear";
	else if (detail == XFocusEvent::NonlinearVirtual) detailString = "nonlinear virtual";
	else if (detail == XFocusEvent::Pointer)          detailString = "pointer";
	else if (detail == XFocusEvent::PointerRoot)      detailString = "pointer root";
	string modeString = "normal";
	if      (mode == XFocusEvent::Grab)         modeString = "grab";
	else if (mode == XFocusEvent::Ungrab)       modeString = "ungrab";
	else if (mode == XFocusEvent::WhileGrabbed) modeString = "while grabbed";

	ftl::printTo(sink, "X11 %% event (\n"
		"  %%, %%\n"
		"  event window id: %%\n"
		") [%%]\n",
		messageName(),
		detailString, modeString,
		eventWindowId,
		sequenceNumber
	);
}

XExposeEvent::XExposeEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  windowId(source->readUInt32()),
	  x(source->readUInt16()),
	  y(source->readUInt16()),
	  width(source->readUInt16()),
	  height(source->readUInt16()),
	  count(source->readUInt16())
{}

void XExposeEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 expose event (\n"
		"  window id: %%\n"
		"  x, y: %%, %%\n"
		"  width, height: %%, %%\n"
		"  count: %%\n"
		") [%%]\n",
		windowId,
		x, y,
		width, height,
		count,
		sequenceNumber
	);
}

XGraphicsExposureEvent::XGraphicsExposureEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
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

void XGraphicsExposureEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 graphics exposure event (\n"
		"  drawable id: %%\n"
		"  x, y: %%, %%\n"
		"  width, height: %%, %%\n"
		"  opcode: %%.%%\n"
		"  count: %%\n"
		") [%%]\n",
		drawableId,
		x, y,
		width, height,
		majorOpcode, minorOpcode,
		count,
		sequenceNumber
	);
}

XNoExposureEvent::XNoExposureEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  drawableId(source->readUInt16()),
	  minorOpcode(source->readUInt16()),
	  majorOpcode(source->readUInt8())
{}

void XNoExposureEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 no exposure event (\n"
		"  drawable id: %%\n"
		"  opcode: %%.%%\n"
		") [%%]\n",
		drawableId,
		majorOpcode, minorOpcode,
		sequenceNumber
	);
}

XVisibilityNotifyEvent::XVisibilityNotifyEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  windowId(source->readUInt32()),
	  state(source->readUInt8())
{}

void XVisibilityNotifyEvent::printTo(LineSink *sink) const
{
	string stateString = "undefined";
	if (state == Unobscured) stateString = "unobscured";
	else if (state == PartiallyObscured) stateString = "partially obscured";
	else if (state == FullyObscured) stateString = "fully obscured";

	ftl::printTo(sink, "X11 visibility notify event (\n"
		"  window id: %%\n"
		"  state: %% (%%)\n"
		") [%%]\n",
		windowId,
		stateString, state,
		sequenceNumber
	);
}

XConfigureNotifyEvent::XConfigureNotifyEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  eventWindowId(source->readUInt32()),
	  windowId(source->readUInt32()),
	  aboveSiblingWindowId(source->readUInt32()),
	  x(source->readInt16()),
	  y(source->readInt16()),
	  width(source->readUInt16()),
	  height(source->readUInt16()),
	  borderWidth(source->readUInt16()),
	  overrideRedirect(source->readUInt8())
{}

void XConfigureNotifyEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 configure notify event (\n"
		"  event window id: %%\n"
		"  window id: %%\n"
		"  above sibling window id: %%\n"
		"  x, y: %%, %%\n"
		"  width, height: %%, %%\n"
		"  border width: %%\n"
		"  override redirect: %%\n"
		") [%%]\n",
		eventWindowId,
		windowId,
		aboveSiblingWindowId,
		x, y,
		width, height,
		borderWidth,
		overrideRedirect,
		sequenceNumber
	);
}

XMapNotifyEvent::XMapNotifyEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  eventWindowId(source->readUInt32()),
	  windowId(source->readUInt32()),
	  overrideRedirect(source->readUInt8())
{}

void XMapNotifyEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 map notify event (\n"
		"  eventWindowId: %%\n"
		"  windowId: %%\n"
		"  overrideRedirect: %%\n"
		") [%%]\n",
		eventWindowId,
		windowId,
		overrideRedirect,
		sequenceNumber
	);
}

XUnmapNotifyEvent::XUnmapNotifyEvent(uint8_t messageCode, bool synthetic, ByteDecoder *source)
	: XMessage(messageCode, synthetic),
	  sequenceNumber(source->readUInt16()),
	  eventWindowId(source->readUInt32()),
	  windowId(source->readUInt32()),
	  fromConfigure(source->readUInt8())
{}

void XUnmapNotifyEvent::printTo(LineSink *sink) const
{
	ftl::printTo(sink, "X11 unap notify event (\n"
		"  eventWindowId: %%\n"
		"  windowId: %%\n"
		"  fromConfigure: %%\n"
		") [%%]\n",
		eventWindowId,
		windowId,
		fromConfigure,
		sequenceNumber
	);
}

} // namespace ftl
