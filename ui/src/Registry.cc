/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/ui/ApplicationProvider>
#include <cc/ui/Application>
#include <cc/ui/Registry>

namespace cc {
namespace ui {

Registry *Registry::instance()
{
    return Singleton<Registry>::instance();
}

void Registry::registerApplicationProvider(ApplicationProvider *provider)
{
    applicationProvider_ = provider;
}

void Registry::registerApplication(Application *application)
{
    application_ = application;
}

}} // namespace cc::ui
