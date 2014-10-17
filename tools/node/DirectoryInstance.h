/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_DIRECTORYINSTANCE_H
#define FLUXNODE_DIRECTORYINSTANCE_H

#include "ServiceInstance.h"
#include "DirectoryDelegate.h"

namespace fluxnode {

class DirectoryInstance: public ServiceInstance
{
public:
    static Ref<DirectoryInstance> create(MetaObject *config);

    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

    inline String path() const { return path_; }

private:
    DirectoryInstance(MetaObject *config);
    String path_;
};

} // namespace fluxnode

#endif // FLUXNODE_DIRECTORYINSTANCE_H
