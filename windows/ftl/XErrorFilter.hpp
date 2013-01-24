 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XERRORFILTER_HPP
#define FTL_XERRORFILTER_HPP

#include "XMessageFilter.hpp"

namespace ftl
{

class XErrorFilter: public XMessageFilter
{
public:
	inline bool match(XMessage *message) const { return message->code == XMessage::Error; }
};

} // namespace ftl

#endif // FTL_XERRORFILTER_HPP
