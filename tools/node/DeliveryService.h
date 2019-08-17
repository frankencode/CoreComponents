/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/yason>
#include "DeliveryDelegate.h"
#include "DeliveryInstance.h"

namespace ccnode {

using namespace cc;

class DeliveryPrototype: public MetaObject
{
public:
    static Ref<DeliveryPrototype> create(const String &className);

protected:
    DeliveryPrototype(const String &className);
};

class DeliveryService: public Object
{
public:
    virtual DeliveryPrototype *configPrototype() const = 0;
    virtual Ref<DeliveryInstance> createInstance(const MetaObject *config) const = 0;
};

} // namespace ccnode
