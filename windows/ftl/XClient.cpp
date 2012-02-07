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
#include <ftl/stdio>
#include "XAuthFile.hpp"
#include "XClient.hpp"

namespace ftl
{

XClient::XClient()
	: defaultScreen_(0),
	  nextResourceId_(0),
	  freeResourceIds_(new List<uint32_t>),
	  sequenceNumber_(0)
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
	++sequenceNumber_;
	
	Ref<XScreenInfo> screenInfo = displayInfo_->screenInfo->at(defaultScreen_);
	Ref<XWindow, Owner> window = new XWindow;
	window->id_ = allocateResourceId();
	window->visualId_ = screenInfo->rootVisualId;
	window->depth_ = screenInfo->rootDepth;
	
	Ref<ByteEncoder, Owner> sink = new ByteEncoder(socket_);
	sink->writeUInt8(1);
	sink->writeUInt8(window->depth_);
	sink->writeUInt16(8 + 1); // request length
	sink->writeUInt32(window->id_);
	sink->writeUInt32(screenInfo->rootWindowId);
	sink->writeUInt16(x);
	sink->writeUInt16(y);
	sink->writeUInt16(width);
	sink->writeUInt16(height);
	sink->writeUInt16(0); // border width
	sink->writeUInt16(1); // class (0=inherit, 1=input/output, 2=input only)
	sink->writeUInt32(window->visualId_);
	sink->writeUInt32(0x800); // value mask (0x800 = event mask)
	sink->writeUInt32(0x1FFFFFF); // event mask
	sink->flush();
	
	return window;
}

