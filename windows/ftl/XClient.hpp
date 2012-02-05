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
#include <ftl/Thread.hpp>
#include "XDisplayInfo.hpp"
#include "XWindow.hpp"

namespace ftl
{

class StreamSocket;

FTL_EXCEPTION(XException, Exception);

class XClient: public Thread, public Singleton<XClient>
{
public:
	Ref<XDisplayInfo> displayInfo() const { return displayInfo_; }
	
	Ref<XWindow, Owner> createWindow(int x, int y, int width, int height);
	void mapWindow(Ref<XWindow> window);
	
private:
	friend class Singleton<XClient>;
	XClient();
	
	Ref<StreamSocket, Owner> socket_;
	Ref<XDisplayInfo, Owner> displayInfo_;
	int defaultScreen_;
	
	uint32_t allocateResourceId();
	void freeResourceId(uint32_t id);
	Mutex resourceIdMutex_;
	uint32_t nextResourceId_;
	Ref<List<uint32_t>, Owner> freeResourceIds_;
	
	uint16_t sequenceNumber_;
	virtual void run();
};

inline Ref<XClient> xClient() { return XClient::instance(); }

} // namespace ftl

#endif // FTL_XCLIENT_HPP
