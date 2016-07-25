/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    static Ref<EchoInstance> create(MetaObject *config) {
        return new EchoInstance(config);
    }

    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const {
        return EchoDelegate::create(worker);
    }

private:
    EchoInstance(MetaObject *config)
        : ServiceInstance(config)
    {}
};

} // namespace ccnode
