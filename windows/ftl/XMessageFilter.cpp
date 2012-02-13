/*
 * XMessageFilter.cpp -- X11 message filter
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "XClient.hpp"
#include "XMessageFilter.hpp"

namespace ftl
{

XMessageFilter::XMessageFilter()
{
	xClient()->activate(this);
}

XMessageFilter::~XMessageFilter()
{
	xClient()->deactivate(this);
}

} // namespace ftl
