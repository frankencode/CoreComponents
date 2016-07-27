/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "HttpResponse.h"
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

HttpClientConnection *ServiceDelegate::client() const
{
    return worker_->client();
}

HttpResponse *ServiceDelegate::response() const
{
    return worker_->response();
}

void ServiceDelegate::autoSecureForwardings()
{
    worker_->autoSecureForwardings();
}

} // namespace ccnode