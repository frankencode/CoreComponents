/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <string.h>
#include <cc/Guard>
#include <cc/System>
#include <cc/Singleton>
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "SecurityMaster.h"

namespace ccnode {

SecurityMaster *SecurityMaster::instance()
{
    return Singleton<SecurityMaster>::instance();
}

void SecurityMaster::start()
{
    SecurityMaster *instance = SecurityMaster::instance();
    if (instance->refreshInterval_ > 0)
        instance->Thread::start();
}

SecurityMaster::SecurityMaster():
    refreshInterval_{NodeConfig::instance()->security()->sessionResumptionKeyRefresh()},
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

SecurityMaster::~SecurityMaster()
{
    shutdown_->push(true);
    Thread::wait();
    deleteKey(&key_);
}

void SecurityMaster::deleteKey(gnutls_datum_t *key)
{
    ::memset(key->data, 0, key->size);
    gnutls_free(key->data);
}

void SecurityMaster::prepareSessionResumption(gnutls_session_t session)
{
    if (refreshInterval_ <= 0) return;
    Guard<Mutex> guard(mutex_);
    int ret = gnutls_session_ticket_enable_server(session, &key_);
    if (ret != GNUTLS_E_SUCCESS)
        CCNODE_ERROR() << gnutls_strerror(ret) << nl;
}

void SecurityMaster::run()
{
    ErrorLog::instance()->open(NodeConfig::instance()->errorLogConfig());

    for (double t = System::now() + refreshInterval_; !shutdown_->popBefore(t); t += refreshInterval_)
    {
        Guard<Mutex> guard(mutex_);
        deleteKey(&key_);
        int ret = gnutls_session_ticket_key_generate(&key_);
        if (ret != GNUTLS_E_SUCCESS)
            CCNODE_ERROR() << gnutls_strerror(ret) << nl;
        else
            CCNODE_INFO() << "Session resumption key refreshed, key size = " << key_.size << nl;
    }
}

} // namespace ccnode
