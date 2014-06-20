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

#include <flux/MetaProtocol.h>

namespace flux { template<class> class Singleton; }

namespace fluxdoc
{

using namespace flux;

class MarkupProtocol: public MetaProtocol
{
private:
	friend class Singleton<MarkupProtocol>;
	MarkupProtocol();
};

const MarkupProtocol *markupProtocol();

} // namespace fluxdoc

#endif // FLUXDOC_MARKUPPROTOCOL_H
