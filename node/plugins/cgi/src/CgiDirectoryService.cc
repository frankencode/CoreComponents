/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DeliveryRegistry>
#include <cc/http/DeliveryService>
#include <cc/http/DirectoryService>
#include <cc/http/CgiService>
#include <cc/http/CgiDirectoryInstance>

namespace cc {
namespace http {

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

}} // namespace cc::http
