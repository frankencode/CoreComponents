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
#include <ftl/Thread.hpp>
#include <ftl/Set.hpp>
#include "XDisplayInfo.hpp"
#include "XMessageFilter.hpp"
#include "XWindow.hpp"

namespace ftl
{

FTL_EXCEPTION(XException, Exception);

class StreamSocket;
class Mutex;
class ByteEncoder;

class XClient: public Thread, public Singleton<XClient>
{
public:
	Ref<XDisplayInfo> displayInfo() const { return displayInfo_; }
	
	uint32_t allocateResourceId();
	void freeResourceId(uint32_t id);
	
	void activate(Ref<XMessageFilter> filter);
	void deactivate(Ref<XMessageFilter> filter);
	
	int createWindow(Ref<XWindow> window);
	int mapWindow(Ref<XWindow> window);
	int unmapWindow(Ref<XWindow> window);
	
	int getFontPath();
	
private:
	friend class Singleton<XClient>;
	XClient();
	~XClient();
	
	Ref<ByteEncoder> messageEncoder();
	int flush(Ref<ByteEncoder> sink);
	
	virtual void run();
	
	Ref<StreamSocket, Owner> socket_;
	Ref<XDisplayInfo, Owner> displayInfo_;
	int defaultScreen_;
	
	Ref<Mutex, Owner> resourceIdMutex_;
	uint32_t nextResourceId_;
	Ref<List<uint32_t>, Owner> freeResourceIds_;
	
	Ref<Mutex, Owner> sequenceNumberMutex_;
	uint16_t sequenceNumber_;
	
	Ref<ByteEncoder, ThreadLocalOwner> messageEncoder_;
	
	Ref<Mutex, Owner> messageFiltersMutex_;
	typedef Set< Ref<XMessageFilter, Owner> > MessageFilters;
	Ref<MessageFilters, Owner> messageFilters_;
};

inline Ref<XClient> xClient() { return XClient::instance(); }

} // namespace ftl

#endif // FTL_XCLIENT_HPP
