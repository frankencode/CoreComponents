/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "str.h"
#include "Variant.h"

namespace flux
{

const char *Variant::typeName(int type, int itemType)
{
	if (type == UndefType) return "undefined";
	else if (type == IntType) return "integer";
	else if (type == BoolType) return "boolean";
	else if (type == FloatType) return "real";
	else if (type == ColorType) return "color";
	else if (type == ObjectType) return "object";
	else if (type == StringType) return "string";
	else if (type == AnyType) return "any";
	else if (type == ListType) {
		if (itemType == IntType) return "list of integer";
		else if (itemType == BoolType) return "list of boolean";
		else if (itemType == FloatType) return "list of real";
		else if (itemType == ColorType) return "list of color";
		else if (itemType == ObjectType) return "list of object";
		else if (itemType == StringType) return "list of string";
		else return "list";
	}
	else if (type == MapType) {
		return "map";
	}
	return "unknown";
}

String str(const Variant &x)
{
	if (type(x) == Variant::UndefType) return "";
	else if (type(x) == Variant::BoolType) return str(bool(x));
	else if (type(x) == Variant::IntType) return str(int(x));
	else if (type(x) == Variant::FloatType) return str(float(x));
	else if (type(x) == Variant::ColorType) return str(Color(x));
	else if (type(x) == Variant::StringType) return String(x);
	/*else if (type(x) == Variant::ObjectType)*/ return str((void *)cast<Object>(x));
}

} // namespace flux
