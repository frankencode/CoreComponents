/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "ServiceDelegate.h"
#include "ScriptHandler.h"

namespace ccnode {

class DirectoryInstance;

class DirectoryDelegate: public ServiceDelegate
{
public:
    static Ref<DirectoryDelegate> create(ServiceWorker *worker, ScriptHandler *scriptHandler = 0);

    virtual void process(HttpRequest *request);

private:
    DirectoryDelegate(ServiceWorker *worker, ScriptHandler *scriptHandler = 0);

    void listDirectory(HttpRequest *request, String path);
    void deliverFile(String path);
    void streamFile(String path);

    Ref<DirectoryInstance> directoryInstance_;
    Ref<ScriptHandler> scriptHandler_;
};

} // namespace ccnode
