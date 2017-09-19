/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Channel>
#include <cc/ui/FontProvider>
#include <cc/ui/Window>
#include <cc/ui/TypeSetter>

namespace cc {
namespace ui {

class RenderQueue;

class Application: public Object
{
public:
    static Ref<Application> create(int argc, char **argv);
    static Application *instance();

    virtual FontProvider *fontProvider() = 0;

    virtual void getDisplayResolution(float *xRes, float *yRes) = 0;

    virtual Ref<Window> openWindow(String title, int width, int height, Color color = Color()) = 0;
    virtual void closeWindow(Window *window) = 0;

    virtual Ref<TypeSetter> createTypeSetter() = 0;

    virtual int run() = 0;

    RenderQueue *renderQueue() const;

protected:
    Application();
    ~Application();

private:
    Ref<RenderQueue> renderQueue_;
};

}} // namespace cc::ui

