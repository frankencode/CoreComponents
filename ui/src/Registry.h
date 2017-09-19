/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace ui {

class ApplicationProvider;
class Application;

class Registry: public Object
{
public:
    static Registry *instance();

    ApplicationProvider *applicationProvider() { return applicationProvider_; }
    Application *application() { return application_; }

private:
    friend class Singleton<Registry>;
    friend class ApplicationProvider;
    friend class Application;

    void registerApplicationProvider(ApplicationProvider *provider);
    void registerApplication(Application *application);

    Ref<ApplicationProvider> applicationProvider_;
    Ref<Application> application_;
};

}} // namespace cc::ui

