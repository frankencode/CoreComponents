/*
 * XMessage.hpp -- X11 message
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

namespace ftl
{

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
	
	inline int code() const { return code_; }
	inline bool synthetic() const { return synthetic_; }

private:
	int code_;
	bool synthetic_;
};

} // namespace ftl

#endif // FTL_XMESSAGE_HPP
