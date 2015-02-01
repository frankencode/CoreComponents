/*
 * Copyright (C) 2007-2015 Frank Mertens.
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
    inline bool showHidden() const { return showHidden_; }

private:
    DirectoryInstance(MetaObject *config);
    String path_;
    bool showHidden_;
};

} // namespace fluxnode

#endif // FLUXNODE_DIRECTORYINSTANCE_H
