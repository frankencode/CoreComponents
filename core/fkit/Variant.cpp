/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Variant.h"

namespace fkit
{

const char *Variant::typeName(int type, int itemType)
{
	if (type == UndefType) return "undefined";
	else if (type == IntType) return "integer";
	else if (type == BoolType) return "boolean";
	else if (type == FloatType) return "real";
	else if (type == ObjectType) return "object";
	else if (type == StringType) return "string";
	else if (type == AnyType) return "any";
	else if (type == ListType) {
		if (itemType == IntType) return "list of integer";
		else if (itemType == BoolType) return "list of boolean";
		else if (itemType == FloatType) return "list of real";
		else if (itemType == ObjectType) return "list of object";
		else if (itemType == StringType) return "list of string";
		else return "list";
	}
	return "unknown";
}

} // namespace fkit
