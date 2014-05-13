/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "YasonSyntax.h"
#include "YasonWriter.h"
#include "yason.h"

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

YasonObjectList *YasonObject::children() const
{
	if (!children_) children_ = YasonObjectList::create();
	return children_;
}

Ref<YasonObject> YasonObject::clone()
{
	Ref<YasonObject> object = produce();
	object->autocomplete(this);
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

namespace yason
{

Variant parse(const ByteArray *text, YasonProtocol *protocol)
{
	return yasonSyntax()->parse(text, protocol);
}

String stringify(Variant value)
{
	Format format;
	YasonWriter::create(format)->write(value);
	return format;
}

} // namespace yason

} // namespace flux
