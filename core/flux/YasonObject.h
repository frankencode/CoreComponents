/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_YASONOBJECT_H
#define FLUX_YASONOBJECT_H

#include "Token.h"
#include "Variant.h"

namespace flux
{

class YasonProtocol;

class YasonObject;
typedef List< Ref<YasonObject> > YasonObjectList;

class YasonObject: public VariantMap
{
public:
	inline static Ref<YasonObject> create(const String &className = "", YasonProtocol *protocol = 0) {
		return new YasonObject(className, protocol);
	}

	inline String className() const { return className_; }
	Variant toVariant() const;
	String toString() const;

	inline bool hasChildren() const { return children_; }
	YasonObjectList *children() const;

	Ref<YasonObject> clone();

	inline YasonProtocol *protocol() const { return protocol_; }

protected:
	friend class YasonSyntax;
	friend class YasonProtocol;

	YasonObject(const String &className = "", YasonProtocol *protocol = 0)
		: className_(className),
		  protocol_(protocol)
	{}

	virtual Ref<YasonObject> produce() {
		return YasonObject::create(className());
	}

	virtual void define() {}

	virtual void realize(const ByteArray *text, Token *objectToken) {}

	void autocomplete(const YasonObject *prototype);

	static Token *nameToken(const ByteArray *text, Token *objectToken, const String &memberName);
	static Token *valueToken(const ByteArray *text, Token *objectToken, const String &memberName);

private:
	String className_;
	mutable Ref<YasonObjectList> children_;
	Ref<YasonProtocol> protocol_;
};

} // namespace flux

#endif // FLUX_YASONOBJECT_H
