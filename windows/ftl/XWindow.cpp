/*
 * XWindow.cpp -- X11 window
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <ftl/PrintDebug.hpp> // DEBUG
#include "XClient.hpp"
#include "XMessageFilter.hpp"
#include "XWindow.hpp"

namespace ftl
{

XWindow::XWindow(int x, int y, int width, int height)
	: id_(xClient()->allocateResourceId()),
	  visualId_(0),
	  depth_(0),
	  x_(x),
	  y_(y),
	  width_(width),
	  height_(height),
	  messageFilter_(new XMessageFilter)
{
	xClient()->createWindow(this);
	start();
}

XWindow::~XWindow()
{
	wait();
}

void XWindow::show()
{
	xClient()->mapWindow(this);
}

void XWindow::hide()
{
	xClient()->unmapWindow(this);
}

void XWindow::run()
{
	while (true) {
		Ref<XMessage, Owner> message = messageFilter_->pop();
		if (!message) break;
		if (message->messageCode == XMessage::ConfigureNotify) {
			Ref<XConfigureNotifyEvent> event = message;
			if (event->windowId != id_) continue;
			x_ = event->x;
			y_ = event->y;
			width_ = event->width;
			height_ = event->height;
			// printTo(error(), "XWindow::run(): x, y, width, height = %%, %%, %%, %%\n", x_, y_, width_, height_);
		}
	}
}

} // namespace ftl
