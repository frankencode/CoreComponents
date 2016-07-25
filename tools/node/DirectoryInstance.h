/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "ServiceInstance.h"
#include "DirectoryDelegate.h"

namespace ccnode {

class DirectoryInstance: public virtual ServiceInstance
{
public:
    static Ref<DirectoryInstance> create(MetaObject *config);

    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

    inline String path() const { return path_; }
    inline bool showHidden() const { return showHidden_; }

protected:
    DirectoryInstance(MetaObject *config);
    String path_;
    bool showHidden_;
};

} // namespace ccnode
