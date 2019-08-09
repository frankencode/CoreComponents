/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "CgiDelegate.h"
#include "CgiScriptHandler.h"

namespace ccnode {

Ref<CgiScriptHandler> CgiScriptHandler::create(ServiceWorker *worker)
{
    return new CgiScriptHandler{worker};
}

CgiScriptHandler::CgiScriptHandler(ServiceWorker *worker):
    delegate_{CgiDelegate::create(worker)}
{}

CgiScriptHandler::~CgiScriptHandler()
{}

bool CgiScriptHandler::process(HttpRequest *request, FileStatus *status, const String &documentRoot)
{
    return delegate_->process(request, status, documentRoot);
}

} // namespace ccnode
