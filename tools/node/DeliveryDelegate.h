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

class NodeConfig;
class DeliveryWorker;
class DeliveryInstance;
class HttpServerConnection;

class DeliveryDelegate: public Object
{
public:
    virtual void process(HttpRequest *request) = 0;

protected:
    DeliveryDelegate(DeliveryWorker *worker);

    const NodeConfig *nodeConfig() const;

    const DeliveryInstance *deliveryInstance() const;
    HttpServerConnection *client() const;

    HttpResponseGenerator *response() const;

    void autoSecureForwardings();

private:
    DeliveryWorker *worker_;
};

} // namespace ccnode
