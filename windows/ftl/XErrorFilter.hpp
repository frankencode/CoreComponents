/*
 * XErrorFilter.hpp -- X11 error event filter
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XERRORFILTER_HPP
#define FTL_XERRORFILTER_HPP

#include "XMessageFilter.hpp"

namespace ftl
{

class XErrorFilter: public XMessageFilter
{
public:
	inline bool match(Ref<XMessage> message) const { return message->code == XMessage::Error; }
};

} // namespace ftl

#endif // FTL_XERRORFILTER_HPP
