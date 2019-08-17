/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryDelegate>
#include <cc/node/HttpResponseGenerator>
#include <cc/node/DeliveryWorker>

namespace cc {
namespace node {

DeliveryDelegate::DeliveryDelegate(DeliveryWorker *worker):
    worker_{worker}
{}

const NodeConfig *DeliveryDelegate::nodeConfig() const
{
    return worker_->nodeConfig();
}

const DeliveryInstance *DeliveryDelegate::deliveryInstance() const
{
    return worker_->deliveryInstance();
}

HttpServerConnection *DeliveryDelegate::client() const
{
    return worker_->client();
}

HttpResponseGenerator *DeliveryDelegate::response() const
{
    return worker_->response();
}

void DeliveryDelegate::autoSecureForwardings()
{
    worker_->autoSecureForwardings();
}

}} // namespace cc::node
