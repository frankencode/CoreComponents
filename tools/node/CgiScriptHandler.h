/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "ScriptHandler.h"

namespace ccnode {

class CgiDelegate;
class ServiceWorker;

class CgiScriptHandler: public ScriptHandler
{
public:
    static Ref<CgiScriptHandler> create(ServiceWorker *worker);

    bool process(HttpRequest *request, FileStatus *status, const String &documentRoot = "");

private:
    CgiScriptHandler(ServiceWorker *worker);
    ~CgiScriptHandler();

    Ref<CgiDelegate> delegate_;
};

} // namespace ccnode
