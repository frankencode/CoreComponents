/*
 * XMessageLogger.hpp -- X11 message logger
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XMESSAGELOGGER_HPP
#define FTL_XMESSAGELOGGER_HPP

#include "XClient.hpp"

namespace ftl
{

class XMessageLogger: public Thread
{
public:
	XMessageLogger(int messageCode = -1, int detail = -1, int sequenceNumber = -1);
	
private:
	virtual void run();
	
	Ref<XMessageFilter, Owner> filter_;
};

} // namespace ftl

#endif // FTL_XMESSAGELOGGER_HPP
