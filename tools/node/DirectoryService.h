/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "DeliveryService.h"

namespace ccnode {

class DirectoryService: public DeliveryService
{
public:
    static Ref<DirectoryService> create() {
        return new DirectoryService;
    }

    DeliveryPrototype *configPrototype() const override { return configPrototype_; }
    Ref<DeliveryInstance> createInstance(const MetaObject *config) const override;

    static void establish(DeliveryPrototype *configPrototype);

private:
    DirectoryService();

    Ref<DeliveryPrototype> configPrototype_;
};

} // namespace ccnode
