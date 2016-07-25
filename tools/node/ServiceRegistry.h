/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include "ServiceDefinition.h"

namespace cc { template<class> class Singleton; }

namespace ccnode {

class ServiceRegistry: public Object
{
public:
    void registerService(ServiceDefinition *service);
    ServiceDefinition *serviceByName(String name) const;

private:
    friend class Singleton<ServiceRegistry>;

    ServiceRegistry();

    typedef Map< String, Ref<ServiceDefinition> > ServiceByName;
    Ref<ServiceByName> serviceByName_;
};

ServiceRegistry *serviceRegistry();

} // namespace ccnode

