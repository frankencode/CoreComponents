/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include "elements.h"
#include "MarkupProtocol.h"

namespace flux {
namespace doc {

MarkupProtocol::MarkupProtocol()
{
	define<TitleElement>();
	define<AuthorElement>();
	define<ParagraphElement>();
	define<ListElement>();

	define<PartElement>();
	define<ImageElement>();
	define<CodeElement>();
}

const MarkupProtocol *markupProtocol() { return Singleton<MarkupProtocol>::instance(); }

}} // namespace flux::doc
