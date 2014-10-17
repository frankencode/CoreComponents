/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Response.h"
#include "ServiceWorker.h"
#include "ServiceDelegate.h"

namespace fluxnode
{

ServiceDelegate::ServiceDelegate(ServiceWorker *worker)
    : worker_(worker)
{}

ServiceInstance *ServiceDelegate::serviceInstance() const
{
    return worker_->serviceInstance();
}

ClientConnection *ServiceDelegate::client() const
{
    return worker_->client();
}

void ServiceDelegate::status(int statusCode, String reasonPhrase)
{
    worker_->response()->status(statusCode, reasonPhrase);
}

void ServiceDelegate::header(String name, String value)
{
    worker_->response()->header(name, value);
}

void ServiceDelegate::begin(ssize_t contentLength)
{
    worker_->response()->begin(contentLength);
}

void ServiceDelegate::write(String bytes)
{
    worker_->response()->write(bytes);
}

Format ServiceDelegate::chunk(String pattern)
{
    return worker_->response()->chunk(pattern);
}

Format ServiceDelegate::chunk()
{
    return worker_->response()->chunk();
}

void ServiceDelegate::end()
{
    worker_->response()->end();
}

void ServiceDelegate::close()
{
    worker_->close();
}

} // namespace fluxnode
