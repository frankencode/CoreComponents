/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/DirectoryService>
#include <cc/DirectoryInstance>
#include <cc/HttpServiceRegistry>

namespace cc {

struct DirectoryService::State: public HttpService::State
{
    State():
        configPrototype_{DirectoryService::name()}
    {
        configPrototype_.establish("path", "");
        configPrototype_.establish("show-hidden", false);
    }

    HttpServiceConfigPrototype configPrototype() const override
    {
        return configPrototype_;
    }

    HttpServiceInstance createInstance(const MetaObject &config) const override
    {
        return DirectoryInstance{config};
    }

    HttpServiceConfigPrototype configPrototype_;
};

const char *DirectoryService::name()
{
    return "Directory";
}

DirectoryService::DirectoryService():
    HttpService{new State}
{}

namespace { HttpServiceRegistry::Announcer<DirectoryService> announcer; }

} // namespace cc
