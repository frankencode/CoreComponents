/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/File>
#include "exceptions.h"
#include "DirectoryInstance.h"

namespace fluxnode
{

Ref<DirectoryInstance> DirectoryInstance::create(MetaObject *config)
{
    return new DirectoryInstance(config);
}

DirectoryInstance::DirectoryInstance(MetaObject *config)
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
