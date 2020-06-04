/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/NodeMaster>
#include <cc/http/SystemLoggingService>
#include <cc/http/DeliveryRegistry>
#include <cc/http/ConnectionManager>
#include <cc/http/HttpServerSocket>
#include <cc/http/SecurityCache>
#include <cc/http/exceptions>
#include <cc/http/debug>
#include <cc/Channel>
#include <cc/Process>
#include <cc/File>
#include <cc/User>
#include <cc/Group>
#include <cc/IoMonitor>

namespace cc {
namespace http {

Ref<NodeMaster> NodeMaster::create(const String &config)
{
    return new NodeMaster{NodeConfig::load(NodeConfig::parse(config))};
}

Ref<NodeMaster> NodeMaster::create(const NodeConfig *config)
{
    return new NodeMaster{config};
}

NodeMaster::NodeMaster(const NodeConfig *config):
    config_{config},
    startedChannel_{StartedChannel::create()},
    signals_{Signals::create()},
    exitCode_{0}
{
    if (config->daemon())
        SystemLoggingService::open(config->daemonName());

    if (config->pidPath() != "" || config->daemon()) {
        String path = config->pidPath();
        if (path == "") path = "/var/run/" + config->daemonName() + ".pid";
        File::save(path, str(Process::getId()) + "\n" );
    }
}

SocketAddress NodeMaster::waitStarted()
{
    return startedChannel_->pop();
}

void NodeMaster::sendSignal(Signal signal)
{
    signals_->pushBack(signal);
}

int NodeMaster::exitCode() const
{
    return exitCode_;
}

void NodeMaster::run()
{
    while (true) {
        try {
            runNode();
        }
        catch (Signaled &ex) {
            if (+ex->signal() != SIGHUP) {
                if (+ex->signal() == SIGINT || +ex->signal() == SIGTERM)
                    exitCode_ = 0;
                else
                    exitCode_ = +ex->signal() + 128;
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

void NodeMaster::runNode()
{
    typedef List< Ref<StreamSocket> > ListeningSockets;
    Ref<ListeningSockets> listeningSockets = ListeningSockets::create();

    for (const SocketAddress &address: config()->address()) {
        auto socket = StreamSocket::listen(address);
        CCNODE_NOTICE() << "Start listening at " << socket->address() << nl;
        listeningSockets->append(socket);
    }

    if (config()->user() != "") {
        String userName = config()->user();
        User user{userName};
        if (!user->isValid()) throw UsageError{"No such user: \"" + userName + "\""};
        CCNODE_NOTICE() << "Dropping to user " << userName << " (uid = " << user->id() << ")" << nl;
        Process::setUserId(user->id());
    }

    CCNODE_NOTICE() << "Starting security master, if needed" << nl;
    Ref<SecurityCache> securityCache = SecurityCache::start(config());

    Ref<ConnectionManager> connectionManager = ConnectionManager::create(config());
    Ref<PendingConnections> pendingConnections = PendingConnections::create();
    Ref<ClosedConnections> closedConnections = connectionManager->closedConnections();

    CCNODE_NOTICE() << "Creating worker pool (concurrency = " << config()->concurrency() << ")" << nl;

    typedef Array< Ref<DeliveryWorker> > WorkerPool;
    Ref<WorkerPool> workerPool = WorkerPool::create(config()->concurrency());
    for (Ref<DeliveryWorker> &worker: workerPool) {
        worker = DeliveryWorker::create(config(), pendingConnections, closedConnections);
        worker->start();
    }

    Ref<IoMonitor> ioMonitor = IoMonitor::create(listeningSockets->count());
    for (StreamSocket *socket: listeningSockets)
        ioMonitor->addEvent(IoReady::Accept, socket);

    CCNODE_DEBUG() << "Accepting connections" << nl;

    for (StreamSocket *socket: listeningSockets)
        startedChannel_->push(socket->address());

    while (true) {
        IoActivity activity = ioMonitor->wait(1000);
        for (const IoEvent *event: activity) {
            try {
                StreamSocket *listeningSocket = Object::cast<StreamSocket *>(event->target());
                Ref<HttpServerSocket> clientSocket = HttpServerSocket::accept(listeningSocket, config(), securityCache);
                Ref<HttpServerConnection> clientConnection = HttpServerConnection::open(clientSocket);
                if (connectionManager->accept(clientConnection)) {
                    CCNODE_DEBUG() << "Accepted connection from " << clientConnection->address() << " with priority " << clientConnection->priority() << nl;
                    pendingConnections->pushBack(clientConnection, clientConnection->priority());
                }
                else {
                    CCNODE_DEBUG() << "Rejected connection from " << clientConnection->address() << nl;
                }
            }
            catch (Exception &ex) {
                CCNODE_ERROR() << ex << nl;
            }
        }

        connectionManager->cycle();

        while (signals_->count() > 0) {
            Signal signal = signals_->popFront();
            if (+signal == SIGINT || +signal == SIGTERM || +signal == SIGHUP) {
                CCNODE_NOTICE() << "Received " << signal << ", shutting down" << nl;
                workerPool = nullptr;
                CCNODE_NOTICE() << "Shutdown complete" << nl;
                throw Signaled{signal};
            }
        }
    }
}

}} // namespace cc::http
