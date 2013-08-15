/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/IoMonitor.h>
#include <fkit/Thread.h>
#include <fkit/Process.h>
#include <fkit/stdio.h>
#include "exceptions.h"
#include "NodeLog.h"
#include "NodeConfig.h"
#include "WorkerPool.h"
#include "ServiceRegistry.h"
#include "DispatchInstance.h"
#include "NodeMaster.h"

namespace fnode
{

NodeMaster::NodeMaster()
{}

int NodeMaster::run(int argc, char **argv)
{
	Process::enableInterrupt(SIGINT);
	Process::enableInterrupt(SIGHUP);
	Thread::blockSignals(SignalSet::createFull());
	{
		Ref<SignalSet> signalSet = SignalSet::createEmpty();
		signalSet->insert(SIGINT);
		signalSet->insert(SIGHUP);
		Thread::unblockSignals(signalSet);
	}

	while (true) {
		try {
			runNode(argc, argv);
		}
		catch (Interrupt &ex) {
			if (ex.signal() == SIGINT) break;
			if (ex.signal() == SIGHUP) continue;
			return ex.signal() + 128;
		}
	}

	return 0;
}

void NodeMaster::runNode(int argc, char **argv)
{
	nodeConfig()->load(argc, argv);
	nodeLog()->init(nodeConfig()->logLevel());

	status() << "Starting..." << nl;

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
		warning() << "No service configured, falling back to Echo service" << nl;

		ServiceDefinition *echoService = serviceRegistry()->serviceByName("Echo");
		YasonObject *config = echoService->configPrototype();
		config->establish("host", "*");
		Ref<ServiceInstance> echoInstance = echoService->createInstance(config);
		nodeConfig()->serviceInstances()->append(echoInstance);
	}

	dispatchInstance->workerPools_ = WorkerPools::create(nodeConfig()->serviceInstances()->size());
	for (int i = 0; i < nodeConfig()->serviceInstances()->size(); ++i)
		dispatchInstance->workerPools_->at(i) = WorkerPool::create(nodeConfig()->serviceInstances()->at(i));

	Ref<WorkerPool> dispatchPool = WorkerPool::create(dispatchInstance);

	typedef List< Ref<StreamSocket> > ListeningSockets;
	Ref<ListeningSockets> listeningSockets = ListeningSockets::create(nodeConfig()->address()->size());

	for (int i = 0; i < nodeConfig()->address()->size(); ++i) {
		Ref<StreamSocket> socket = StreamSocket::listen(nodeConfig()->address()->at(i));
		listeningSockets->at(i) = socket;
	}

	status() << "Accepting connections..." << nl;

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
						debug() << "Accepted connection from " << client->address() << nl;
						dispatchPool->dispatch(client);
					}
				}
			}
		}
	}
	catch (Interrupt &ex) {
		status() << "Shutting down..." << nl;
		dispatchInstance->workerPools_ = 0;
		throw ex;
	}
}

} // namespace fnode
