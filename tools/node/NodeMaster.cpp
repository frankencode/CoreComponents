/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/IoMonitor.h>
#include <flux/Thread.h>
#include <flux/Process.h>
#include <flux/User.h>
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

NodeMaster::NodeMaster()
{}

int NodeMaster::run(int argc, char **argv)
{
	SystemLog::open(String(argv[0])->fileName(), 0, LOG_DAEMON);

	Process::enableInterrupt(SIGINT);
	Process::enableInterrupt(SIGTERM);
	Process::enableInterrupt(SIGHUP);
	Thread::blockSignals(SignalSet::createFull());
	{
		Ref<SignalSet> signalSet = SignalSet::createEmpty();
		signalSet->insert(SIGINT);
		signalSet->insert(SIGTERM);
		signalSet->insert(SIGHUP);
		Thread::unblockSignals(signalSet);
	}

	while (true) {
		try {
			runNode(argc, argv);
		}
		catch (Interrupt &ex) {
			if (ex.signal() == SIGINT || ex.signal() == SIGTERM || ex.signal() == SIGHUP) break;
			return ex.signal() + 128;
		}
		#ifdef NDEBUG
		catch (SystemError &ex) {
			return 1;
		}
		#endif
	}

	return 0;
}

void NodeMaster::runNode(int argc, char **argv)
{
	nodeConfig()->load(argc, argv);

	if (nodeConfig()->daemon() && !Process::isDaemonized())
		Process::daemonize();

	errorLog()->open(nodeConfig()->errorLogConfig());

	#ifdef NDEBUG
	try {
		runNode();
	}
	catch (Exception &ex) {
		FLUXNODE_ERROR() << ex.message() << nl;
		throw;
	}
	#else
	runNode();
	#endif
}

void NodeMaster::runNode()
{
	FLUXNODE_NOTICE() << "Starting (pid = " << Process::currentId() << ")" << nl;

	Ref<DispatchInstance> dispatchInstance;
	for (int i = 0; i < nodeConfig()->serviceInstances()->size(); ++i) {
		if (nodeConfig()->serviceInstances()->at(i)->serviceName() == "Dispatch") {
			dispatchInstance = nodeConfig()->serviceInstances()->at(i);
			nodeConfig()->serviceInstances()->pop(i);
			--i;
		}
	}

	if (!dispatchInstance) {
		ServiceDefinition *dispatchService = serviceRegistry()->serviceByName("Dispatch");
		YasonObject *config = dispatchService->configPrototype();
		dispatchInstance = dispatchService->createInstance(config);
	}

	if (nodeConfig()->serviceInstances()->size() == 0)
	{
		FLUXNODE_WARNING() << "No service configured, falling back to Echo service" << nl;

		ServiceDefinition *echoService = serviceRegistry()->serviceByName("Echo");
		YasonObject *config = echoService->configPrototype();
		config->establish("host", "*");
		Ref<ServiceInstance> echoInstance = echoService->createInstance(config);
		nodeConfig()->serviceInstances()->append(echoInstance);
	}

	typedef List< Ref<StreamSocket> > ListeningSockets;
	Ref<ListeningSockets> listeningSockets = ListeningSockets::create(nodeConfig()->address()->size());

	for (int i = 0; i < nodeConfig()->address()->size(); ++i) {
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

	dispatchInstance->workerPools_ = WorkerPools::create(nodeConfig()->serviceInstances()->size());
	for (int i = 0; i < nodeConfig()->serviceInstances()->size(); ++i)
		dispatchInstance->workerPools_->at(i) = WorkerPool::create(nodeConfig()->serviceInstances()->at(i), connectionManager->closedConnections());

	Ref<WorkerPool> dispatchPool = WorkerPool::create(dispatchInstance, connectionManager->closedConnections());

	FLUXNODE_NOTICE() << "Up and running (pid = " << Process::currentId() << ")" << nl;

	try {
		FLUXNODE_DEBUG() << "Accepting connections" << nl;
		Ref<IoMonitor> ioMonitor = IoMonitor::create();
		while (true) {
			ioMonitor->readyAccept()->clear();
			for (int i = 0; i < listeningSockets->size(); ++i)
				ioMonitor->readyAccept()->insert(listeningSockets->at(i));
			int n = ioMonitor->wait(1);
			if (n > 0) {
				for (int i = 0; i < listeningSockets->size(); ++i) {
					StreamSocket *socket = listeningSockets->at(i);
					if (ioMonitor->readyAccept()->contains(socket)) {
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
			}
			connectionManager->cycle();
		}
	}
	catch (Interrupt &ex) {
		FLUXNODE_NOTICE() << "Received " << ex.signalName() << ", shutting down" << nl;
		dispatchInstance->workerPools_ = 0;
		dispatchPool = 0;
		FLUXNODE_NOTICE() << "Shutdown complete" << nl;
		throw ex;
	}
}

} // namespace fluxnode
