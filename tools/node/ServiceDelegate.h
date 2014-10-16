/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SERVICEDELEGATE_H
#define FLUXNODE_SERVICEDELEGATE_H

#include <flux/Format>
#include "Request.h"

namespace fluxnode
{

class ServiceWorker;
class ServiceInstance;
class ClientConnection;

class ServiceDelegate: public Object
{
public:
	virtual void process(Request *request) = 0;

protected:
	ServiceDelegate(ServiceWorker *worker);

	ServiceInstance *serviceInstance() const;
	ClientConnection *client() const;

	void status(int statusCode, String reasonPhrase = "");
	void header(String name, String value);
	void begin(ssize_t contentLength = -1);
	void write(String bytes);
	Format chunk(String pattern);
	Format chunk();
	void end();
	void close();

private:
	ServiceWorker *worker_;
};

} // namespace fluxnode

#endif // FLUXNODE_SERVICEDELEGATE_H
