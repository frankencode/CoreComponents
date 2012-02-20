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

#include "XClient.hpp"
#include "XWindow.hpp"

namespace ftl
{

XWindow::XWindow(int x, int y, int width, int height, uint32_t eventMask)
	: id_(xClient()->allocateResourceId()),
	  visualId_(0),
	  depth_(0),
	  x_(x),
	  y_(y),
	  width_(width),
	  height_(height),
	  eventMask_(eventMask)
{}

} // namespace ftl
