/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "EchoDelegate.h"

namespace ccnode {

class EchoInstance: public ServiceInstance
{
public:
    static Ref<EchoInstance> create(const MetaObject *config) {
        return new EchoInstance{config};
    }

    Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const override {
        return EchoDelegate::create(worker);
    }

private:
    EchoInstance(const MetaObject *config):
        ServiceInstance{config}
    {}
};

} // namespace ccnode
