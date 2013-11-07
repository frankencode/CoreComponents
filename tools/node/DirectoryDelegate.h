/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_DIRECTORYDELEGATE_H
#define FLUXNODE_DIRECTORYDELEGATE_H

#include "ServiceDelegate.h"

namespace fnode
{

class DirectoryInstance;

class DirectoryDelegate: public ServiceDelegate
{
public:
	static Ref<DirectoryDelegate> create(ServiceWorker *worker);

	virtual void process(Request *request);

private:
	DirectoryDelegate(ServiceWorker *worker);

	void listDirectory(Request *request, String path);

	Ref<DirectoryInstance> directoryInstance_;
};

} // namespace fnode

#endif // FLUXNODE_DIRECTORYDELEGATE_H
