/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryDelegate>
#include <cc/node/DeliveryInstance>
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

const LoggingInstance *DeliveryDelegate::errorLoggingInstance() const
{
    return worker_->deliveryInstance()->errorLoggingInstance();
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

#if 0
Stream *DeliveryDelegate::errorStream() const
{
    return worker_->deliveryInstance()->loggingInstance()->errorStream();
}

Stream *DeliveryDelegate::warningStream() const
{
    return worker_->deliveryInstance()->loggingInstance()->warningStream();
}

Stream *DeliveryDelegate::noticeStream() const
{
    return worker_->deliveryInstance()->loggingInstance()->noticeStream();
}

Stream *DeliveryDelegate::infoStream() const
{
    return worker_->deliveryInstance()->loggingInstance()->infoStream();
}

Stream *DeliveryDelegate::debugStream() const
{
    return worker_->deliveryInstance()->loggingInstance()->debugStream();
}
#endif

}} // namespace cc::node
