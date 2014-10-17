/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_DIRECTORYDELEGATE_H
#define FLUXNODE_DIRECTORYDELEGATE_H

#include "ServiceDelegate.h"

namespace fluxnode
{

class DirectoryInstance;

class DirectoryDelegate: public ServiceDelegate
{
public:
    static Ref<DirectoryDelegate> create(ServiceWorker *worker);

    virtual void process(Request *request);

private:
    DirectoryDelegate(ServiceWorker *worker);

    void listDirectory(Request *request, String path);
    void deliverFile(String path);
    void streamFile(String path);

    Ref<DirectoryInstance> directoryInstance_;
};

} // namespace fluxnode

#endif // FLUXNODE_DIRECTORYDELEGATE_H
