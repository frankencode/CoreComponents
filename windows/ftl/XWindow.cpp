 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
		hook<XMessage> message = messageFilter_->pop();
		if (!message) break;
		if (message->messageCode == XMessage::ConfigureNotify) {
			XConfigureNotifyEvent *event = cast<XConfigureNotifyEvent>(message.get());
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
