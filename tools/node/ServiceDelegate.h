/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include "HttpRequest.h"
#include "HttpResponseGenerator.h"

namespace ccnode {

class ServiceWorker;
class ServiceInstance;
class HttpServerConnection;

class ServiceDelegate: public Object
{
public:
    virtual void process(HttpRequest *request) = 0;

protected:
    ServiceDelegate(ServiceWorker *worker);

    ServiceInstance *serviceInstance() const;
    HttpServerConnection *client() const;

    HttpResponseGenerator *response() const;

    void autoSecureForwardings();

private:
    ServiceWorker *worker_;
};

} // namespace ccnode
