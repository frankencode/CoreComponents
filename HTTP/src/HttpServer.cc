/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpServer>
#include <cc/HttpServiceRegistry>
#include <cc/HttpConnectionManager>
#include <cc/HttpServiceWorker>
#include <cc/httpDebug>
#include <cc/PluginLoader>
#include <cc/TlsSessionCache>
#include <cc/ServerSocket>
#include <cc/IoMonitor>
#include <cc/Thread>
#include <cc/Channel>
#include <cc/Process>
#include <cc/UserInfo>
#include <cc/File>
#include <cc/str>

namespace cc {

CC_PLUGIN_TARGET

void HttpServer::loadPlugins()
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        PluginLoader{pluginTarget(), { "delivery", "logging" }};
    }
}

struct HttpServer::State: public Object::State
{
    explicit State(const String &config):
        State{HttpServerConfig{config}}
    {}

    explicit State(const HttpServerConfig &config):
        nodeConfig_{config}
    {
        if (config.pidPath() != "" || config.daemon()) {
            String path = config.pidPath();
            if (path == "") path = "/var/run/" + config.daemonName() + ".pid";
            File::save(path, str(Process::currentId()) + "\n");
        }
    }

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void wait()
    {
        if (thread_) thread_.wait();
    }

    void run()
    {
        while (true) {
            try {
                runNode();
            }
            catch (Signaled &ex) {
                if (ex.signal() != Signal::HangUp) {
                    if (ex.signal() == Signal::Interrupt || ex.signal() == Signal::Terminate)
                        exitCode_ = 0;
                    else
                        exitCode_ = +ex.signal() + 128;
                    break;
                }
            }
            catch (Exception &ex) {
                CCNODE_ERROR() << ex << nl;
                exitCode_ = 1;
                break;
            }
        }
    }

    void runNode()
    {
        List<ServerSocket> listeningSockets;

        for (const SocketAddress &address: nodeConfig_.address()) {
            CCNODE_NOTICE() << "Start listening at " << address << nl;
            listeningSockets.append(ServerSocket{address});
        }

        if (nodeConfig_.user() != "") {
            String userName = nodeConfig_.user();
            UserInfo user{userName};
            if (!user) throw UsageError{"No such user: \"" + userName + "\""};
            CCNODE_NOTICE() << "Dropping to user " << userName << " (uid = " << user.id() << ")" << nl;
            Process::setUserId(user.id());
        }

        const double tlsRefreshInterval = nodeConfig_.tlsOptions().sessionResumptionKeyRefresh();
        if (tlsRefreshInterval > 0) {
            CCNODE_NOTICE() << "TLS session resumption key refresh interval set to " << tlsRefreshInterval << "s" << nl;
        }
        TlsSessionCache sessionCache{tlsRefreshInterval,
            [this](const Bytes &key){
                CCNODE_INFO() << "TLS session resumption key refreshed: key = 0x" << hex(key) << nl;
            }
        };


        HttpConnectionManager connectionManager{nodeConfig_};
        PendingConnections pendingConnections;
        ClosedConnections closedConnections = connectionManager.closedConnections();

        CCNODE_NOTICE() << "Creating worker pool (concurrency = " << nodeConfig_.concurrency() << ")" << nl;

        Array<HttpServiceWorker> workerPool = Array<HttpServiceWorker>::allocate(nodeConfig_.concurrency());
        for (HttpServiceWorker &worker: workerPool) {
            worker = HttpServiceWorker{nodeConfig_, pendingConnections, closedConnections, sessionCache};
            worker.start();
        }

        IoMonitor ioMonitor;
        for (const StreamSocket &socket: listeningSockets) {
            ioMonitor.watch(socket, IoEvent::ReadyAccept);
        }

        CCNODE_DEBUG() << "Accepting connections" << nl;

        for (const StreamSocket &socket: listeningSockets)
            startedChannel_.pushBack(socket.address());

        while (true) {
            ioMonitor.wait(
                [&](const IoActivity &activity){
                    try {
                        ServerSocket serverSocket = activity.target().as<ServerSocket>();
                        StreamSocket stream = serverSocket.accept();
                        stream.setIncomingTimeout(nodeConfig_.connectionTimeout());
                        HttpClientConnection client{stream, serverSocket.address()};
                        if (connectionManager.accept(&client)) {
                            CCNODE_DEBUG() << "Accepted connection from " << client.peerAddress() << " with priority " << client.priority() << nl;
                            pendingConnections.pushBack(client, client.priority());
                        }
                        else {
                            CCNODE_DEBUG() << "Rejected connection from " << client.peerAddress() << nl;
                        }
                    }
                    catch (Exception &ex) {
                        CCNODE_ERROR() << ex << nl;
                    }
                },
                1000
            );

            connectionManager.cycle();

            while (signals_.count() > 0) {
                Signal signal { Signal::Undefined };
                signals_.popFront(&signal);
                if (signal == Signal::Interrupt || signal == Signal::Terminate || signal == Signal::HangUp) {
                    CCNODE_NOTICE() << "Received " << signal << ", shutting down ..." << nl;
                    workerPool.deplete();
                    CCNODE_NOTICE() << "Shutdown complete" << nl;
                    throw Signaled{signal};
                }
            }
        }
    }

    const HttpLoggingServiceInstance &errorLoggingInstance() const { return nodeConfig_.errorLoggingInstance(); }

    Thread thread_;
    HttpServerConfig nodeConfig_;
    Channel<SocketAddress> startedChannel_;
    Channel<Signal> signals_;
    int exitCode_ { 0 };
};

HttpServer::HttpServer(const String &config):
    Object{new State{config}}
{}

HttpServer::HttpServer(const HttpServerConfig &config):
    Object{new State{config}}
{}

void HttpServer::start()
{
    me().start();
}

SocketAddress HttpServer::waitStarted()
{
    SocketAddress address;
    me().startedChannel_.popFront(&address);
    return address;
}

void HttpServer::wait()
{
    me().wait();
}

void HttpServer::shutdown()
{
    me().signals_.pushBack(Signal::Terminate);
}

Channel<Signal> &HttpServer::signals()
{
    return me().signals_;
}

int HttpServer::exitCode() const
{
    return me().exitCode_;
}

HttpServer::State &HttpServer::me()
{
    return Object::me.as<State>();
}

const HttpServer::State &HttpServer::me() const
{
    return Object::me.as<State>();
}

}
