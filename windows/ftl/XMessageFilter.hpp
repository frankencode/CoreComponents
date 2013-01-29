 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XMESSAGEFILTER_HPP
#define FTL_XMESSAGEFILTER_HPP

#include <ftl/Channel.hpp>
#include "XMessage.hpp"

namespace ftl
{

class XMessageFilter: public Channel< hook<XMessage> >
{
public:
	XMessageFilter();
	~XMessageFilter();

	virtual bool match(XMessage *event) { return true; }
};

} // namespace ftl

#endif // FTL_XMESSAGEFILTER_HPP
