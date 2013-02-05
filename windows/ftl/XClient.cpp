 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <ftl/Process.hpp>
#include <ftl/StreamSocket.hpp>
#include <ftl/Mutex.hpp>
#include <ftl/ByteEncoder.hpp>
#include <ftl/ByteDecoder.hpp>
#include <ftl/format.hpp>
#include <ftl/PrintDebug.hpp>
#include "XAuthFile.hpp"
#include "XClient.hpp"

namespace ftl
{

XClient::XClient()
	: defaultScreen_(0),
	  resourceIdMutex_(Mutex::create()),
	  nextResourceId_(0),
	  freeResourceIds_(List<uint32_t>::create()),
	  sequenceNumberMutex_(Mutex::create()),
	  sequenceNumber_(0),
	  messageFiltersMutex_(Mutex::create()),
	  messageFilters_(MessageFilters::create())
{
	string host = "";
	int display = 0;

	hook<SocketAddress> address;
	string authProtocol, authData;
	{
		hook<StringList> parts = Process::env("DISPLAY")->split(':');
		if (parts->length() == 2) {
			host = parts->get(0);
			parts = parts->get(1)->split('.');
			display = parts->get(0)->toInt();
			if (parts->length() == 2)
				defaultScreen_ = parts->get(1)->toInt();
		}
		if (host == "") {
			string path = format("/tmp/.X11-unix/X%%") << display;
			address = SocketAddress::create(AF_LOCAL, path);
		}
		else {
			hook<SocketAddressList> list = SocketAddress::resolve(host, "x11");
			if (list->length() == 0) FTL_THROW(XException, "Unknown host");
			address = list->get(0);

			if (host == "") host = SocketAddress::hostName();
			hook<XAuthFile> file = new XAuthFile;
			XAuthRecords *records = file->records();
			for (int i = 0, n = records->length(); i < n; ++i) {
				XAuthRecord *record = records->get(i);
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

	socket_ = StreamSocket::create(address);
	socket_->connect();

	hook<ByteEncoder> sink = ByteEncoder::open(socket_);

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

	hook<ByteDecoder> source = ByteDecoder::open(socket_);

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

void XClient::activate(XMessageFilter *filter)
{
	Guard<Mutex> guard(messageFiltersMutex_);
	messageFilters_->insert(filter);
}

void XClient::deactivate(XMessageFilter *filter)
{
	if (!messageFiltersMutex_) return;
	Guard<Mutex> guard(messageFiltersMutex_);
	messageFilters_->remove(filter);
}

int XClient::createWindow(XWindow *window)
{
	XScreenInfo *screenInfo = displayInfo_->screenInfo->at(defaultScreen_);
	window->visualId_ = screenInfo->rootVisualId;
	window->depth_ = screenInfo->rootDepth;

	hook<ByteEncoder> sink = messageEncoder();
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

int XClient::mapWindow(XWindow *window)
{
	hook<ByteEncoder> sink = messageEncoder();
	sink->writeUInt8(8);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(2); // request length
	sink->writeUInt32(window->id_);
	return flush(sink);
}

int XClient::unmapWindow(XWindow *window)
{
	hook<ByteEncoder> sink = messageEncoder();
	sink->writeUInt8(10);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(2); // request length
	sink->writeUInt32(window->id_);
	return flush(sink);
}

int XClient::getFontPath()
{
	hook<ByteEncoder> sink = messageEncoder();
	sink->writeUInt8(52);
	sink->writeUInt8(0); // unused
	sink->writeUInt16(1); // request list
	return flush(sink);
}

ByteEncoder *XClient::messageEncoder()
{
	if (!messageEncoder_)
		messageEncoder_ = ByteEncoder::open(socket_, int(displayInfo_->maximumRequestLength) * 4, localEndian());
	return messageEncoder_;
}

int XClient::flush(ByteEncoder *sink)
{
	Guard<Mutex> guard(sequenceNumberMutex_);
	sink->flush();
	return ++sequenceNumber_;
}

void XClient::run()
{
	printTo(error(), "XClient::run()\n");
	try {
		hook<ByteDecoder> source = ByteDecoder::open(socket_);

		while (true) {
			uint8_t messageCode = source->readUInt8();
			bool synthetic = messageCode & 0x80;
			messageCode &= 0x7F;

			hook<XMessage> message;

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
					XMessageFilter *filter = messageFilters_->at(i);
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
			messageFilters_->at(i)->push(null<XMessage>());
	}
}

} // namespace ftl
