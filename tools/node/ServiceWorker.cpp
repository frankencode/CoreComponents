/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Format.h>
#include <flux/RefGuard.h>
#include <flux/System.h>
#include <flux/TimeoutLimiter.h>
#include "exceptions.h"
#include "ErrorLog.h"
#include "AccessLog.h"
#include "VisitLog.h"
#include "NodeConfig.h"
#include "ServiceDefinition.h"
#include "ServiceDelegate.h"
#include "Response.h"
#include "ServiceWorker.h"

namespace fluxnode
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

void ServiceWorker::logDelivery(ClientConnection *client, int statusCode, size_t bytesWritten)
{
	Stream *stream = accessLog()->noticeStream();
	if (400 <= statusCode && statusCode <= 499) stream = accessLog()->warningStream();
	else if (500 <= statusCode) stream = accessLog()->errorStream();

	Request *request = client->request();
	String requestLine = request ? request->line() : "INVALID";
	double requestTime = request ? request->time() : System::now();
	String userAgent =   request ? request->value("User-Agent") : "";

	Format(stream)
		<< client->address()->networkAddress() << " "
		<< Date::create(requestTime)->toString() << " "
		<< "\"" << requestLine << "\" "
		<< statusCode << " " << bytesWritten << " "
		<< "\"" << userAgent << "\" "
		<< client->priority()
		<< nl;
}

void ServiceWorker::logVisit(Visit *visit)
{
	Stream *stream = (visit->priority() == 0) ? visitLog()->noticeStream() : visitLog()->infoStream();
	Format(stream) << visit->remoteAddress()->networkAddress() << " " << fixed(visit->departureTime() - visit->arrivalTime(), 3) << " " << visit->priority() << nl;
}

void ServiceWorker::run()
{
	errorLog()->open(serviceInstance_->errorLogConfig());
	accessLog()->open(serviceInstance_->accessLogConfig());
	visitLog()->open(serviceInstance_->visitLogConfig());

	while (pendingConnections_->pop(&client_))
	{
		Ref<Visit> visit;

		try {
			if (serviceInstance_->connectionTimeout() > 0) {
				FLUXNODE_DEBUG() << "Establishing connection timeout of " << serviceInstance_->connectionTimeout() << "s..." << nl;
				client_->setupTimeout(serviceInstance_->connectionTimeout());
			}
			while (client_) {
				FLUXNODE_DEBUG() << "Reading request..." << nl;
				Ref<Request> request = client_->readRequest();
				{
					RefGuard<Response> guard(&response_);
					response_ = Response::create(client_);
					serviceDelegate_->process(request);
					response_->end();
					if (response_->delivered()) {
						visit = client_->visit();
						logDelivery(client_, response_->statusCode(), response_->bytesWritten());
						if (!client_->isPayloadConsumed())
							close();
					}
					else {
						close();
					}
				}
				if (client_) {
					if (client_ && request->value("Connection") == "close")
						close();
				}
			}
		}
		catch (ProtocolException &ex) {
			Format("HTTP/1.1 %% %%\r\n\r\n", client_->stream()) << ex.statusCode() << " " << ex.message();
			logDelivery(client_, ex.statusCode());
		}
		#ifdef NDEBUG
		catch (Exception &ex) {
			FLUXNODE_ERROR() << ex.message() << nl;
			Format("HTTP/1.1 500 Internal Server Error: %%\r\n\r\n", client_->stream()) << ex.message();
			logDelivery(client_, 500);
		}
		#endif
		catch (TimeoutExceeded &) { FLUXNODE_DEBUG() << "Connection timed out (" << client_->address() << ")" << nl; }
		catch (CloseRequest &) {}
		close();

		if (visit) {
			visit->updateDepartureTime();
			logVisit(visit);
			closedConnections_->push(visit);
		}
	}
}

Response *ServiceWorker::response() const
{
	return response_;
}

void ServiceWorker::close()
{
	if (client_) {
		FLUXNODE_DEBUG() << "Closing connection to " << client_->address() << nl;
		client_ = 0;
	}
}

} // namespace fluxnode
