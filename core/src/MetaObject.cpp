/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Guard>
#include <flux/YasonSyntax>
#include <flux/MetaProtocol>
#include <flux/MetaObject>

namespace flux {

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
