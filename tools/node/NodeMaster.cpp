/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton.h>
#include <flux/Process.h>
#include <flux/User.h>
#include <flux/IoMonitor.h>
#include <flux/SignalMaster.h>
#include "exceptions.h"
#include "ErrorLog.h"
#include "AccessLog.h"
#include "SystemLog.h"
#include "NodeConfig.h"
#include "WorkerPool.h"
#include "ServiceRegistry.h"
#include "DispatchInstance.h"
#include "ConnectionManager.h"
#include "NodeMaster.h"

namespace fluxnode
{

NodeMaster* nodeMaster() { return Singleton<NodeMaster>::instance(); }

int NodeMaster::run(int argc, char **argv)
{
	SystemLog::open(String(argv[0])->fileName(), 0, LOG_DAEMON);
	nodeConfig()->load(argc, argv);

	if (nodeConfig()->daemon() && !Process::isDaemonized())
		Process::daemonize();

	Thread::blockSignals(SignalSet::createFull());
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
	errorLog()->open(nodeConfig()->errorLogConfig());

	while (true) {
		try {
			runNode();
		}
		catch (Interrupt &ex) {
			if (ex.signal() == SIGINT || ex.signal() == SIGTERM || ex.signal() == SIGHUP) break;
			exitCode_ = ex.signal() + 128;
		}
		catch (Exception &ex) {
			FLUXNODE_ERROR() << ex.message() << nl;
			exitCode_ = 1;
		}
	}
}

void NodeMaster::runNode() const
{
	FLUXNODE_NOTICE() << "Starting (pid = " << Process::currentId() << ")" << nl;

	Ref<DispatchInstance> dispatchInstance;
	for (int i = 0; i < nodeConfig()->serviceInstances()->count(); ++i) {
		if (nodeConfig()->serviceInstances()->at(i)->serviceName() == "Dispatch") {
			dispatchInstance = nodeConfig()->serviceInstances()->at(i);
			nodeConfig()->serviceInstances()->pop(i);
			--i;
		}
	}

	if (!dispatchInstance) {
		ServiceDefinition *dispatchService = serviceRegistry()->serviceByName("Dispatch");
		MetaObject *config = dispatchService->configPrototype();
		dispatchInstance = dispatchService->createInstance(config);
	}

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
		FLUXNODE_WARNING() << "No service configured, falling back to Echo service" << nl;

		ServiceDefinition *echoService = serviceRegistry()->serviceByName("Echo");
		MetaObject *config = echoService->configPrototype();
		config->establish("host", "*");
		Ref<ServiceInstance> echoInstance = echoService->createInstance(config);
		nodeConfig()->serviceInstances()->append(echoInstance);
	}

	typedef List< Ref<StreamSocket> > ListeningSockets;
	Ref<ListeningSockets> listeningSockets = ListeningSockets::create(nodeConfig()->address()->count());

	for (int i = 0; i < nodeConfig()->address()->count(); ++i) {
		SocketAddress *address = nodeConfig()->address()->at(i);
		FLUXNODE_NOTICE() << "Start listening at " << address << nl;
		Ref<StreamSocket> socket = StreamSocket::listen(address);
		listeningSockets->at(i) = socket;
	}

	if (nodeConfig()->user() != "") {
		Ref<User> user = User::lookup(nodeConfig()->user());
		if (!user->exists()) throw UsageError("No such user: \"" + nodeConfig()->user() + "\"");
		FLUXNODE_NOTICE() << "Switching to user " << user->name() << " (uid = " << user->id() << ")" << nl;
		Process::setUserId(user->id());
	}

	Ref<ConnectionManager> connectionManager = ConnectionManager::create(nodeConfig()->serviceWindow());

	dispatchInstance->workerPools_ = WorkerPools::create(nodeConfig()->serviceInstances()->count());
	for (int i = 0; i < nodeConfig()->serviceInstances()->count(); ++i)
		dispatchInstance->workerPools_->at(i) = WorkerPool::create(nodeConfig()->serviceInstances()->at(i), connectionManager->closedConnections());

	Ref<WorkerPool> dispatchPool = WorkerPool::create(dispatchInstance, connectionManager->closedConnections());

	FLUXNODE_NOTICE() << "Up and running (pid = " << Process::currentId() << ")" << nl;

	Ref<IoMonitor> ioMonitor = IoMonitor::create(listeningSockets->count());
	for (int i = 0; i < listeningSockets->count(); ++i)
		ioMonitor->addEvent(listeningSockets->at(i), IoEvent::ReadyAccept);

	FLUXNODE_DEBUG() << "Accepting connections" << nl;

	while (true) {
		Ref<IoActivity> activity = ioMonitor->wait(1);
		if (activity->count() > 0) {
			for (int i = 0; i < activity->count(); ++i) {
				StreamSocket *socket = cast<StreamSocket>(activity->at(i)->stream());
				Ref<StreamSocket> clientSocket = socket->accept();
				Ref<SocketAddress> clientAddress = SocketAddress::create();
				if (!clientSocket->getPeerAddress(clientAddress)) {
					FLUXNODE_DEBUG() << "Failed to get peer address" << nl;
					continue;
				}
				Ref<ClientConnection> client = ClientConnection::create(clientSocket, clientAddress);
				connectionManager->prioritize(client);
				FLUXNODE_DEBUG() << "Accepted connection from " << client->address() << " with priority " << client->priority() << nl;
				dispatchPool->dispatch(client);
			}
		}

		connectionManager->cycle();

		if (signalMaster_->receivedSignals()->count() > 0) {
			int signal = signalMaster_->receivedSignals()->popFront();
			FLUXNODE_NOTICE() << "Received " << signalName(signal) << ", shutting down" << nl;
			dispatchInstance->workerPools_ = 0;
			dispatchPool = 0;
			FLUXNODE_NOTICE() << "Shutdown complete" << nl;
			throw Interrupt(signal);
		}
	}
}

} // namespace fluxnode
