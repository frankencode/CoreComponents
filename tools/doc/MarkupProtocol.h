/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_MARKUPPROTOCOL_H
#define FLUXDOC_MARKUPPROTOCOL_H

#include <flux/yason.h>
#include <flux/Singleton.h>

namespace fluxdoc
{

using namespace flux;

class MarkupProtocol: public YasonProtocol, public Singleton<MarkupProtocol>
{
private:
	friend class Singleton<MarkupProtocol>;
	MarkupProtocol();
};

inline MarkupProtocol *markupProtocol() { return MarkupProtocol::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_MARKUPPROTOCOL_H
