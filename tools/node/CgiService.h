/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "ServiceDefinition.h"

namespace ccnode {

class CgiService: public ServiceDefinition
{
public:
    static Ref<CgiService> create() {
        return new CgiService;
    }

    ServicePrototype *configPrototype() const override { return configPrototype_; }
    Ref<ServiceInstance> createInstance(const MetaObject *config) const override;

    static void establish(ServicePrototype *configPrototype);

private:
    CgiService();

    Ref<ServicePrototype> configPrototype_;
};

} // namespace ccnode
