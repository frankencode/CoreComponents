/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SERVICEDELEGATE_H
#define FNODE_SERVICEDELEGATE_H

#include <flux/Format.h>
#include "Request.h"

namespace fnode
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

} // namespace fnode

#endif // FNODE_SERVICEDELEGATE_H
