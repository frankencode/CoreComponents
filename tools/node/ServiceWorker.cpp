/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Memory.h>
#include <fkit/Format.h>
#include <fkit/RefGuard.h>
#include <fkit/stdio.h> // DEBUG
#include "exceptions.h"
#include "NodeLog.h"
#include "NodeConfig.h"
#include "ServiceDefinition.h"
#include "ServiceDelegate.h"
#include "Response.h"
#include "ServiceWorker.h"

namespace fnode
{

Ref<ServiceWorker> ServiceWorker::create(ServiceInstance *serviceInstance, ClosedConnections *closedConnections)
{
	Ref<ServiceWorker> worker = new ServiceWorker(serviceInstance, closedConnections);
	worker->serviceDelegate_ = serviceInstance->createDelegate(worker);
	return worker;
}

ServiceWorker::ServiceWorker(ServiceInstance *serviceInstance, ClosedConnections *closedConnections)
	: serviceInstance_(serviceInstance),
	  pendingConnections_(PendingConnections::create()),
	  closedConnections_(closedConnections)
{}

ServiceWorker::~ServiceWorker()
{
	pendingConnections_->push(Ref<ClientConnection>());
	wait();
}

void ServiceWorker::run()
{
	while (pendingConnections_->pop(&client_)) {
		try {
			while (client_) {
				if (!client_->request_) {
					debug() << "Reading request header..." << nl;
					client_->request_ = Request::parse(client_);
					if ( (client_->request_->method() != "GET") &&
					     (client_->request_->method() != "HEAD") ) throw NotImplemented();
				}
				{
					RefGuard<Response> guard(&response_);
					response_ = Response::create(client_->stream_);
					serviceDelegate_->process(client_->request_);
				}
				if (client_) {
					if (client_->request_->value("Connection") == "close")
						client_ = 0;
					else
						client_->request_ = 0;
				}
			}
		}
		catch (ProtocolException &ex) {
			Format("HTTP/1.1 %% %%\r\n\r\n", client_->stream_) << ex.statusCode() << " " << ex.message();
		}
		catch (Exception &ex) {
			debug() << ex.message() << nl;
			Format("HTTP/1.1 500 Internal Server Error: %%\r\n\r\n", client_->stream_) << ex.message();
		}
		client_ = 0;
		// closedConnections_->push(clientAddress); // TODO
	}
}

Response *ServiceWorker::response() const
{
	return response_;
}

void ServiceWorker::close()
{
	client_ = 0;
}

} // namespace fnode
