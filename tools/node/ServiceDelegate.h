/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace ccnode {

class ServiceWorker;
class ServiceInstance;
class HttpClientConnection;

class ServiceDelegate: public Object
{
public:
    virtual void process(HttpRequest *request) = 0;

protected:
    ServiceDelegate(ServiceWorker *worker);

    ServiceInstance *serviceInstance() const;
    HttpClientConnection *client() const;

    HttpResponse *response() const;

    void autoSecureForwardings();

private:
    ServiceWorker *worker_;
};

} // namespace ccnode
