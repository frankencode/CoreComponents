 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XWINDOW_HPP
#define FTL_XWINDOW_HPP

#include <ftl/types.hpp>
#include <ftl/Thread.hpp>

namespace ftl
{

class XClient;
class XMessageFilter;

class XWindow: public Thread
{
public:
	XWindow(int x, int y, int width, int height);
	~XWindow();

	inline int x() const { return x_; }
	inline int y() const { return y_; }
	inline int width() const { return width_; }
	inline int height() const { return height_; }

	void show();
	void hide();

private:
	friend class XClient;

	enum EventMask {
		KeyPress             = 0x00000001,
		KeyRelease           = 0x00000002,
		ButtonPress          = 0x00000004,
		ButtonRelease        = 0x00000008,
		EnterWindow          = 0x00000010,
		LeaveWindow          = 0x00000020,
		PointerMotion        = 0x00000040,
		PointerMotionHint    = 0x00000080,
		Button1Motion        = 0x00000100,
		Button2Motion        = 0x00000200,
		Button3Motion        = 0x00000400,
		Button4Motion        = 0x00000800,
		Button5Motion        = 0x00001000,
		ButtonMotion         = 0x00002000,
		KeymapState          = 0x00004000,
		Exposure             = 0x00008000,
		VisibilityChange     = 0x00010000,
		StructureNotify      = 0x00020000,
		ResizeRedirect       = 0x00040000,
		SubstructureNotify   = 0x00080000,
		SubstructureRedirect = 0x00100000,
		FocusChange          = 0x00200000,
		PropertyChange       = 0x00400000,
		ColormapChange       = 0x00800000,
		OwnerGrabButton      = 0x01000000,
		DefaultEventMask     =
			0x01FFFFFF & ~(
				KeymapState|
				ResizeRedirect|
				SubstructureNotify|
				SubstructureRedirect
			)
	};

	void run();

	uint32_t id_;
	uint32_t visualId_;
	int depth_;
	int x_, y_, width_, height_;

	hook<XMessageFilter> messageFilter_;
};

} // namespace ftl

#endif // FTL_XWINDOW_HPP
