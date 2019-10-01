/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/SecurityCache>
#include <cc/node/NodeConfig>
#include <cc/node/debug>
#include <cc/Guard>
#include <cc/System>
#include <string.h>

namespace cc {
namespace node {

Ref<SecurityCache> SecurityCache::start(const NodeConfig *nodeConfig)
{
    Ref<SecurityCache> cache = new SecurityCache{nodeConfig};
    if (cache->refreshInterval_ > 0)
        cache->Thread::start();
    return cache;
}

SecurityCache::SecurityCache(const NodeConfig *nodeConfig):
    nodeConfig_{nodeConfig},
    refreshInterval_{nodeConfig->security()->sessionResumptionKeyRefresh()},
    mutex_{Mutex::create()},
    shutdown_{Channel<bool>::create()}
{
    if (refreshInterval_ > 0) {
        int ret = gnutls_session_ticket_key_generate(&key_);
        if (ret != GNUTLS_E_SUCCESS) {
            CCNODE_ERROR() << gnutls_strerror(ret) << nl;
            refreshInterval_ = 0;
        }
        else {
            CCNODE_NOTICE() << "Session resumption enabled, key refresh interval set to " << refreshInterval_ << "s" << nl;
            CCNODE_INFO() << "Initial session resumption key generated, key size = " << key_.size << nl;
        }
    }
}

SecurityCache::~SecurityCache()
{
    if (refreshInterval_ > 0) {
        shutdown_->push(true);
        Thread::wait();
        deleteKey(&key_);
    }
}

const NodeConfig *SecurityCache::nodeConfig() const
{
    return nodeConfig_;
}

const LoggingInstance *SecurityCache::errorLoggingInstance() const
{
    return nodeConfig_->errorLoggingInstance();
}

void SecurityCache::deleteKey(gnutls_datum_t *key)
{
    ::memset(key->data, 0, key->size);
    gnutls_free(key->data);
}

void SecurityCache::prepareSessionResumption(gnutls_session_t session)
{
    if (refreshInterval_ <= 0) return;
    Guard<Mutex> guard(mutex_);
    int ret = gnutls_session_ticket_enable_server(session, &key_);
    if (ret != GNUTLS_E_SUCCESS)
        CCNODE_ERROR() << gnutls_strerror(ret) << nl;
}

void SecurityCache::run()
{
    for (double t = System::now() + refreshInterval_; !shutdown_->popBefore(t); t += refreshInterval_)
    {
        Guard<Mutex> guard{mutex_};
        deleteKey(&key_);
        int ret = gnutls_session_ticket_key_generate(&key_);
        if (ret != GNUTLS_E_SUCCESS)
            CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        else
            CCNODE_INFO() << "Session resumption key refreshed, key size = " << key_.size << nl;
    }
}

}} // namespace cc::node
