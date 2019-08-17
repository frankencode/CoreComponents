/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include "DeliveryService.h"

namespace cc { template<class> class Singleton; }

namespace ccnode {

class DeliveryRegistry: public Object
{
public:
    static DeliveryRegistry *instance();

    template<class T>
    void registerService() { registerService(T::create()); }

    void registerService(DeliveryService *service);

    DeliveryService *serviceByName(const String &name) const;

private:
    friend class Singleton<DeliveryRegistry>;

    DeliveryRegistry();

    typedef Map< String, Ref<DeliveryService> > ServiceByName;
    Ref<ServiceByName> serviceByName_;
};

} // namespace ccnode
