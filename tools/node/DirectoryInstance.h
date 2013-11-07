/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_DIRECTORYINSTANCE_H
#define FLUXNODE_DIRECTORYINSTANCE_H

#include "ServiceInstance.h"
#include "DirectoryDelegate.h"

namespace fluxnode
{

class DirectoryInstance: public ServiceInstance
{
public:
	static Ref<DirectoryInstance> create(YasonObject *config);

	virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

	inline String path() const { return path_; }

private:
	DirectoryInstance(YasonObject *config);
	String path_;
};

} // namespace fluxnode

#endif // FLUXNODE_DIRECTORYINSTANCE_H
