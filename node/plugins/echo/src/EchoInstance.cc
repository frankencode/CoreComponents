/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/EchoDelegate>
#include <cc/node/EchoInstance>

namespace cc {
namespace node {

Ref<EchoInstance> EchoInstance::create(const MetaObject *config)
{
    return new EchoInstance{config};
}

Ref<DeliveryDelegate> EchoInstance::createDelegate(DeliveryWorker *worker) const
{
    return EchoDelegate::create(worker);
}

EchoInstance::EchoInstance(const MetaObject *config):
    DeliveryInstance{config}
{}

}} // namespace cc::node
