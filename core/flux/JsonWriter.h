/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_JSONWRITER_H
#define FLUX_JSONWRITER_H

#include "yason.h"

namespace flux
{

class JsonWriter: public Object
{
public:
	static Ref<JsonWriter> create(Format format = Format(), String indent = "  ");
	void write(Variant value);

protected:
	JsonWriter(Format format, String indent);
	void writeValue(Variant value, int depth);
	void writeList(Variant value, int depth);
	void writeObject(Variant value, int depth);
	void writeIndent(int depth);

	template<class T>
	void writeTypedList(Variant value, int depth);

	Format format_;
	String indent_;
};

} // namespace flux

#endif // FLUX_JSONWRITER_H
