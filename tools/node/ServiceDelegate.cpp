/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Response.h"
#include "ServiceWorker.h"
#include "ServiceDelegate.h"

namespace fnode
{

ServiceDelegate::ServiceDelegate(ServiceWorker *worker)
	: worker_(worker)
{}

ServiceInstance *ServiceDelegate::serviceInstance() const
{
	return worker_->serviceInstance();
}

ClientConnection *ServiceDelegate::client() const
{
	return worker_->client();
}

void ServiceDelegate::header(String name, String value)
{
	worker_->response()->header(name, value);
}

void ServiceDelegate::begin()
{
	worker_->response()->begin();
}

void ServiceDelegate::write(String bytes)
{
	worker_->response()->write(bytes);
}

Format ServiceDelegate::chunk(String pattern)
{
	return worker_->response()->chunk(pattern);
}

Format ServiceDelegate::chunk()
{
	return worker_->response()->chunk();
}

void ServiceDelegate::end()
{
	worker_->response()->end();
}

void ServiceDelegate::close()
{
	worker_->close();
}

bool ServiceDelegate::readyRead(double interval) const
{
	return worker_->client()->request()->payload()->readyRead(interval);
}

int ServiceDelegate::readAvail(ByteArray *buf)
{
	return worker_->client()->request()->payload()->readAvail(buf);
}

} // namespace fnode