void XClient::mapWindow(Ref<XWindow> window)
{
	++sequenceNumber_;
	
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

static const char *x11MessageName(int messageCode)
{
	const char *names[] = {
		"error",             // 0
		"display info",      // 1
		"key press",         // 2
		"key release",       // 3
		"button press",      // 4
		"button release",    // 5
		"motion notify",     // 6
		"enter notify",      // 7
		"leave notify",      // 8
		"focus in",          // 9
		"focus out",         // 10
		"keymap notify",     // 11
		"expose",            // 12
		"graphics exposure", // 13
		"no exposure",       // 14
		"visibility notify", // 15
		"create notify",     // 16
		"destroy notify",    // 17
		"unmap notify",      // 18
		"map notify",        // 19
		"map request",       // 20
		"reparent notify",   // 21
		"configure notify",  // 22
		"configure request", // 23
		"gravity notify",    // 24
		"resize request",    // 25
		"circulate notify",  // 26
		"circulate request", // 27
		"property notify",   // 28
		"selection clear",   // 29
		"selection request", // 30
		"selection notify",  // 31
		"colormap notify",   // 32
		"client message",    // 33
		"mapping notify"     // 34
	};
	
	return ((0 <= messageCode) && (messageCode <= 34)) ? names[messageCode] : "unknown";
}


void XClient::run()
{
	printTo(error(), "XClient::run()\n");
	Ref<ByteDecoder, Owner> source = new ByteDecoder(socket_);
	
	while (true) {
		uint8_t messageCode = source->readUInt8();
		bool synthetic = messageCode & 0x80;
		messageCode &= 0x7F;
		
		if (messageCode == 0) // error
		{
			uint8_t code = source->readUInt8();
			uint16_t sequenceNumber = source->readUInt16();
			uint32_t badSomething = source->readUInt32();
			uint16_t minorOpcode = source->readUInt16();
			uint8_t majorOpcode = source->readUInt8();
			source->skipPad(32);
			
			bool badValue = (code == 2);
			bool badResource =
				((3 <= code) && (code <= 7)) ||
				(code == 9) ||
				(code == 12) ||
				(code == 13) ||
				(code == 14);
			
			const char* errorNames[] = {
				"unknown", "request", "value", "window", "pixmap", "atom", "cursor", "font",
				"match", "drawable", "access", "alloc", "colormap", "graphics context",
				"id choice", "name", "length", "implementation"
			};
			const char* errorName = errorNames[((1 <= code) && (code <= 17)) ? int(code) : 0];
			
			if (badValue) {
				printTo(error(), "X11 %% error (opcode: %%.%%, bad value: %%) [%%]\n",
					errorName, majorOpcode, minorOpcode, badSomething, sequenceNumber
				);
			}
			else if (badResource) {
				printTo(error(), "X11 %% error (opcode: %%.%%, bad resource id: %%) [%%]\n",
					errorName, majorOpcode, minorOpcode, badSomething, sequenceNumber
				);
			}
			else {
				printTo(error(), "X11 %% error (opcode: %%.%%) [%%]\n",
					errorName, majorOpcode, minorOpcode, sequenceNumber
				);
			}
		}
		else if ((2 <= messageCode) && (messageCode <= 6))
		{
			uint8_t detail = source->readUInt8();
			uint16_t sequenceNumber = source->readUInt16();
			uint32_t time = source->readUInt32();
			uint32_t rootWindowId = source->readUInt32();
			uint32_t eventWindowId = source->readUInt32();
			uint32_t childWindowId = source->readUInt32();
			uint16_t rootX = source->readUInt16();
			uint16_t rootY = source->readUInt16();
			uint16_t eventX = source->readUInt16();
			uint16_t eventY = source->readUInt16();
			uint16_t state = source->readUInt16();
			uint8_t sameScreen = source->readUInt8();
			source->skipPad(32);
			
			String detailString;
			if ((messageCode == 2) || (messageCode == 3))
				detailString = Format("keycode: %%") << detail;
			else if ((messageCode == 4) || (messageCode == 5))
				detailString = Format("button: %%") << detail;
			else if (messageCode == 6)
				detailString = (detail == 0) ? "normal" : "hint";
			
			String stateString;
			{
				enum KeyButtonMask {
					Shift   = 1,
					Lock    = 2,
					Control = 4,
					Mod1    = 8,
					Mod2    = 16,
					Mod3    = 32,
					Mod4    = 64,
					Mod5    = 128,
					Button1 = 256,
					Button2 = 512,
					Button3 = 1024,
					Button4 = 2048,
					Button5 = 4096
				};
				Ref<StringList, Owner> bitNames = new StringList;
				if (state & Shift) bitNames->append("Shift");
				else if (state & Lock) bitNames->append("Lock");
				else if (state & Control) bitNames->append("Control");
				else if (state & Mod1) bitNames->append("Mod1");
				else if (state & Mod2) bitNames->append("Mod2");
				else if (state & Mod3) bitNames->append("Mod3");
				else if (state & Mod4) bitNames->append("Mod4");
				else if (state & Mod5) bitNames->append("Mod5");
				else if (state & Button1) bitNames->append("Button1");
				else if (state & Button2) bitNames->append("Button2");
				else if (state & Button3) bitNames->append("Button3");
				else if (state & Button4) bitNames->append("Button4");
				else if (state & Button5) bitNames->append("Button5");
				stateString = bitNames->join("|");
			}
			
			printTo(error(), "X11 %% event (\n"
				"  %%, time: %%,\n"
				"  root window id: %%,\n"
				"  event window id: %%,\n"
				"  child window id: %%,\n"
				"  rootX, rootY: %%, %%\n"
				"  eventX, eventY: %%, %%\n"
				"  same screen: %%\n"
				") [%%]\n",
				x11MessageName(messageCode),
				detailString, time,
				rootWindowId,
				eventWindowId,
				childWindowId,
				rootX, rootY,
				eventX, eventY,
				bool(sameScreen),
				sequenceNumber
			);
		}
		/*else if ((messageCode == 6) || (messageCode == 7))
		{
			uint8_t detail = source->readUInt8();
			uint16_t sequenceNumber = source->readUInt16();
			uint32_t time = source->readUInt32();
			uint32_t rootWindowId = source->readUInt32();
			uint32_t eventWindowId = source->readUInt32();
			uint32_t childWindowId = source->readUInt32();
			uint16_t rootX = source->readUInt16();
			uint16_t rootY = source->readUInt16();
			uint16_t eventX = source->readUInt16();
			uint16_t eventY = source->readUInt16();
			uint16_t state = source->readUInt16();
			uint8_t mode = source->readUInt8();
			uint8_t flags = source->readUInt8();
			
			String evenName = 
			String detailString 
		}*/
		else {
			printTo(error(), "X11 %% (%%)\n", x11MessageName(messageCode), messageCode);
		}
		source->skipPad(32);
	}
}

} // namespace ftl
