/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_MARKUPPROTOCOL_H
#define FLUXDOC_MARKUPPROTOCOL_H

#include <flux/MetaProtocol>

namespace flux {

template<class> class Singleton;

namespace doc {

class MarkupProtocol: public MetaProtocol
{
private:
	friend class Singleton<MarkupProtocol>;
	MarkupProtocol();
};

const MarkupProtocol *markupProtocol();

}} // namespace flux::doc

#endif // FLUXDOC_MARKUPPROTOCOL_H
