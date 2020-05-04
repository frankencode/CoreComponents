/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/CgiDirectoryInstance>
#include <cc/http/CgiScriptHandler>
#include <cc/http/DirectoryDelegate>

namespace cc {
namespace http {

Ref<CgiDirectoryInstance> CgiDirectoryInstance::create(const MetaObject *config)
{
    return new CgiDirectoryInstance{config};
}

CgiDirectoryInstance::CgiDirectoryInstance(const MetaObject *config):
    DeliveryInstance{config},
    DirectoryInstance{config},
    CgiInstance{config}
{}

Ref<DeliveryDelegate> CgiDirectoryInstance::createDelegate(DeliveryWorker *worker) const
{
    return DirectoryDelegate::create(worker, CgiScriptHandler::create(worker));
}

}} // namespace cc::http
