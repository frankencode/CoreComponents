/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "DirectoryInstance.h"
#include "CgiInstance.h"

namespace ccnode {

class CgiDirectoryInstance: public DirectoryInstance, public CgiInstance
{
public:
    static Ref<CgiDirectoryInstance> create(MetaObject *config);

    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

private:
    CgiDirectoryInstance(MetaObject *config);

    String script_;
    Ref<MetaObject> environment_;
};

} // namespace ccnode
