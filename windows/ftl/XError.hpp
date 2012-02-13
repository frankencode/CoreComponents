/*
 * XMessage.hpp -- X11 error message
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_XERROR_HPP
#define FTL_XERROR_HPP

#include "XMessage.hpp"

namespace ftl
{

class XError: public XMessage
{
public:
	XError()
		: XMessage(XMessage::Error)
	{}
	
	enum ErrorCode {
		Request        = 1,
		Value          = 2,
		Window         = 3,
		Pixmap         = 4,
		Atom           = 5,
		Cursor         = 6,
		Font           = 7,
		Match          = 8,
		Drawable       = 9,
		Access         = 10,
		Alloc          = 11,
		Colormap       = 12,
		GContext       = 13,
		IdChoice       = 14,
		Name           = 15,
		Length         = 16,
		Implementation = 17
	};
	
	uint8_t errorCode;
	uint16_t sequenceNumber;
	union {
		uint32_t badResourceId;
		uint32_t badValue;
	}
	uint16_t minorOpcode;
	uint8_t majorOpcode;
};

} // namespace ftl

#endif // FTL_XERROR_HPP
