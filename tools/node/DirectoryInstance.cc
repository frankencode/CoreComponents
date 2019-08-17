/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/exceptions>
#include "DirectoryInstance.h"

namespace ccnode {

Ref<DirectoryInstance> DirectoryInstance::create(const MetaObject *config)
{
    return new DirectoryInstance(config);
}

DirectoryInstance::DirectoryInstance(const MetaObject *config):
    DeliveryInstance{config},
    path_{config->value("path")},
    showHidden_{config->value("show-hidden")}
{
    if (path_ == "")
        throw UsageError{"DirectoryInstance: Mandatory argument \"path\" is missing"};
    path_ = path_->canonicalPath();
    if (!File::exists(path_)) {
        throw UsageError{
            Format("DirectoryInstance: Directory path \"%%\" does not exist") << path_
        };
    }
    if (FileStatus::read(path_)->type() != FileType::Directory) {
        throw UsageError{
            Format("DirectoryInstance: Path \"%%\" does not point to a directory") << path_
        };
    }
}

Ref<DeliveryDelegate> DirectoryInstance::createDelegate(DeliveryWorker *worker) const
{
    return DirectoryDelegate::create(worker);
}

} // namespace ccnode
