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
			if (ex.signal() == SIGINT || ex.signal() == SIGTERM) break;
			if (ex.signal() == SIGHUP) continue;
			return ex.signal() + 128;
		}
	}

	return 0;
}

void NodeMaster::runNode(int argc, char **argv)
{
	nodeConfig()->load(argc, argv);

	if (nodeConfig()->daemon() && !Process::isDaemonized())
		Process::daemonize();

	errorLog()->open(nodeConfig()->errorLogConfig());
	// accessLog()->open(nodeConfig()->accessLogConfig());

	FLUXNODE_NOTICE() << "Starting..." << nl;

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

	Ref<ConnectionManager> connectionManager = ConnectionManager::create(nodeConfig()->serviceWindow());

	dispatchInstance->workerPools_ = WorkerPools::create(nodeConfig()->serviceInstances()->size());
	for (int i = 0; i < nodeConfig()->serviceInstances()->size(); ++i)
		dispatchInstance->workerPools_->at(i) = WorkerPool::create(nodeConfig()->serviceInstances()->at(i), connectionManager->closedConnections());

	Ref<WorkerPool> dispatchPool = WorkerPool::create(dispatchInstance, connectionManager->closedConnections());

	typedef List< Ref<StreamSocket> > ListeningSockets;
	Ref<ListeningSockets> listeningSockets = ListeningSockets::create(nodeConfig()->address()->size());

	for (int i = 0; i < nodeConfig()->address()->size(); ++i) {
		Ref<StreamSocket> socket = StreamSocket::listen(nodeConfig()->address()->at(i));
		listeningSockets->at(i) = socket;
	}

	if (nodeConfig()->user() != "") {
		Ref<User> user = User::lookup(nodeConfig()->user());
		if (!user->exists()) throw UserError("No such user: \"" + nodeConfig()->user() + "\"");
		Process::setUserId(user->id());
	}

	FLUXNODE_DEBUG() << "Accepting connections with a service window of " << nodeConfig()->serviceWindow() << "s..." << nl;

	try {
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
						Ref<ClientConnection> client = ClientConnection::create(socket->accept());
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
		FLUXNODE_NOTICE() << "Shutting down..." << nl;
		dispatchInstance->workerPools_ = 0;
		throw ex;
	}
}

} // namespace fluxnode
