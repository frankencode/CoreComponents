/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "YasonSyntax.h"
#include "YasonObject.h"

namespace flux
{

Variant YasonObject::toVariant() const
{
	return Ref<YasonObject>(const_cast<YasonObject *>(this));
}

String YasonObject::toString() const
{
	return yason::stringify(toVariant());
}

YasonObjectList *YasonObject::children()
{
	if (!children_) {
		Guard<SpinLock> guard(&mutex_);
		if (!children_) children_ = YasonObjectList::create();
	}
	return children_;
}

YasonProtocol *YasonObject::protocol()
{
	if (!protocol_) {
		Guard<SpinLock> guard(&mutex_);
		if (!protocol_) protocol_ = YasonProtocol::create();
	}
	return protocol_;
}

Ref<YasonObject> YasonObject::clone()
{
	Ref<YasonObject> object = produce();
	object->YasonObject::autocomplete(this);
	if (hasChildren()) {
		for (int i = 0; i < children()->size(); ++i) {
			object->children()->append(children()->at(i)->clone());
		}
	}
	return object;
}

void YasonObject::autocomplete(const YasonObject *prototype)
{
	if (!prototype) return;

	if (prototype->size() != size()) {
		for (int i = 0; i < prototype->size(); ++i) {
			String name = prototype->keyAt(i);
			if (size() <= i || keyAt(i) != name)
				insert(name, prototype->valueAt(i));
		}
	}
}

Token *YasonObject::nameToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
	return yasonSyntax()->nameToken(text, objectToken, memberName);
}

Token *YasonObject::valueToken(const ByteArray *text, Token *objectToken, const String &memberName)
{
	return yasonSyntax()->valueToken(text, objectToken, memberName);
}

} // namespace flux
