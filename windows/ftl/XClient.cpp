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
#include <ftl/Mutex.hpp>
#include <ftl/ByteEncoder.hpp>
#include <ftl/ByteDecoder.hpp>
#include <ftl/Format.hpp>
#include <ftl/stdio>
#include "XAuthFile.hpp"
#include "XClient.hpp"

namespace ftl
{

XClient::XClient()
	: defaultScreen_(0),
	  resourceIdMutex_(new Mutex),
	  nextResourceId_(0),
	  freeResourceIds_(new List<uint32_t>),
	  sequenceNumberMutex_(new Mutex),
	  sequenceNumber_(0),
	  messageFiltersMutex_(new Mutex),
	  messageFilters_(new MessageFilters)
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

XClient::~XClient()
{
	messageFiltersMutex_ = 0;
	messageFilters_ = 0;
}

uint32_t XClient::allocateResourceId()
{
	Guard<Mutex> guard(resourceIdMutex_);
	if (freeResourceIds_->length() > 0)
		return freeResourceIds_->pop();
	if (nextResourceId_ == displayInfo_->resourceIdMask)
		FTL_THROW(XException, "Pool of resource IDs exhausted");
	return displayInfo_->resourceIdBase | ((++nextResourceId_) & displayInfo_->resourceIdMask);
}

void XClient::freeResourceId(uint32_t id)
{
	Guard<Mutex> guard(resourceIdMutex_);
	freeResourceIds_->push(id);
}

void XClient::activate(Ref<XMessageFilter> filter)
{
	Guard<Mutex> guard(messageFiltersMutex_);
	messageFilters_->insert(filter);
}

void XClient::deactivate(Ref<XMessageFilter> filter)
{
	if (!messageFiltersMutex_) return;
	Guard<Mutex> guard(messageFiltersMutex_);
	messageFilters_->remove(filter);
}

int XClient::createWindow(Ref<XWindow> window)
{
	Ref<XScreenInfo> screenInfo = displayInfo_->screenInfo->at(defaultScreen_);
	window->visualId_ = screenInfo->rootVisualId;
	window->depth_ = screenInfo->rootDepth;
	
	Ref<ByteEncoder, Owner> sink = messageEncoder();
	sink->writeUInt8(1);
	sink->writeUInt8(window->depth_);
	sink->writeUInt16(8 + 1); // request length
	sink->writeUInt32(window->id_);
	sink->writeUInt32(screenInfo->rootWindowId);
	sink->writeInt16(window->x_);
	sink->writeInt16(window->y_);
	sink->writeUInt16(window->width_);
	sink->writeUInt16(window->height_);
	sink->writeUInt16(0); // border width
	sink->writeUInt16(1); // class (0=inherit, 1=input/output, 2=input only)
	sink->writeUInt32(window->visualId_);
	sink->writeUInt32(0x800); // value mask (0x800 = event mask)
	sink->writeUInt32(XWindow::DefaultEventMask);
	return flush(sink);
}

int XClient::mapWindow(Ref<XWindow> window)
{
	Ref<ByteEncoder, Owner> sink = messageEncoder();
	sink->writeUInt8(8);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(2); // request length
	sink->writeUInt32(window->id_);
	return flush(sink);
}

int XClient::unmapWindow(Ref<XWindow> window)
{
	Ref<ByteEncoder, Owner> sink = messageEncoder();
	sink->writeUInt8(10);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(2); // request length
	sink->writeUInt32(window->id_);
	return flush(sink);
}

int XClient::getFontPath()
{
	Ref<ByteEncoder, Owner> sink = messageEncoder();
	sink->writeUInt8(52);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(1); // request list
	return flush(sink);
}

Ref<ByteEncoder> XClient::messageEncoder()
{
	if (!messageEncoder_)
		messageEncoder_ = new ByteEncoder(Ref<Stream>(socket_), int(displayInfo_->maximumRequestLength) * 4, localEndian());
	return messageEncoder_;
}

int XClient::flush(Ref<ByteEncoder> sink)
{
	Guard<Mutex> guard(sequenceNumberMutex_);
	sink->flush();
	return ++sequenceNumber_;
}

void XClient::run()
{
	printTo(error(), "XClient::run()\n");
	try {
		Ref<ByteDecoder, Owner> source = new ByteDecoder(socket_);
		
		while (true) {
			uint8_t messageCode = source->readUInt8();
			bool synthetic = messageCode & 0x80;
			messageCode &= 0x7F;
			
			Ref<XMessage, Owner> message;
			
			if (messageCode == XMessage::Error) {
				message = new XError(messageCode, synthetic, source);
			}
			else if (XMessage::inputEvent(messageCode)) {
				message = new XInputEvent(messageCode, synthetic, source);
			}
			else if (XMessage::focusEvent(messageCode)) {
				message = new XFocusEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::Expose) {
				source->readUInt8(); // unused
				message = new XExposeEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::GraphicsExposure) {
				source->readUInt8(); // unused
				message = new XGraphicsExposureEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::NoExposure) {
				source->readUInt8(); // unused
				message = new XNoExposureEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::VisibilityNotify) {
				source->readUInt8(); // unused
				message = new XVisibilityNotifyEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::ConfigureNotify) {
				source->readUInt8(); // unused
				message = new XConfigureNotifyEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::MapNotify) {
				source->readUInt8(); // unused
				message = new XMapNotifyEvent(messageCode, synthetic, source);
			}
			else if (messageCode == XMessage::UnmapNotify) {
				source->readUInt8(); // unused
				message = new XUnmapNotifyEvent(messageCode, synthetic, source);
			}
			else {
				message = new XMessage(messageCode, synthetic);
			}
			source->skipPad(32);
			{
				Guard<Mutex> guard(messageFiltersMutex_);
				for (int i = 0, n = messageFilters_->length(); i < n; ++i) {
					Ref<XMessageFilter> filter = messageFilters_->at(i);
					if (filter->match(message)) filter->push(message);
				}
			}
		}
	}
	catch (Exception& exception) {
		// printTo(error(), "%%\n", exception.what());
	}
	{
		Guard<Mutex> guard(messageFiltersMutex_);
		for (int i = 0, n = messageFilters_->length(); i < n; ++i)
			messageFilters_->at(i)->push(Ref<XMessage, Owner>());
	}
}

} // namespace ftl
