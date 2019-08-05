/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "HttpResponseGenerator.h"
#include "ServiceWorker.h"
#include "ServiceDelegate.h"

namespace ccnode {

ServiceDelegate::ServiceDelegate(ServiceWorker *worker):
    worker_{worker}
{}

const NodeConfig *ServiceDelegate::nodeConfig() const
{
    return worker_->nodeConfig();
}

ServiceInstance *ServiceDelegate::serviceInstance() const
{
    return worker_->serviceInstance();
}

HttpServerConnection *ServiceDelegate::client() const
{
    return worker_->client();
}

HttpResponseGenerator *ServiceDelegate::response() const
{
    return worker_->response();
}

void ServiceDelegate::autoSecureForwardings()
{
    worker_->autoSecureForwardings();
}

} // namespace ccnode
