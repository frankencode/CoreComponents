/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/CgiScriptHandler>
#include <cc/node/CgiDelegate>

namespace cc {
namespace node {

Ref<CgiScriptHandler> CgiScriptHandler::create(DeliveryWorker *worker)
{
    return new CgiScriptHandler{worker};
}

CgiScriptHandler::CgiScriptHandler(DeliveryWorker *worker):
    delegate_{CgiDelegate::create(worker)}
{}

CgiScriptHandler::~CgiScriptHandler()
{}

bool CgiScriptHandler::process(HttpRequest *request, FileStatus *status, const String &documentRoot)
{
    return delegate_->process(request, status, documentRoot);
}

}} // namespace cc::node
