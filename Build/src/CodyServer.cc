/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyServer>
#include <cc/ServerSocket>
#include <cc/ClientSocket>
#include <cc/SpinLock>
#include <cc/Thread>
#include <cc/Dir>

namespace cc::build {

struct CodyServer::State final: public Object::State
{
    explicit State(CodyMessage::Flags flags):
        flags_{flags}
    {
        serverAddress_ = listenSocket_.getLocalAddress();
        shutdown_.acquire();

        Dir::establish(cmiCachePath_);
    }

    ~State()
    {
        if (thread_) {
            shutdown_.release();
            ClientSocket{serverAddress_}.waitEstablished();
        }
    }

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void run()
    {
        while (true) {
            StreamSocket clientSocket = listenSocket_.accept();
            if (shutdown_.tryAcquire()) {
                shutdown_.release();
                break;
            }
            // TODO...
        }
        // TODO: exception handling
    }

    const String cmiCachePath_ { "gcm.cache" };
    CodyMessage::Flags flags_;
    ServerSocket listenSocket_ { SocketAddress{ProtocolFamily::InternetV6, "::"} };
    SocketAddress serverAddress_;
    Thread thread_;
    SpinLock shutdown_;
};

} // namespace cc::build
