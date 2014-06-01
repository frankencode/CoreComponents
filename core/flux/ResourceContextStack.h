/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_RESOURCECONTEXTSTACK_H
#define FLUX_RESOURCECONTEXTSTACK_H

#include "String.h"

namespace flux
{

template<class> class ThreadLocalSingleton;
template<class> class Queue;

class ResourceContextStack: public Object
{
public:
	void push(String resource);
	String pop();

	String top() const;

private:
	friend class ThreadLocalSingleton<ResourceContextStack>;
	ResourceContextStack();

	Ref< Queue<String> > queue_;
};

ResourceContextStack *resourceContextStack();

} // namespace flux

#endif // FLUX_RESOURCECONTEXTSTACK_H
