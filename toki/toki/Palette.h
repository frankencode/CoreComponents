/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_PALETTE_H
#define FLUXTOKI_PALETTE_H

#include <flux/yason.h>
#include "Style.h"

namespace fluxtoki
{

using namespace flux;

class Palette: public YasonObject
{
public:

private:
	Palette();

	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);
};

} // namespace fluxtoki

#endif // FLUXTOKI_PALETTE_H
