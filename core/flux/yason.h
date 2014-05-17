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

class YasonProtocol: public Object
{
public:
	inline static Ref<YasonProtocol> create() {
		return new YasonProtocol;
	}

	template<class Prototype>
	Prototype *define(const String &className) {
		Ref<Prototype> prototype = Prototype::create(className);
		define(prototype);
		return prototype;
	}

	template<class Prototype>
	Prototype *define() {
		Ref<Prototype> prototype = Prototype::create();
		define(prototype);
		return prototype;
	}

	YasonObject *define(YasonObject *prototype) {
		prototype->define();
		prototypes()->insert(prototype->className(), prototype);
		return prototype;
	}

	template<class Prototype>
	static Ref<YasonObject> createPrototype() {
		Ref<YasonObject> prototype = Prototype::create();
		prototype->define();
		return prototype;
	}

	virtual YasonObject *lookup(String className) {
		YasonObject *prototype = 0;
		prototypes()->lookup(className, &prototype);
		return prototype;
	}

	virtual int maxCount() const { return flux::intMax; }

	inline bool lookup(const String &className, YasonObject **prototype) {
		*prototype = lookup(className);
		return *prototype;
	}

protected:
	friend class YasonSyntax;

	virtual Ref<YasonObject> produce(YasonObject *prototype) {
		return prototype->produce();
	}

private:
	typedef Map<String, Ref<YasonObject> > Prototypes;
	Prototypes *prototypes() {
		if (!prototypes_) prototypes_ = Prototypes::create();
		return prototypes_;
	}
	Ref<Prototypes> prototypes_;
};

namespace yason {
	Variant parse(const ByteArray *text, YasonProtocol *protocol = 0);
	String stringify(Variant value);
}

} // namespace flux

#endif // FLUX_YASON_H
