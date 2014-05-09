/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_YASON_H
#define FLUX_YASON_H

#include "exceptions.h"
#include "Map.h"
#include "Variant.h"
#include "Stream.h"
#include "Token.h"

namespace flux
{

class YasonObject;

typedef List< Ref<YasonObject> > YasonObjectList;

class YasonProtocol;

class YasonObject: public Map<String, Variant>
{
	typedef Map<String, Variant> Super;

public:
	inline static Ref<YasonObject> create(const String &className = "", YasonProtocol *protocol = 0) {
		return new YasonObject(className, protocol);
	}

	inline String className() const { return className_; }
	Variant toVariant() const;
	String toString() const;

	inline bool hasChildren() const { return children_; }
	YasonObjectList *children() const;

	inline YasonProtocol *protocol() const { return protocol_; }

protected:
	friend class YasonSyntax;
	friend class YasonProtocol;

	YasonObject(const String &className = "", YasonProtocol *protocol = 0)
		: className_(className),
		  protocol_(protocol)
	{}

	virtual void define() {}

	virtual Ref<YasonObject> produce() {
		return YasonObject::create(className());
	}

	virtual void realize(const ByteArray *text, Token *objectToken) {}

	static Token *nameToken(const ByteArray *text, Token *objectToken, const String &memberName);
	static Token *valueToken(const ByteArray *text, Token *objectToken, const String &memberName);

private:
	String className_;
	mutable Ref<YasonObjectList> children_;
	Ref<YasonProtocol> protocol_;
};

class YasonProtocol: public Map<String, Ref<YasonObject> >
{
public:
	inline static Ref<YasonProtocol> create() {
		return new YasonProtocol;
	}

	template<class Prototype>
	void add() {
		Ref<YasonObject> prototype = Prototype::create();
		prototype->define();
		insert(prototype->className(), prototype);
	}

	template<class Prototype>
	void add(String className) {
		Ref<YasonObject> prototype = Prototype::create(className);
		prototype->define();
		insert(className, prototype);
	}

protected:
	friend class YasonSyntax;

	virtual Ref<YasonObject> produce(YasonObject *prototype) {
		return prototype->produce();
	}
};

class Yason
{
public:
	static Variant parse(const ByteArray *text, YasonProtocol *protocol = 0);
	static String stringify(Variant value);
};

} // namespace flux

#endif // FLUX_YASON_H
