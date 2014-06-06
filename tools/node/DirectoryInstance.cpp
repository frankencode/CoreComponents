/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include "errors.h"
#include "DirectoryInstance.h"

namespace fluxnode
{

Ref<DirectoryInstance> DirectoryInstance::create(YasonObject *config)
{
	return new DirectoryInstance(config);
}

DirectoryInstance::DirectoryInstance(YasonObject *config)
	: ServiceInstance(config),
	  path_(config->value("path"))
{
	if (path_ == "")
		throw UsageError("DirectoryInstance: Mandatory argument \"path\" is missing");
	path_ = path_->canonicalPath();
	if (!File::exists(path_)) {
		throw UsageError(
			Format("DirectoryInstance: Directory path \"%%\" does not exist") << path_
		);
	}
	if (File::status(path_)->type() != File::Directory) {
		throw UsageError(
			Format("DirectoryInstance: Path \"%%\" does not point to a directory") << path_
		);
	}
}

Ref<ServiceDelegate> DirectoryInstance::createDelegate(ServiceWorker *worker) const
{
	return DirectoryDelegate::create(worker);
}

} // namespace fluxnode
