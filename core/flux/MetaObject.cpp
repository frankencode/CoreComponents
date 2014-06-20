/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Guard.h"
#include "YasonSyntax.h"
#include "MetaProtocol.h"
#include "MetaObject.h"

namespace flux
{

MetaObject::MetaObject(const String &className, MetaProtocol *protocol)
	: className_(className),
	  protocol_(protocol)
{}

Variant MetaObject::toVariant() const
{
	return Ref<MetaObject>(const_cast<MetaObject *>(this));
}

String MetaObject::toString() const
{
	return yason::stringify(toVariant());
}

MetaObjectList *MetaObject::children()
{
	if (!children_) {
		Guard<SpinLock> guard(&mutex_);
		if (!children_) children_ = MetaObjectList::create();
	}
	return children_;
}

MetaProtocol *MetaObject::protocol()
{
	if (!protocol_) {
		Guard<SpinLock> guard(&mutex_);
		if (!protocol_) protocol_ = MetaProtocol::create();
	}
	return protocol_;
}

Ref<MetaObject> MetaObject::clone()
{
	Ref<MetaObject> object = produce();
	object->MetaObject::autocomplete(this);
	if (hasChildren()) {
		for (int i = 0; i < children()->count(); ++i) {
			object->children()->append(children()->at(i)->clone());
		}
	}
	return object;
}

void MetaObject::autocomplete(const MetaObject *prototype)
{
	if (!prototype) return;

	if (prototype->count() != count()) {
		for (int i = 0; i < prototype->count(); ++i) {
			String name = prototype->keyAt(i);
			if (count() <= i || keyAt(i) != name)
				insert(name, prototype->valueAt(i));
		}
	}
}

Token *MetaObject::nameToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
	return yasonSyntax()->nameToken(text, objectToken, memberName);
}

Token *MetaObject::valueToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
	return yasonSyntax()->valueToken(text, objectToken, memberName);
}

Token *MetaObject::childToken(Token *objectToken, int childIndex)
{
	return yasonSyntax()->childToken(objectToken, childIndex);
}

} // namespace flux
