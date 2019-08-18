/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/NodeMaster>
#include <cc/node/ErrorLog>
#include <cc/node/AccessLog>
#include <cc/node/SystemLog>
#include <cc/node/DeliveryRegistry>
#include <cc/node/ConnectionManager>
#include <cc/node/HttpServerSocket>
#include <cc/node/SecurityCache>
#include <cc/node/exceptions>
#include <cc/Channel>
#include <cc/Process>
#include <cc/User>
#include <cc/Group>
#include <cc/IoMonitor>

namespace cc {
namespace node {

Ref<NodeMaster> NodeMaster::create(const NodeConfig *config)
{
    return new NodeMaster{config};
}

NodeMaster::NodeMaster(const NodeConfig *config):
    config_{config},
    signals_{Signals::create()},
    exitCode_{0}
{
    if (config->daemon())
        SystemLog::open(config->daemonName(), 0, LOG_DAEMON);
}

void NodeMaster::signaled(Signal signal)
{
    signals_->pushBack(signal);
}

int NodeMaster::exitCode() const
{
    return exitCode_;
}

void NodeMaster::run()
{
    try {
        ErrorLog::instance()->open(config()->errorLogConfig());
    }
    catch (Exception &ex) {
        CCNODE_ERROR() << ex << nl;
        exitCode_ = 1;
        return;
    }

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

    for (SocketAddress *address: config()->address()) {
        CCNODE_NOTICE() << "Start listening at " << address << nl;
        listeningSockets->append(StreamSocket::listen(address));
    }

    if (config()->user() != "") {
        String userName = config()->user();
        String groupName = config()->group();
        if (groupName == "") groupName = userName;
        Ref<User> user = User::lookup(userName);
        Ref<Group> group = Group::lookup(groupName);
        if (!user->isValid()) throw UsageError{"No such user: \"" + userName + "\""};
        if (!group->isValid()) throw UsageError{"No such group: \"" + groupName + "\""};
        CCNODE_NOTICE() << "Dropping process persona to user:group " << userName << ":" << groupName << " (uid:gid = " << user->id() << ":" << group->id() << ")" << nl;
        Process::setUserId(user->id());
        Process::setGroupId(group->id());
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

    while (true) {
        Ref<IoActivity> activity = ioMonitor->wait(1000);
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

}} // namespace cc::node
