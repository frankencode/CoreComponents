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

namespace cc { template<class> class Singleton; }

namespace ccnode {

using namespace cc;

class SecurityConfig;

class SecurityMaster: public Thread
{
public:
    static SecurityMaster *instance();
    static void start();
    void prepareSessionResumption(gnutls_session_t session);

private:
    friend class Singleton<SecurityMaster>;

    SecurityMaster();
    ~SecurityMaster();
    virtual void run();
    void deleteKey(gnutls_datum_t *key);

    double refreshInterval_;
    Ref<Mutex> mutex_;
    Ref< Channel<bool> > shutdown_;
    gnutls_datum_t key_;
};

} // namespace ccnode
