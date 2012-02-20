/*
 * XMessage.hpp -- X11 messages
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XMESSAGE_HPP
#define FTL_XMESSAGE_HPP

#include <ftl/Instance.hpp>
#include <ftl/types.hpp>

namespace ftl
{

class ByteDecoder;

class XMessage: public Instance
{
public:
	enum MessageCode {
		Error            = 0,
		Reply            = 1,
		KeyPress         = 2,
		KeyRelease       = 3,
		ButtonPress      = 4,
		ButtonRelease    = 5,
		MotionNotify     = 6,
		EnterNotify      = 7,
		LeaveNotify      = 8,
		FocusIn          = 9,
		FocusOut         = 10,
		KeymapNotify     = 11,
		Expose           = 12,
		GraphicsExposure = 13,
		NoExposure       = 14,
		VisibilityNotify = 15,
		CreateNotify     = 16,
		DestroyNotify    = 17,
		UnmapNotify      = 18,
		MapNotify        = 19,
		MapRequest       = 20,
		ReparentNotify   = 21,
		ConfigureNotify  = 22,
		ConfigureRequest = 23,
		GravityNotify    = 24,
		ResizeRequest    = 25,
		CirculateNotify  = 26,
		CirculateRequest = 27,
		PropertyNotify   = 28,
		SelectionClear   = 29,
		SelectionRequest = 30,
		SelectionNotify  = 31,
		ColormapNotify   = 32,
		ClientMessage    = 33,
		MappingNotify    = 34
	};

	XMessage(uint8_t messageCode_, bool synthetic_)
		: messageCode(messageCode_), synthetic(synthetic_)
	{}

	static bool inputEvent(uint8_t messageCode) { return (KeyPress <= messageCode) && (messageCode <= LeaveNotify); }
	static bool focusEvent(uint8_t messageCode) { return (FocusIn <= messageCode) && (messageCode <= FocusOut); }
	inline bool inputEvent() { return inputEvent(messageCode); }
	inline bool focusEvent() { return focusEvent(messageCode); }
	
	uint8_t messageCode;
	bool synthetic;
};

class XError: public XMessage
{
public:
	enum ErrorCode {
		Request        = 1,
		Value          = 2,
		Window         = 3,
		Pixmap         = 4,
		Atom           = 5,
		Cursor         = 6,
		Font           = 7,
		Match          = 8,
		Drawable       = 9,
		Access         = 10,
		Alloc          = 11,
		Colormap       = 12,
		GContext       = 13,
		IdChoice       = 14,
		Name           = 15,
		Length         = 16,
		Implementation = 17
	};
	
	XError(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);
	
	inline int valueError() const { return errorCode == Value; }
	inline int resourceError() {
		return
			((Window <= errorCode) && (errorCode <= Font)) ||
			(errorCode == Drawable) ||
			(errorCode == Colormap) ||
			(errorCode == GContext) ||
			(errorCode == IdChoice);
	}
	
	uint8_t errorCode;
	uint16_t sequenceNumber;
	union {
		uint32_t badValue;
		uint32_t badResourceId;
		uint32_t badSomething;
	};
	uint16_t minorOpcode;
	uint8_t majorOpcode;
};

class XInputEvent: public XMessage
{
public:
	enum KeyButtonFlag {
		Shift   = 1,
		Lock    = 2,
		Control = 4,
		Mod1    = 8,
		Mod2    = 16,
		Mod3    = 32,
		Mod4    = 64,
		Mod5    = 128,
		Button1 = 256,
		Button2 = 512,
		Button3 = 1024,
		Button4 = 2048,
		Button5 = 4096
	};

	enum MotionDetail {
		Hint = 1
	};

	enum EnterOrLeaveDetail {
		Ancestor         = 0,
		Virtual          = 1,
		Inferior         = 2,
		Nonlinear        = 3,
		NonlinearVirtual = 4
	};

	enum EnterOrLeaveMode {
		Grab   = 1,
		Ungrab = 2
	};

	XInputEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);

	union {
		uint8_t keyCode;
		uint8_t button;
		uint8_t detail;
	};
	uint16_t sequenceNumber;
	uint32_t time;
	uint32_t rootWindowId;
	uint32_t eventWindowId;
	uint32_t childWindowId;
	uint16_t rootX, rootY;
	uint16_t eventX, eventY;
	uint16_t state;
	uint8_t mode;
	bool sameScreen;
	bool focus;
};

class XFocusEvent: public XMessage
{
public:
	enum Detail {
		Ancestor         = 0,
		Virtual          = 1,
		Inferior         = 2,
		Nonlinear        = 3,
		NonlinearVirtual = 4,
		Pointer          = 5,
		PointerRoot      = 6,
		NoDetail         = 7
	};
	enum Mode {
		Grab         = 1,
		Ungrab       = 2,
		WhileGrabbed = 3
	};

	XFocusEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);

	uint8_t detail;
	uint16_t sequenceNumber;
	uint32_t eventWindowId;
	uint8_t mode;
};

class XExposeEvent: public XMessage
{
public:
	XExposeEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);

	uint16_t sequenceNumber;
	uint32_t windowId;
	uint16_t x, y, width, height;
	uint16_t count;
};

class XGraphicsExposureEvent: public XMessage
{
public:
	XGraphicsExposureEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);

	uint16_t sequenceNumber;
	uint32_t drawableId;
	uint16_t x, y, width, height;
	uint16_t minorOpcode;
	uint16_t count;
	uint8_t majorOpcode;
};

class XNoExposureEvent: public XMessage
{
public:
	XNoExposureEvent(uint8_t messageCode, bool synthetic, Ref<ByteDecoder> source);

	uint16_t sequenceNumber;
	uint32_t drawableId;
	uint16_t minorOpcode;
	uint8_t majorOpcode;
};

} // namespace ftl

#endif // FTL_XMESSAGE_HPP
