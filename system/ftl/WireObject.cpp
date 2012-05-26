/*
 * WireObject.cpp -- the Wire object
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Format.hpp"
#include "WireObject.hpp"

namespace ftl
{

Variant WireObject::member(const String& name) const
{
	Variant value;
	Ref<StringList, Owner> parts = name->split('.');
	Ref<WireObject> object = this;
	for (int i = 0, n = parts->length(); i < n; ++i) {
		if (object->lookup(parts->at(i), &value)) {
			if (i == n - 1) break;
			object = value;
			if (object) continue;
		}
		throw WireObjectException(Format("No such member: \"%%\"") << name);
	}
	return value;
}

void WireObject::setMember(const String& name, const Variant& newValue)
{
	Variant value;
	Ref<StringList, Owner> parts = name->split('.');
	Ref<WireObject> object = this;
	for (int i = 0, n = parts->length(); i < n; ++i) {
		if (object->lookup(parts->at(i), &value)) {
			if (i == n - 1) break;
			object = value;
			if (object) continue;
		}
		throw WireObjectException(Format("No such member: \"%%\"") << name);
	}
	object->setValue(name, newValue);
}

} // namespace ftl
