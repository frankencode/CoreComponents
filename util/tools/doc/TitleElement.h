/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_TITLETELEMENT_H
#define FLUXDOC_TITLETELEMENT_H

#include "TextElement.h"

namespace flux {
namespace doc {

class TitleElement: public TextElement
{
public:
    static Ref<TitleElement> create() { return new TitleElement; }

protected:
    TitleElement(String className = "Title")
        : TextElement(className)
    {}

    virtual Ref<MetaObject> produce()
    {
        return TitleElement::create();
    }
};

}} // namespace flux::doc

#endif // FLUXDOC_TITLETELEMENT_H
