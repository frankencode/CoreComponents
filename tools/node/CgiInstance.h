/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/net/SocketAddress>
#include "ServiceInstance.h"
#include "CgiDelegate.h"

namespace ccnode {

using namespace cc::net;

class CgiInstance: public virtual ServiceInstance
{
public:
    static Ref<CgiInstance> create(const MetaObject *config);

    Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const override;

    String script() const { return script_; }
    StringList *server() const { return server_; }
    VariantMap *environment() const { return environment_; }

    const SocketAddressList *serverAddress() const { return serverAddress_; }

    int randomSeed() const {
        static int counter = 0;
        return counter++;
    }

protected:
    CgiInstance(const MetaObject *config);

    String script_;
    Ref<StringList> server_;
    Ref<MetaObject> environment_;

    Ref<SocketAddressList> serverAddress_;
};

} // namespace ccnode
