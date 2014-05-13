/*
 * Copyright (C) 2007-2014 Frank Mertens.
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
class Arguments;

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
	friend class Arguments;

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

class YasonProtocol: public Map<String, Ref<YasonObject> >
{
public:
	inline static Ref<YasonProtocol> create() {
		return new YasonProtocol;
	}

	template<class Prototype>
	Prototype *define(String className) {
		Ref<Prototype> prototype = Prototype::create(className);
		cast<YasonObject>(prototype)->define();
		insert(className, prototype);
		return prototype;
	}

	template<class Prototype>
	Prototype *define() {
		Ref<Prototype> prototype = Prototype::create();
		cast<YasonObject>(prototype)->define();
		insert(prototype->className(), prototype);
		return prototype;
	}

	YasonObject *define(YasonObject *prototype) {
		prototype->define();
		insert(prototype->className(), prototype);
		return prototype;
	}

protected:
	friend class YasonSyntax;

	virtual Ref<YasonObject> produce(YasonObject *prototype) {
		return prototype->produce();
	}
};

namespace yason {
	Variant parse(const ByteArray *text, YasonProtocol *protocol = 0);
	String stringify(Variant value);
}

} // namespace flux

#endif // FLUX_YASON_H
