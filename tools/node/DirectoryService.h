/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "WebService.h"

namespace ccnode {

class DirectoryService: public WebService
{
public:
    static Ref<DirectoryService> create() {
        return new DirectoryService;
    }

    WebServicePrototype *configPrototype() const override { return configPrototype_; }
    Ref<ServiceInstance> createInstance(const MetaObject *config) const override;

    static void establish(WebServicePrototype *configPrototype);

private:
    DirectoryService();

    Ref<WebServicePrototype> configPrototype_;
};

} // namespace ccnode
