/*
 * XMessageLogger.cpp -- X11 event logger
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <ftl/stdio>
#include "XMessageFilter.hpp"
#include "XMessageLogger.hpp"

namespace ftl
{

XMessageLogger::XMessageLogger()
	: messageFilter_(new XMessageFilter)
{}

void XMessageLogger::run()
{
	while (true) {
		Ref<XMessage, Owner> event = messageFilter_->pop();
		if (!event) break;
		event->printTo(error());
	}
}

} // namespace ftl
