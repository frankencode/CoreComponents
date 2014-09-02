/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format.h>
#include <flux/RefGuard.h>
#include <flux/System.h>
#include <flux/stream/TimeoutLimiter.h>
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

using namespace flux::stream;

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
	if (400 <= statusCode && statusCode <= 499) stream = accessLog()->debugStream();
	else if (500 <= statusCode) stream = accessLog()->errorStream();

	Request *request = client->request();
	String requestHost = request ? request->host() : "";
	String requestLine = request ? request->line() : "";
	double requestTime = request ? request->time() : System::now();
	String userAgent =   request ? request->value("User-Agent") : "";

	Format(stream)
		<< client->address()->networkAddress() << " "
		<< Date::create(requestTime)->toString() << " "
		<< "\"" << requestHost << "\" "
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

	while (pendingConnections_->waitNext(&client_))
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
					// response_->insert("Keep-Alive", Format("timeout=%%, max=100000") << serviceInstance()->connectionTimeout());
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
					if (client_ && request->value("Connection")->equalsCaseInsensitive("close"))
						close();
				}
			}
		}
		catch (ProtocolException &ex) {
			Format("HTTP/1.1 %% %%\r\n\r\n", client_->stream()) << ex.statusCode() << " " << ex.message();
			logDelivery(client_, ex.statusCode());
		}
		catch (TimeoutExceeded &) {
			FLUXNODE_DEBUG() << "Connection timed out (" << client_->address() << ")" << nl;
			Format("HTTP/1.1 408 Request Timeout\r\n\r\n", client_->stream());
			logDelivery(client_, 408);
		}
		catch (ConnectionResetByPeer &)
		{}
		catch (CloseRequest &)
		{}
		catch (Exception &ex) {
			FLUXNODE_ERROR() << ex.message() << nl;
			// Format("HTTP/1.1 500 Internal Server Error: %%\r\n\r\n", client_->stream()) << ex.message();
			// logDelivery(client_, 500);
		}
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
		pendingConnections_->popFront();
		client_ = 0;
	}
}

} // namespace fluxnode
