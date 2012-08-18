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

Ref<WireObject> WireObject::resolve(String path, String *name, Variant *value) const
{
	String hn;  if (!name) name = &hn;
	Variant hv; if (!value) value = &hv;
	Ref<StringList, Owner> parts = path->split('.');
	Ref<WireObject> object = this;
	for (int i = 0, n = parts->length(); i < n; ++i) {
		*name = parts->at(i);
		if (object->lookup(*name, value)) {
			if (i == n - 1) break;
			object = *value;
			if (object) continue;
		}
		return 0;
	}
	return object;
}

bool WireObject::hasMember(String path) const
{
	return resolve(path);
}

Variant WireObject::member(String path) const
{
	Variant value;
	if (!resolve(path, 0, &value))
		throw WireObjectException(Format("No such member: \"%%\"") << path);
	return value;
}

void WireObject::setMember(String path, Variant value)
{
	String name;
	Ref<WireObject> object = resolve(path, &name, &value);
	if (!object)
		throw WireObjectException(Format("No such member: \"%%\"") << path);
	object->setValue(name, value);
}

} // namespace ftl
