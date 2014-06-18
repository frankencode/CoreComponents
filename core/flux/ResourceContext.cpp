/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ThreadLocalSingleton.h"
#include "Queue.h"
#include "ResourceContext.h"

namespace flux
{

ResourceContext::ResourceContext()
	: queue_(Queue<String>::create())
{}

void ResourceContext::push(String resource)
{
	queue_->pushBack(resource);
}

String ResourceContext::pop()
{
	if (queue_->count() == 0) return String();
	return queue_->popBack();
}

String ResourceContext::top() const
{
	return queue_->count() > 0 ? queue_->back() : String();
}

ResourceContext *resourceContextStack()
{
	return ThreadLocalSingleton<ResourceContext>::instance();
}

} // namespace flux
