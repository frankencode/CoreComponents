/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    worker_(worker)
{}

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
