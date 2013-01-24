 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XMESSAGELOGGER_HPP
#define FTL_XMESSAGELOGGER_HPP

#include <ftl/Thread.hpp>

namespace ftl
{

class XMessageFilter;

class XMessageLogger: public Thread
{
public:
	XMessageLogger();

private:
	void run();
	XMessageFilter *messageFilter_;
};

} // namespace ftl

#endif // FTL_XMESSAGELOGGER_HPP
