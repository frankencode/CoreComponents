/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/CgiDirectoryInstance>
#include <cc/node/CgiScriptHandler>
#include <cc/node/DirectoryDelegate>

namespace cc {
namespace node {

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

}} // namespace cc::node
