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
#include "ResourceContextStack.h"

namespace flux
{

ResourceContextStack::ResourceContextStack()
	: queue_(Queue<String>::create())
{}

void ResourceContextStack::push(String resource)
{
	queue_->pushBack(resource);
}

String ResourceContextStack::pop()
{
	return queue_->popBack();
}

String ResourceContextStack::top() const
{
	return queue_->size() > 0 ? queue_->back() : String();
}

ResourceContextStack *resourceContextStack()
{
	return ThreadLocalSingleton<ResourceContextStack>::instance();
}

} // namespace flux
