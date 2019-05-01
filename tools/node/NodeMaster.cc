/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Process>
#include <cc/User>
#include <cc/Group>
#include <cc/IoMonitor>
#include <cc/SignalMaster>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "AccessLog.h"
#include "SystemLog.h"
#include "NodeConfig.h"
#include "ServiceRegistry.h"
#include "ConnectionManager.h"
#include "HttpServerSocket.h"
#include "SecurityMaster.h"
#include "NodeMaster.h"

namespace ccnode {

using namespace cc::http;

NodeMaster* nodeMaster() { return Singleton<NodeMaster>::instance(); }

int NodeMaster::run(int argc, char **argv)
{
    Thread::blockSignals(SignalSet::createFull());

    SystemLog::open(String{argv[0]}->fileName(), 0, LOG_DAEMON);
    NodeConfig::instance()->load(argc, argv);

    if (NodeConfig::instance()->daemon() && !Process::isDaemonized())
        Process::daemonize();

    auto signalMaster = SignalMaster::start([=](int signal, bool *fin){
        nodeMaster()->signals_->pushBack(signal);
        *fin = (signal == SIGINT || signal == SIGTERM);
    });

    nodeMaster()->start();
    nodeMaster()->wait();

    signalMaster->wait();

    return nodeMaster()->exitCode_;
}

NodeMaster::NodeMaster():
    signals_{Signals::create()},
    exitCode_{0}
{}

void NodeMaster::run()
{
    try {
        ErrorLog::instance()->open(NodeConfig::instance()->errorLogConfig());
    }
    catch (Exception &ex) {
        CCNODE_ERROR() << ex.message() << nl;
        exitCode_ = 1;
        return;
    }

    while (true) {
        try {
            runNode();
        }
        catch (Interrupt &ex) {
            if (ex.signal() != SIGHUP) {
                exitCode_ = ex.signal() + 128;
                break;
            }
        }
        #ifdef NDEBUG
        catch (Exception &ex) {
            CCNODE_ERROR() << ex.message() << nl;
            exitCode_ = 1;
            break;
        }
        #endif
    }
}

void NodeMaster::runNode()
{
    CCNODE_NOTICE() << "Starting (pid = " << Process::currentId() << ")" << nl;

    if (NodeConfig::instance()->directoryPath() != "") {
        ServiceDefinition *directoryService = serviceRegistry()->serviceByName("Directory");
        MetaObject *config = directoryService->configPrototype();
        config->establish("host", "*");
        config->establish("path", NodeConfig::instance()->directoryPath());
        Ref<ServiceInstance> directoryInstance = directoryService->createInstance(config);
        NodeConfig::instance()->serviceInstances()->append(directoryInstance);
    }

    if (NodeConfig::instance()->serviceInstances()->count() == 0)
    {
        CCNODE_WARNING() << "No service configured, falling back to Echo service" << nl;

        ServiceDefinition *echoService = serviceRegistry()->serviceByName("Echo");
        MetaObject *config = echoService->configPrototype();
        config->establish("host", "*");
        Ref<ServiceInstance> echoInstance = echoService->createInstance(config);
        NodeConfig::instance()->serviceInstances()->append(echoInstance);
    }

    typedef List< Ref<StreamSocket> > ListeningSockets;
    Ref<ListeningSockets> listeningSockets = ListeningSockets::create();

    for (SocketAddress *address: NodeConfig::instance()->address()) {
        CCNODE_NOTICE() << "Start listening at " << address << nl;
        listeningSockets->append(StreamSocket::listen(address));
    }

    if (NodeConfig::instance()->user() != "") {
        String userName = NodeConfig::instance()->user();
        String groupName = NodeConfig::instance()->group();
        if (groupName == "") groupName = userName;
        Ref<User> user = User::lookup(userName);
        Ref<Group> group = Group::lookup(groupName);
        if (!user->isValid()) throw UsageError("No such user: \"" + userName + "\"");
        if (!group->isValid()) throw UsageError("No such group: \"" + groupName + "\"");
        CCNODE_NOTICE() << "Dropping process persona to user:group " << userName << ":" << groupName << " (uid:gid = " << user->id() << ":" << group->id() << ")" << nl;
        Process::setPersona(user->id(), group->id());
    }

    CCNODE_NOTICE() << "Starting security master, if needed" << nl;
    SecurityMaster::start();

    Ref<ConnectionManager> connectionManager = ConnectionManager::create();
    Ref<PendingConnections> pendingConnections = PendingConnections::create();
    Ref<ClosedConnections> closedConnections = connectionManager->closedConnections();

    CCNODE_NOTICE() << "Creating worker pool (concurrency = " << NodeConfig::instance()->concurrency() << ")" << nl;

    typedef Array< Ref<ServiceWorker> > WorkerPool;
    Ref<WorkerPool> workerPool = WorkerPool::create(NodeConfig::instance()->concurrency());
    for (Ref<ServiceWorker> &worker: workerPool) {
        worker = ServiceWorker::create(pendingConnections, closedConnections);
        worker->start();
    }

    CCNODE_NOTICE() << "Up and running (pid = " << Process::currentId() << ")" << nl;

    Ref<IoMonitor> ioMonitor = IoMonitor::create(listeningSockets->count());
    for (StreamSocket *socket: listeningSockets)
        ioMonitor->addEvent(IoReady::Accept, socket);

    CCNODE_DEBUG() << "Accepting connections" << nl;

    while (true) {
        Ref<IoActivity> activity = ioMonitor->wait(1000);
        for (const IoEvent *event: activity) {
            try {
                StreamSocket *listeningSocket = Object::cast<StreamSocket *>(event->target());
                Ref<HttpServerSocket> clientSocket = HttpServerSocket::accept(listeningSocket);
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
            int signal = signals_->popFront();
            if (signal == SIGINT || signal == SIGTERM || signal == SIGHUP) {
                CCNODE_NOTICE() << "Received " << signalName(signal) << ", shutting down" << nl;
                workerPool = nullptr;
                CCNODE_NOTICE() << "Shutdown complete" << nl;
                throw Interrupt(signal);
            }
        }
    }
}

} // namespace ccnode
