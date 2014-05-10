/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "fragments.h"
#include "MarkupProtocol.h"

namespace fluxdoc
{

MarkupProtocol::MarkupProtocol()
{
	define<Title>();
	define<Author>();
	define<Heading>();
	define<Paragraph>();
	define<fluxdoc::Item>();

	define<Part>();
	define<Image>();
	define<Code>();
}

} // namespace fluxdoc
