/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryRegistry>
#include <cc/node/DeliveryService>
#include <cc/node/DirectoryService>
#include <cc/node/CgiService>
#include <cc/node/CgiDirectoryInstance>

namespace cc {
namespace node {

class CgiDirectoryService: public DeliveryService
{
public:
    static Ref<CgiDirectoryService> create() {
        return new CgiDirectoryService;
    }

    DeliveryPrototype *configPrototype() const override { return configPrototype_; }

    Ref<DeliveryInstance> createInstance(const MetaObject *config) const override
    {
        return CgiDirectoryInstance::create(config);
    }

private:
    CgiDirectoryService():
        configPrototype_{DeliveryPrototype::create("CGI-Directory")}
    {
        CgiService::establish(configPrototype_);
        DirectoryService::establish(configPrototype_);
    }

    Ref<DeliveryPrototype> configPrototype_;
};

class CgiDirectoryServiceAnnouncer {
public:
    CgiDirectoryServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<CgiDirectoryService>();
        done = true;
    }
};

namespace { CgiDirectoryServiceAnnouncer announcer; }

}} // namespace cc::node
