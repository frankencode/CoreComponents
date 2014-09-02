/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_ELEMENT_H
#define FLUXDOC_ELEMENT_H

#include <flux/yason.h>

namespace flux {
namespace doc {

typedef MetaObject Element;
typedef List< Ref<Element> > ElementList;

}} // namespace flux::doc

#endif // FLUXDOC_ELEMENT_H
