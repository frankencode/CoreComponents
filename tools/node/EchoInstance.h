/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "EchoDelegate.h"

namespace ccnode {

class EchoInstance: public DeliveryInstance
{
public:
    static Ref<EchoInstance> create(const MetaObject *config) {
        return new EchoInstance{config};
    }

    Ref<DeliveryDelegate> createDelegate(DeliveryWorker *worker) const override {
        return EchoDelegate::create(worker);
    }

private:
    EchoInstance(const MetaObject *config):
        DeliveryInstance{config}
    {}
};

} // namespace ccnode
