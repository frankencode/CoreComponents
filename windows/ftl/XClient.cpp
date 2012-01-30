/*
 * XClient.cpp -- X11 protocol client
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <ftl/Process.hpp>
#include <ftl/StreamSocket.hpp>
#include <ftl/ByteEncoder.hpp>
#include <ftl/ByteDecoder.hpp>
#include <ftl/Format.hpp>
#include <ftl/stdio> // DEBUG
#include "XAuthFile.hpp"
#include "XClient.hpp"

namespace ftl
{

XClient::XClient()
	: nextResourceId_(0),
	  freeResourceIds_(new List<uint32_t>),
	  defaultScreen_(0)
{
	String host = 0;
	int display = 0;
	
	Ref<SocketAddress, Owner> address;
	String authProtocol, authData;
	{
		Ref<StringList, Owner> parts = Process::env("DISPLAY")->split(':');
		if (parts->length() == 2) {
			host = parts->get(0);
			parts = parts->get(1)->split('.');
			display = parts->get(0)->toInt();
			if (parts->length() == 2)
				defaultScreen_ = parts->get(1)->toInt();
		}
		if (host == "") {
			String path = Format("/tmp/.X11-unix/X%%") << display;
			address = new SocketAddress(AF_LOCAL, path);
		}
		else {
			Ref<SocketAddressList, Owner> list = SocketAddress::resolve(host, "x11");
			if (list->length() == 0) FTL_THROW(XException, "Unknown host");
			address = list->get(0);
			
			if (host == "") host = SocketAddress::hostName();
			XAuthFile file;
			Ref<XAuthRecords> records = file.records();
			for (int i = 0, n = records->length(); i < n; ++i) {
				Ref<XAuthRecord> record = records->get(i);
				if ( (record->family == address->family()) &&
				     (record->host == host) &&
				     (record->display == display) )
				{
					authProtocol = record->protocol;
					authData = record->data;
					break;
				}
			}
		}
	}
	
	socket_ = new StreamSocket(address);
	socket_->connect();
	
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket_);
	
	sink->writeUInt8((sink->endian() == LittleEndian) ? 'l' : 'B');
	sink->writeUInt8(0); // unused
	sink->writeUInt16(11); // protocol-major-version
	sink->writeUInt16(0); // protocol-minor-version
	sink->writeUInt16(authProtocol->length());
	sink->writeUInt16(authData->length());
	sink->writeUInt16(0); // unused
	sink->write(authProtocol);
	sink->writePad(4);
	sink->write(authData);
	sink->writePad(4);
	sink->flush();
	
	Ref<ByteDecoder, Owner> source = new ByteDecoder(socket_);
	
	displayInfo_ = new XDisplayInfo;
	displayInfo_->read(source);
	
	if (defaultScreen_ >= displayInfo_->screenInfo->length())
		defaultScreen_ = displayInfo_->screenInfo->length() - 1;
}

Ref<XWindow, Owner> XClient::createWindow(int x, int y, int width, int height)
{
	Ref<XScreenInfo> screenInfo = displayInfo_->screenInfo->at(defaultScreen_);
	Ref<XWindow, Owner> window = new XWindow;
	window->id_ = allocateResourceId();
	window->visualId_ = screenInfo->rootVisualId;
	window->depth_ = screenInfo->rootDepth;
	
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket_);
	sink->writeUInt8(1);
	sink->writeUInt8(window->depth_);
	sink->writeUInt16(8); // request length
	sink->writeUInt32(window->id_);
	sink->writeUInt32(screenInfo->rootWindowId);
	sink->writeUInt16(x);
	sink->writeUInt16(y);
	sink->writeUInt16(width);
	sink->writeUInt16(height);
	sink->writeUInt16(0); // border width
	sink->writeUInt16(1); // class (0=inherit, 1=input/output, 2=input only)
	sink->writeUInt32(window->visualId_);
	sink->writeUInt32(0); // value mask
	
	sink->flush();
	
	return window;
}

void XClient::mapWindow(Ref<XWindow> window)
{
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket_);
	sink->writeUInt8(8);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(2); // request length
	sink->writeUInt32(window->id_);
	sink->flush();
}

uint32_t XClient::allocateResourceId()
{
	Guard<Mutex> guard(&resourceIdMutex_);
	if (freeResourceIds_->length() > 0)
		return freeResourceIds_->pop();
	if (nextResourceId_ == displayInfo_->resourceIdMask)
		FTL_THROW(XException, "Pool of resource IDs exhausted");
	return displayInfo_->resourceIdBase | ((++nextResourceId_) & displayInfo_->resourceIdMask);
}

void XClient::freeResourceId(uint32_t id)
{
	Guard<Mutex> guard(&resourceIdMutex_);
	freeResourceIds_->push(id);
}

} // namespace ftl
