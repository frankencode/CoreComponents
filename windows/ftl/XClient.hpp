/*
 * XClient.hpp -- X11 protocol client
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XCLIENT_HPP
#define FTL_XCLIENT_HPP

#include <ftl/Exception.hpp>
#include <ftl/String.hpp>
#include <ftl/Mutex.hpp>
#include "XDisplayInfo.hpp"

namespace ftl
{

class StreamSocket;

FTL_EXCEPTION(XException, Exception);

class XClient: public Instance
{
public:
	XClient(String host = "", int display = 0, int screen = 0);
	
	Ref<XDisplayInfo> displayInfo() const { return displayInfo_; }
	
private:
	Ref<StreamSocket, Owner> socket_;
	
	uint32_t allocateResourceId();
	void freeResourceId(uint32_t id);
	Mutex resourceIdMutex_;
	uint32_t nextResourceId_;
	Ref<List<uint32_t>, Owner> freeResourceIds_;
	
	Ref<XDisplayInfo, Owner> displayInfo_;
};

} // namespace ftl

#endif // FTL_XCLIENT_HPP
