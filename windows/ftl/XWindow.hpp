/*
 * XWindow.hpp -- X11 window
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XWINDOW_HPP
#define FTL_XWINDOW_HPP

#include <ftl/types.hpp>

namespace ftl
{

class XClient;

class XWindow: public Instance
{
private:
	friend class XClient;
	uint32_t id_;
	uint32_t visualId_;
	int depth_;
};

} // namespace ftl

#endif // FTL_XWINDOW_HPP
