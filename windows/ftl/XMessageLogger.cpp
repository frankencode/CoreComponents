/*
 * XMessageLogger.cpp -- X11 message logger
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <ftl/stdio>
#include "XMessageFilter.hpp"
#include "XMessageLogger.hpp"

namespace ftl
{

static const char *x11MessageName(int messageCode)
{
	const char *names[] = {
		"error",             // 0
		"reply",             // 1
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

XMessageLogger::XMessageLogger()
	: messageFilter_(new XMessageFilter)
{}

void XMessageLogger::run()
{
	while (true) {
		Ref<XMessage, Owner> message = messageFilter_->pop();
		if (!message) break;
		
		if (message->inputEvent()) {
			Ref<XInputEvent> event = message;
			String detailString;
			if ((event->messageCode == XMessage::KeyPress) || (event->messageCode == XMessage::KeyRelease)) {
				detailString = Format("keycode: %%") << event->keyCode;
			}
			else if ((event->messageCode == XMessage::ButtonPress) || (event->messageCode == XMessage::ButtonRelease)) {
				detailString = Format("button: %%") << event->button;
			}
			else if (event->messageCode == XMessage::MotionNotify) {
				detailString = (event->detail == XInputEvent::Hint) ? "hint" : "normal";
			}
			else if ((event->messageCode == XMessage::EnterNotify) || (event->messageCode == XMessage::LeaveNotify)) {
				if      (event->detail == XInputEvent::Ancestor)         detailString = "ancestor";
				else if (event->detail == XInputEvent::Virtual)          detailString = "virtual";
				else if (event->detail == XInputEvent::Inferior)         detailString = "inferior";
				else if (event->detail == XInputEvent::Nonlinear)        detailString = "nonlinear";
				else if (event->detail == XInputEvent::NonlinearVirtual) detailString = "nonlinear virtual";
			}
			
			String stateString;
			{
				Ref<StringList, Owner> bitNames = new StringList;
				if      (event->state & XInputEvent::Shift)   bitNames->append("Shift");
				else if (event->state & XInputEvent::Lock)    bitNames->append("Lock");
				else if (event->state & XInputEvent::Control) bitNames->append("Control");
				else if (event->state & XInputEvent::Mod1)    bitNames->append("Mod1");
				else if (event->state & XInputEvent::Mod2)    bitNames->append("Mod2");
				else if (event->state & XInputEvent::Mod3)    bitNames->append("Mod3");
				else if (event->state & XInputEvent::Mod4)    bitNames->append("Mod4");
				else if (event->state & XInputEvent::Mod5)    bitNames->append("Mod5");
				else if (event->state & XInputEvent::Button1) bitNames->append("Button1");
				else if (event->state & XInputEvent::Button2) bitNames->append("Button2");
				else if (event->state & XInputEvent::Button3) bitNames->append("Button3");
				else if (event->state & XInputEvent::Button4) bitNames->append("Button4");
				else if (event->state & XInputEvent::Button5) bitNames->append("Button5");
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
				x11MessageName(event->messageCode),
				detailString, event->time,
				event->rootWindowId,
				event->eventWindowId,
				event->childWindowId,
				event->rootX, event->rootY,
				event->eventX, event->eventY,
				event->sameScreen,
				event->sequenceNumber
			);
		}
		else if (message->focusEvent()) {
			Ref<XFocusEvent> event = message;
			String detailString = "none";
			if      (event->detail == XFocusEvent::Ancestor)         detailString = "ancestor";
			else if (event->detail == XFocusEvent::Virtual)          detailString = "virtual";
			else if (event->detail == XFocusEvent::Inferior)         detailString = "inferior";
			else if (event->detail == XFocusEvent::Nonlinear)        detailString = "nonlinear";
			else if (event->detail == XFocusEvent::NonlinearVirtual) detailString = "nonlinear virtual";
			else if (event->detail == XFocusEvent::Pointer)          detailString = "pointer";
			else if (event->detail == XFocusEvent::PointerRoot)      detailString = "pointer root";
			String modeString = "normal";
			if      (event->mode == XFocusEvent::Grab)         modeString = "grab";
			else if (event->mode == XFocusEvent::Ungrab)       modeString = "ungrab";
			else if (event->mode == XFocusEvent::WhileGrabbed) modeString = "while grabbed";
			printTo(error(), "X11 %% event (\n"
				"  %%, %%\n"
				"  event window id: %%\n"
				") [%%]\n",
				x11MessageName(event->messageCode),
				detailString, modeString,
				event->eventWindowId,
				event->sequenceNumber
			);
		}
		else if (message->messageCode == XMessage::Expose) {
			Ref<XExposeEvent> event = message;
			printTo(error(), "X11 expose event (\n"
				"  window id: %%\n"
				"  x, y: %%, %%\n"
				"  width, height: %%, %%\n"
				"  count: %%\n"
				") [%%]\n",
				event->windowId,
				event->x, event->y,
				event->width, event->height,
				event->count,
				event->sequenceNumber
			);
		}
		else if (message->messageCode == XMessage::GraphicsExposure) {
			Ref<XGraphicsExposureEvent> event = message;
			printTo(error(), "X11 graphics exposure event (\n"
				"  drawable id: %%\n"
				"  x, y: %%, %%\n"
				"  width, height: %%, %%\n"
				"  opcode: %%.%%\n"
				"  count: %%\n"
				") [%%]\n",
				event->drawableId,
				event->x, event->y,
				event->width, event->height,
				event->majorOpcode, event->minorOpcode,
				event->count,
				event->sequenceNumber
			);
		}
		else if (message->messageCode == XMessage::NoExposure) {
			Ref<XNoExposureEvent> event = message;
			printTo(error(), "X11 no exposure event (\n"
				"  drawable id: %%\n"
				"  opcode: %%.%%\n"
				") [%%]\n",
				event->drawableId,
				event->majorOpcode, event->minorOpcode,
				event->sequenceNumber
			);
		}
		else {
			printTo(error(), "X11 %% (%%)\n", x11MessageName(message->messageCode), message->messageCode);
		}
	}
}

} // namespace ftl
