 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <ftl/StandardStreams.hpp>
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
		Ref<XMessage> event = messageFilter_->pop();
		if (!event) break;
		event->printTo(error());
	}
}

} // namespace ftl
