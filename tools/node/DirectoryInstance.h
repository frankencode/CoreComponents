/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_DIRECTORYINSTANCE_H
#define FNODE_DIRECTORYINSTANCE_H

#include "ServiceInstance.h"
#include "DirectoryDelegate.h"

namespace fnode
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

} // namespace fnode

#endif // FNODE_DIRECTORYINSTANCE_H
