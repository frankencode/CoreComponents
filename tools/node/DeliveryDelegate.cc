/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "HttpResponseGenerator.h"
#include "DeliveryWorker.h"
#include "DeliveryDelegate.h"

namespace ccnode {

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

} // namespace ccnode
