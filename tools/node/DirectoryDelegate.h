/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "DeliveryDelegate.h"
#include "ScriptHandler.h"

namespace ccnode {

class DirectoryInstance;

class DirectoryDelegate: public DeliveryDelegate
{
public:
    static Ref<DirectoryDelegate> create(DeliveryWorker *worker, ScriptHandler *scriptHandler = nullptr);

    void process(HttpRequest *request) override;

private:
    DirectoryDelegate(DeliveryWorker *worker, ScriptHandler *scriptHandler = nullptr);

    void listDirectory(HttpRequest *request, const String &path);
    void deliverFile(const String &path);
    void streamFile(const String &path);

    Ref<const DirectoryInstance> directoryInstance_;
    Ref<ScriptHandler> scriptHandler_;
};

} // namespace ccnode
