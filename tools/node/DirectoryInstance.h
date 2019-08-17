/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "DeliveryInstance.h"
#include "DirectoryDelegate.h"

namespace ccnode {

class DirectoryInstance: public virtual DeliveryInstance
{
public:
    static Ref<DirectoryInstance> create(const MetaObject *config);

    Ref<DeliveryDelegate> createDelegate(DeliveryWorker *worker) const override;

    String path() const { return path_; }
    bool showHidden() const { return showHidden_; }

protected:
    DirectoryInstance(const MetaObject *config);

    String path_;
    bool showHidden_;
};

} // namespace ccnode
