/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsSessionCache>
#include <cc/TlsError>
#include <cc/ReadWriteLock>
#include <cc/Semaphore>
#include <cc/Thread>
#include <cc/System>

namespace cc {

struct TlsSessionCache::State: public Object::State
{
    State(double refreshInterval, Fun<void(const Bytes)> &&onKeyRefresh):
        refreshInterval_{refreshInterval},
        onKeyRefresh_{onKeyRefresh}
    {
        if (refreshInterval_ > 0) {
            generateKey();
            worker_ = Thread{[this]{ run(); }};
            worker_.start();
        }
    }

    ~State()
    {
        if (worker_) {
            shutdown_.release();
            worker_.wait();
            deleteKey();
        }
    }

    void run()
    {
        for (
            double t = System::now() + refreshInterval_;
            !shutdown_.acquireBefore(t);
            t += refreshInterval_
        ) {
            generateKey();
        }
    }

    void generateKey()
    {
        WriteGuard guard{lock_};
        if (keyBytes_) deleteKey();
        int ret = gnutls_session_ticket_key_generate(&key_);
        if (ret != GNUTLS_E_SUCCESS) throw TlsError{ret};
        keyBytes_ = Bytes{};
        keyBytes_.wrapAround(key_.data, key_.size);
        if (onKeyRefresh_) onKeyRefresh_(keyBytes_);
    }

    void deleteKey()
    {
        ::memset(key_.data, 0, key_.size);
        gnutls_free(key_.data);
    }

    void prepareSessionResumption(gnutls_session_t session)
    {
        if (refreshInterval_ <= 0) return;
        ReadGuard guard{lock_};
        int ret = gnutls_session_ticket_enable_server(session, &key_);
        if (ret != GNUTLS_E_SUCCESS) throw TlsError{ret};
    }

    Thread worker_;
    double refreshInterval_;
    Fun<void(const Bytes)> onKeyRefresh_;
    ReadWriteLock lock_;
    Semaphore<int> shutdown_;
    gnutls_datum_t key_;
    Bytes keyBytes_;
};

TlsSessionCache::TlsSessionCache(double refreshInterval, Fun<void(const Bytes)> &&onKeyRefresh):
    Object{new State{refreshInterval, move(onKeyRefresh)}}
{}

void TlsSessionCache::prepareSessionResumption(gnutls_session_t session)
{
    me().prepareSessionResumption(session);
}

TlsSessionCache::State &TlsSessionCache::me()
{
    return Object::me.as<State>();
}

} // namespace cc
