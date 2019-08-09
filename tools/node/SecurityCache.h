/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>
#include <cc/Thread>
#include <cc/Mutex>
#include <cc/Channel>

namespace ccnode {

using namespace cc;

class NodeConfig;

class SecurityCache: public Thread
{
public:
    static Ref<SecurityCache> start(const NodeConfig *nodeConfig);
    void prepareSessionResumption(gnutls_session_t session);

private:
    SecurityCache(const NodeConfig *nodeConfig);
    ~SecurityCache();

    const NodeConfig *nodeConfig() const;

    virtual void run();
    void deleteKey(gnutls_datum_t *key);

    const NodeConfig *nodeConfig_;
    double refreshInterval_;
    Ref<Mutex> mutex_;
    Ref< Channel<bool> > shutdown_;
    gnutls_datum_t key_;
};

} // namespace ccnode
