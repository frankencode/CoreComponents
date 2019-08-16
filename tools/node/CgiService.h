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

class CgiService: public WebService
{
public:
    static Ref<CgiService> create() {
        return new CgiService;
    }

    WebServicePrototype *configPrototype() const override { return configPrototype_; }
    Ref<ServiceInstance> createInstance(const MetaObject *config) const override;

    static void establish(WebServicePrototype *configPrototype);

private:
    CgiService();

    Ref<WebServicePrototype> configPrototype_;
};

} // namespace ccnode
