/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <SDL2/SDL.h>
#include <cc/ui/Application>
#include <cc/ui/UpdateChannel>

namespace cc {
namespace ui {

class SdlWindow;
class SdlRenderer;
class SdlView;

class SdlApplication: public Application {
public:
    static Ref<SdlApplication> create(int argc, char **argv);

    virtual FontProvider *fontProvider();

    virtual void getDisplayResolution(float *xRes, float *yRes);

    virtual Ref<Window> openWindow(String title, int width, int height, Color color = Color());
    virtual void closeWindow(Window *window);

    virtual Ref<TypeSetter> createTypeSetter();

    virtual int run();

private:
    friend class SdlWindow;
    friend class SdlRenderer;

    SdlApplication(int argc, char **argv);
    ~SdlApplication();

    void pushUpdateEvent(SdlWindow *window);
    void composeScene(SDL_Renderer *renderer, SdlView *view);
    void handleUpdateEvent(SdlWindow *window);

    void handleWindowEvent(SDL_WindowEvent *windowEvent);

    Ref<FontProvider> fontProvider_;

    typedef Map<Uint32, Ref<SdlWindow> > WindowById;
    Ref<WindowById> windowById_;

    Ref<UpdateChannel> updateChannel_;
    Uint32 updateEvent_;

    Ref<SdlRenderer> renderer_;
};

}} // namespace cc::ui

