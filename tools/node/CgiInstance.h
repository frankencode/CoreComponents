/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    static Ref<CgiInstance> create(MetaObject *config);

    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

    String script() const { return script_; }
    StringList *server() const { return server_; }
    VariantMap *environment() const { return environment_; }

    SocketAddressList *serverAddress() { return serverAddress_; }
    int randomSeed() { return delegateCount_++; }

protected:
    CgiInstance(MetaObject *config);

    String script_;
    Ref<StringList> server_;
    Ref<MetaObject> environment_;

    Ref<SocketAddressList> serverAddress_;
    int delegateCount_;
};

} // namespace ccnode
