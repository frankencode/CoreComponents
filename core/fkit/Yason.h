/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_YASON_H
#define FKIT_YASON_H

#include "Map.h"
#include "Variant.h"
#include "UserException.h"
#include "Stream.h"

namespace fkit
{

class YasonException: public UserException
{
public:
	YasonException(const String &error, ByteArray *text, int offset);

	String error() const { return error_; }

	String text() const { return text_; }
	int offset() const { return offset_; }

	int line() const { return line_; }
	int pos() const { return pos_; }

private:
	String error_;
	String text_;
	int offset_;
	int line_;
	int pos_;
};

class YasonObject: public Map<String, Variant>
{
	typedef Map<String, Variant> Super;

public:
	inline static Ref<YasonObject> create(const String &className = "") { return new YasonObject(className); }
	inline static Ref<YasonObject> clone(YasonObject *a) { return new YasonObject(*a); }

	inline String className() const { return className_; }
	Variant toVariant() const;
	String toString() const;

protected:
	friend class YasonParser;

	YasonObject(const String &className = ""): className_(className) {}

	explicit YasonObject(const YasonObject &b)
		: Super(b),
		  className_(b.className_)
	{}

	String className_;
};

typedef Map<String, Ref<YasonObject> > YasonProtocol;

class Yason
{
public:
	static Variant parse(ByteArray *text, YasonProtocol *protocol = 0, YasonObject *virgin = 0);
	static String stringify(Variant value);
};

} // namespace fkit

#endif // FKIT_YASON_H
