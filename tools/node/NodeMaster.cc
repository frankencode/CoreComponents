/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
#include "exceptions.h"
#include "ErrorLog.h"
#include "AccessLog.h"
#include "SystemLog.h"
#include "NodeConfig.h"
#include "ServiceRegistry.h"
#include "ConnectionManager.h"
#include "HttpSocket.h"
#include "SecurityMaster.h"
#include "NodeMaster.h"

namespace ccnode {

NodeMaster* nodeMaster() { return Singleton<NodeMaster>::instance(); }

int NodeMaster::run(int argc, char **argv)
{
    Thread::blockSignals(SignalSet::createFull());

    SystemLog::open(String(argv[0])->fileName(), 0, LOG_DAEMON);
    nodeConfig()->load(argc, argv);

    if (nodeConfig()->daemon() && !Process::isDaemonized())
        Process::daemonize();

    nodeMaster()->signalMaster_->start();
    nodeMaster()->start();
    nodeMaster()->wait();
    return nodeMaster()->exitCode_;
}

NodeMaster::NodeMaster():
    signalMaster_(SignalMaster::create()),
    exitCode_(0)
{}

void NodeMaster::run()
{
    try {
        errorLog()->open(nodeConfig()->errorLogConfig());
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
            if (ex.signal() == SIGINT || ex.signal() == SIGTERM || ex.signal() == SIGHUP) break;
            exitCode_ = ex.signal() + 128;
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

void NodeMaster::runNode() const
{
    CCNODE_NOTICE() << "Starting (pid = " << Process::currentId() << ")" << nl;

    if (nodeConfig()->directoryPath() != "") {
        ServiceDefinition *directoryService = serviceRegistry()->serviceByName("Directory");
        MetaObject *config = directoryService->configPrototype();
        config->establish("host", "*");
        config->establish("path", nodeConfig()->directoryPath());
        Ref<ServiceInstance> directoryInstance = directoryService->createInstance(config);
        nodeConfig()->serviceInstances()->append(directoryInstance);
    }

    if (nodeConfig()->serviceInstances()->count() == 0)
    {
        CCNODE_WARNING() << "No service configured, falling back to Echo service" << nl;

        ServiceDefinition *echoService = serviceRegistry()->serviceByName("Echo");
        MetaObject *config = echoService->configPrototype();
        config->establish("host", "*");
        Ref<ServiceInstance> echoInstance = echoService->createInstance(config);
        nodeConfig()->serviceInstances()->append(echoInstance);
    }

    typedef List< Ref<HttpSocket> > ListeningSockets;
    Ref<ListeningSockets> listeningSockets = ListeningSockets::create(nodeConfig()->address()->count());

    for (int i = 0; i < nodeConfig()->address()->count(); ++i) {
        SocketAddress *address = nodeConfig()->address()->at(i);
        CCNODE_NOTICE() << "Start listening at " << address << nl;
        listeningSockets->at(i) = HttpSocket::listen(address);
    }

    if (nodeConfig()->user() != "") {
        String userName = nodeConfig()->user();
        String groupName = nodeConfig()->group();
        if (groupName == "") groupName = userName;
        Ref<User> user = User::lookup(userName);
        Ref<Group> group = Group::lookup(groupName);
        if (!user->exists()) throw UsageError("No such user: \"" + userName + "\"");
        if (!group->exists()) throw UsageError("No such group: \"" + groupName + "\"");
        CCNODE_NOTICE() << "Dropping process persona to user:group " << userName << ":" << groupName << " (uid:gid = " << user->id() << ":" << group->id() << ")" << nl;
        Process::setPersona(user->id(), group->id());
    }

    CCNODE_NOTICE() << "Starting security master, if needed" << nl;
    SecurityMaster::start();

    Ref<ConnectionManager> connectionManager = ConnectionManager::create();
    Ref<PendingConnections> pendingConnections = PendingConnections::create();
    Ref<ClosedConnections> closedConnections = connectionManager->closedConnections();

    CCNODE_NOTICE() << "Creating worker pool (concurrency = " << nodeConfig()->concurrency() << ")" << nl;

    typedef Array< Ref<ServiceWorker> > WorkerPool;
    Ref<WorkerPool> workerPool = WorkerPool::create(nodeConfig()->concurrency());
    for (int i = 0; i < workerPool->count(); ++i) {
        Ref<ServiceWorker> worker = ServiceWorker::create(pendingConnections, closedConnections);
        workerPool->at(i) = worker;
        worker->start();
    }

    CCNODE_NOTICE() << "Up and running (pid = " << Process::currentId() << ")" << nl;

    Ref<IoMonitor> ioMonitor = IoMonitor::create(listeningSockets->count());
    for (int i = 0; i < listeningSockets->count(); ++i)
        ioMonitor->addEvent(SystemIo::ReadyAccept, listeningSockets->at(i));

    CCNODE_DEBUG() << "Accepting connections" << nl;

    while (true) {
        Ref<IoActivity> activity = ioMonitor->poll(1000);
        if (activity->count() > 0) {
            for (int i = 0; i < activity->count(); ++i) {
                HttpSocket *socket = cast<HttpSocket>(activity->at(i)->target());
                try {
                    Ref<HttpSocket> clientSocket = socket->accept();
                    Ref<HttpClientConnection> client = HttpClientConnection::open(clientSocket);
                    if (connectionManager->accept(client)) {
                        CCNODE_DEBUG() << "Accepted connection from " << client->address() << " with priority " << client->priority() << nl;
                        pendingConnections->pushBack(client, client->priority());
                    }
                    else {
                        CCNODE_DEBUG() << "Rejected connection from " << client->address() << nl;
                    }
                }
                catch (ProtocolException &ex) {
                    CCNODE_DEBUG() << ex.message() << nl;
                }
                catch (Exception &ex) {
                    CCNODE_DEBUG() << ex.message() << nl;
                }
            }
        }

        connectionManager->cycle();

        while (signalMaster_->receivedSignals()->count() > 0) {
            int signal = signalMaster_->receivedSignals()->popFront();
            if (signal == SIGWINCH || signal == SIGPIPE) continue;
            CCNODE_NOTICE() << "Received " << signalName(signal) << ", shutting down" << nl;
            workerPool = 0;
            CCNODE_NOTICE() << "Shutdown complete" << nl;
            throw Interrupt(signal);
        }
    }
}

} // namespace ccnode