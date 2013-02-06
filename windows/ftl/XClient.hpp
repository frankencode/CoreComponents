 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XCLIENT_HPP
#define FTL_XCLIENT_HPP

#include <ftl/Exception.hpp>
#include <ftl/string.hpp>
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
	XDisplayInfo *displayInfo() const { return displayInfo_; }

	uint32_t allocateResourceId();
	void freeResourceId(uint32_t id);

	void activate(XMessageFilter *filter);
	void deactivate(XMessageFilter *filter);

	int createWindow(XWindow *window);
	int mapWindow(XWindow *window);
	int unmapWindow(XWindow *window);

	int getFontPath();

private:
	friend class Singleton<XClient>;
	XClient();
	~XClient();

	ByteEncoder *messageEncoder();
	int flush(ByteEncoder *sink);

	virtual void run();

	Ref<StreamSocket> socket_;
	Ref<XDisplayInfo> displayInfo_;
	int defaultScreen_;

	Ref<Mutex> resourceIdMutex_;
	uint32_t nextResourceId_;
	Ref< List<uint32_t> > freeResourceIds_;

	Ref<Mutex> sequenceNumberMutex_;
	uint16_t sequenceNumber_;

	thread_local_hook<ByteEncoder> messageEncoder_;

	Ref<Mutex> messageFiltersMutex_;
	typedef Set< Ref<XMessageFilter> > MessageFilters;
	Ref<MessageFilters> messageFilters_;
};

inline XClient *xClient() { return XClient::instance(); }

} // namespace ftl

#endif // FTL_XCLIENT_HPP
