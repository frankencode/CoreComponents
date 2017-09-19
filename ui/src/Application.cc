/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Registry>
#include <cc/ui/RenderQueue>
#include <cc/ui/ApplicationProvider>
#include <cc/ui/Application>

namespace cc {
namespace ui {

Ref<Application> Application::create(int argc, char **argv)
{
    return Registry::instance()->applicationProvider()->createApplication(argc, argv);
}

Application *Application::instance()
{
    return Registry::instance()->application();
}

Application::Application():
    renderQueue_(RenderQueue::create())
{
    Registry::instance()->registerApplication(this);
}

Application::~Application()
{}

RenderQueue *Application::renderQueue() const
{
    return renderQueue_;
}

}} // namespace cc::ui
