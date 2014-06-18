/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_RESOURCECONTEXT_H
#define FLUX_RESOURCECONTEXT_H

#include "String.h"

namespace flux
{

template<class> class ThreadLocalSingleton;
template<class> class Queue;

class ResourceContext: public Object
{
public:
	void push(String resource);
	String pop();

	String top() const;

private:
	friend class ThreadLocalSingleton<ResourceContext>;
	ResourceContext();

	Ref< Queue<String> > queue_;
};

ResourceContext *resourceContextStack();

} // namespace flux

#endif // FLUX_RESOURCECONTEXT_H
