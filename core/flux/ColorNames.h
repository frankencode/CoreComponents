/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_COLORNAMES_H
#define FLUX_COLORNAMES_H

#include "Color.h"

namespace flux
{

template<class Char, class Value> class PrefixTree;

template<class SubClass> class Singleton;

class ColorNames: public Object
{
public:
	bool lookup(const char *name, Color *color) const;

private:
	friend class Singleton<ColorNames>;

	ColorNames();

	typedef PrefixTree<char, Color> ColorByName;
	Ref<ColorByName> colorByName_;
};

const ColorNames *colorNames();

} // namespace flux

#endif // FLUX_COLORNAMES_H
