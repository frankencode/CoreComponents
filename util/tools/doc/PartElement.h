/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PARTELEMENT_H
#define FLUX_PARTELEMENT_H

#include "PathElement.h"

namespace flux {
namespace doc {

class PartElement: public PathElement
{
public:
    static Ref<PartElement> create() { return new PartElement; }

protected:
    PartElement(String className = "Part")
        : PathElement(className)
    {}

    virtual Ref<MetaObject> produce()
    {
        return create();
    }
};

}} // namespace flux::doc

#endif // FLUX_PARTELEMENT_H
