/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <string.h>
#include <cc/exceptions>
#include <cc/stdio> // DEBUG
#include <cc/Guard>
#include <cc/ui/ApplicationProvider>
#include <cc/ui/Image>
#include <cc/ui/sdl/FtFontProvider>
#include <cc/ui/sdl/FtTypeSetter>
#include <cc/ui/sdl/SdlWindow>
#include <cc/ui/sdl/SdlView>
#include <cc/ui/sdl/SdlRenderer>
#include <cc/ui/sdl/SdlApplication>

namespace cc {
namespace ui {

Ref<SdlApplication> SdlApplication::create(int argc, char **argv)
{
    return new SdlApplication(argc, argv);
}

SdlApplication::SdlApplication(int argc, char **argv):
    fontProvider_(FtFontProvider::create()),
    windowById_(WindowById::create()),
    updateChannel_(UpdateChannel::create()),
    renderer_(SdlRenderer::start(this))
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        CC_DEBUG_ERROR(SDL_GetError());

    updateEvent_ = SDL_RegisterEvents(1);
    if (updateEvent_ == (Uint32)-1)
        CC_DEBUG_ERROR("Failed to register update event");
}

SdlApplication::~SdlApplication()
{
    windowById_->deplete();
    SDL_Quit();
}

FontProvider *SdlApplication::fontProvider()
{
    return fontProvider_;
}

void SdlApplication::getDisplayResolution(float *xRes, float *yRes)
{
    // FIXME: with SDL >= 2.0.4:
    //  * provide dpi resolution for desktop and *2 for mobile targets (<=10 in)
    //  * select resolution of the physically largest connected screen
    *xRes = 120;
    *yRes = 120;
}

Ref<Window> SdlApplication::openWindow(String title, int width, int height, Color color)
{
    Ref<SdlWindow> window = SdlWindow::open(title, width, height, color);
    windowById_->insert(window->id(), window);
    return window;
}

void SdlApplication::closeWindow(Window *window)
{
    windowById_->remove(Object::cast<SdlWindow *>(window)->id());
}

Ref<TypeSetter> SdlApplication::createTypeSetter()
{
    return FtTypeSetter::create();
}

int SdlApplication::run()
{
    SDL_Event event_, *event = &event_;
    while (true) {
        if (!SDL_WaitEvent(event))
            CC_DEBUG_ERROR(SDL_GetError());
        if (event->type == SDL_QUIT) break;
        else if (event->type == updateEvent_) handleUpdateEvent(Object::cast<SdlWindow *>(updateChannel_->pop()));
        else if (event->type == SDL_WINDOWEVENT) handleWindowEvent(&event->window);
    }
    return 0;
}

void SdlApplication::pushUpdateEvent(SdlWindow *window)
{
    updateChannel_->push(window);

    SDL_Event event;
    SDL_zero(event);
    event.type = updateEvent_;
    SDL_PushEvent(&event);
}

void SdlApplication::composeScene(SDL_Renderer *renderer, SdlView *view)
{
    Guard<SdlView> guard(view);
    SDL_Texture *texture = view->texture_;
    Image *image = view->image_;
    void *dstData = 0;
    int pitch = 0;
    if (SDL_LockTexture(texture, 0, &dstData, &pitch) != 0) CC_DEBUG_ERROR(SDL_GetError());
    if (pitch == image->pitch()) {
        ::memcpy(dstData, image->data(), image->w() * image->h() * sizeof(Color));
    }
    else {
        fout() << "pitch mismatch:(" << nl;
        fout("image->pitch(), pitch = %%, %%\n") << image->pitch() << pitch;
        fout("image->w() * 4 = %%\n") << image->w() * 4;
        fout("view->w() * 4 = %%\n") << view->w() * 4;
        fout("image->w(), view->w() = %%, %%\n") << image->w() << view->w();
        // TODO
    }
    SDL_UnlockTexture(texture);

    // fout("status, data, pitch = %%, %%, %%\n") << status << data << pitch << nl;
    // fout("view->w() * 4 = %%\n") << view->w() * 4;
    {
        SDL_Rect dstRect;
        view->mapLocalToGlobal(0, 0, &dstRect.x, &dstRect.y);
        dstRect.w = view->w();
        dstRect.h = view->h();
        SDL_RenderCopy(renderer, texture, 0, &dstRect);
    }
    for (int i = 0; i < view->childCount(); ++i)
        composeScene(renderer, Object::cast<SdlView *>(view->childAt(i)));
}

void SdlApplication::handleUpdateEvent(SdlWindow *window)
{
    if (!window) {
        fout() << "update dropped" << nl;
        return;
    }
    SDL_Renderer *renderer = window->renderer_;
    Color c = window->rootView()->color();
    SDL_SetRenderDrawColor(renderer, Color::red(c), Color::green(c), Color::blue(c), 0xFF);
    SDL_RenderClear(renderer);
    composeScene(renderer, Object::cast<SdlView *>(window->rootView()));
    SDL_RenderPresent(renderer);
    fout() << "update event for " << (void*)window << nl;
}

void SdlApplication::handleWindowEvent(SDL_WindowEvent *windowEvent)
{
    if (windowEvent->event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        SdlWindow *window;
        if (windowById_->lookup(windowEvent->windowID, &window))
            window->windowSizeChangedEvent(windowEvent->data1, windowEvent->data2);
    }
    else if (windowEvent->event == SDL_WINDOWEVENT_MOVED) {
        SdlWindow *window;
        if (windowById_->lookup(windowEvent->windowID, &window)) {
            int x = windowEvent->data1;
            int y = windowEvent->data2;
            if (window->x() != x || window->y() != y)
                window->windowMovedEvent(x, y);
        }
    }
}

CCUI_APPLICATION_REGISTRATION(SdlApplication)

}} // namespace cc::ui
