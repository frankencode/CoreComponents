/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "EchoInstance.h"

namespace fluxnode {

class EchoService: public ServiceDefinition
{
public:
    static Ref<EchoService> create() {
        return new EchoService;
    }

    virtual ServicePrototype *configPrototype() const { return configPrototype_; }
    virtual Ref<ServiceInstance> createInstance(MetaObject *config) const { return EchoInstance::create(config); }

private:
    EchoService():
        configPrototype_(ServicePrototype::create("Echo"))
    {}

    Ref<ServicePrototype> configPrototype_;
};

class EchoAnnouncer {
public:
    EchoAnnouncer()
    {
        static bool done = false;
        if (done) return;
        Ref<EchoService> echoService = EchoService::create();
        serviceRegistry()->registerService(echoService);
        done = true;
    }
};

namespace { EchoAnnouncer announcer; }

} // namespace fluxnode
