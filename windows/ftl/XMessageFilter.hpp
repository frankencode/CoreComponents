/*
 * XMessageFilter.hpp -- X11 event filter
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XMESSAGEFILTER_HPP
#define FTL_XMESSAGEFILTER_HPP

#include <ftl/Channel.hpp>
#include "XMessage.hpp"

namespace ftl
{

class XMessageFilter: public Channel< Ref<XMessage, Owner> >
{
public:
	XMessageFilter();
	~XMessageFilter();

	virtual bool match(Ref<XMessage> event) { return true; }
};

} // namespace ftl

#endif // FTL_XMESSAGEFILTER_HPP
