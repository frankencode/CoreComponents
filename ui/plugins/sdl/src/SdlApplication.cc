/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/Singleton>
#include <cc/Format>
#include <cc/debug> // DEBUG
#include <cc/ui/SdlWindow>
#include <cc/ui/Timer>
#include <cc/ui/TimeMaster>
#include <cc/ui/SdlApplication>

namespace cc {
namespace ui {

SdlApplication *SdlApplication::instance()
{
    return Singleton<SdlApplication>::instance();
}

SdlApplication::SdlApplication():
    windows_(Windows::create()),
    event_(new SDL_Event)
{}

SdlApplication::~SdlApplication()
{
    delete event_;
    windows_->deplete();
    SDL_Quit();
}

void SdlApplication::init(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        CC_DEBUG_ERROR(SDL_GetError());

    timerEvent_ = SDL_RegisterEvents(1);
}

void SdlApplication::getDisplayResolution(double *xRes, double *yRes) const
{
    // FIXME: with SDL >= 2.0.4:
    //  * provide dpi resolution for desktop and *2 for mobile targets (<=10 in)
    //  * select resolution of the physically largest connected screen
    *xRes = 120;
    *yRes = 120;
}

Window *SdlApplication::openWindow(View *view, String title, WindowMode mode)
{
    Ref<SdlWindow> window = SdlWindow::open(view, title, mode);
    windows_->insert(window->id_, window);
    return window;
}

int SdlApplication::run()
{
    while (true) {
        if (!SDL_WaitEvent(event_))
            CC_DEBUG_ERROR(SDL_GetError());

        CC_INSPECT(event_->type);

        if (event_->type == timerEvent_) {
            Timer *t = (Timer *)event_->user.data1;
            notifyTimer(t);
            t->decRefCount();
        }
        else if (
            event_->type == SDL_FINGERMOTION ||
            event_->type == SDL_FINGERDOWN ||
            event_->type == SDL_FINGERUP
        ) {
            handleFingerEvent(&event_->tfinger);
        }
        else if (event_->type == SDL_WINDOWEVENT) {
            handleWindowEvent(&event_->window);
        }
        else if (event_->type == SDL_QUIT) {
            return 0;
        }

        for (int i = 0; i < windows_->count(); ++i)
            windows_->valueAt(i)->commitFrame();
    }

    return 0;
}

void SdlApplication::handleFingerEvent(const SDL_TouchFingerEvent *e)
{
    // #ifndef NDEBUG
    auto eventName = [](Uint32 type) -> String {
        switch (type) {
            case SDL_FINGERMOTION: return "SDL_FINGERMOTION";
            case SDL_FINGERDOWN: return "SDL_FINGERDOWN";
            case SDL_FINGERUP: return "SDL_FINGERUP";
        };
        return "";
    };
    CC_DEBUG
        << eventName(e->type) << nl
        << "  timestsamp: " << e->timestamp << nl
        << "  touchId: " << e->touchId << nl
        << "  fingerId: " << e->fingerId << nl
        << "  x, y: " << e->x << ", " << e->y << nl
        << "  dx, dy: " << e->dx << ", " << e->dy << nl;
    // #endif
}

String SdlApplication::windowEventToString(const SDL_WindowEvent *e)
{
    switch (e->event) {
    case SDL_WINDOWEVENT_SHOWN: return "SDL_WINDOWEVENT_SHOWN";
    case SDL_WINDOWEVENT_HIDDEN: return "SDL_WINDOWEVENT_HIDDEN";
    case SDL_WINDOWEVENT_EXPOSED: return  Format("SDL_WINDOWEVENT_EXPOSED (%% %%)") << e->data1 << e->data2;
    case SDL_WINDOWEVENT_MOVED: return Format("SDL_WINDOWEVENT_MOVED (%% %%)") << e->data1 << e->data2;
    case SDL_WINDOWEVENT_RESIZED: return Format("SDL_WINDOWEVENT_RESIZED (%% %%)") << e->data1 << e->data2;
    case SDL_WINDOWEVENT_SIZE_CHANGED: return Format("SDL_WINDOWEVENT_SIZE_CHANGED (%% %%)") << e->data1 << e->data2;
    case SDL_WINDOWEVENT_MINIMIZED: return "SDL_WINDOWEVENT_MINIMIZED";
    case SDL_WINDOWEVENT_MAXIMIZED: return "SDL_WINDOWEVENT_MAXIMIZED";
    case SDL_WINDOWEVENT_RESTORED: return "SDL_WINDOWEVENT_RESTORED";
    case SDL_WINDOWEVENT_ENTER: return "SDL_WINDOWEVENT_ENTER";
    case SDL_WINDOWEVENT_LEAVE: return "SDL_WINDOWEVENT_LEAVE";
    case SDL_WINDOWEVENT_FOCUS_GAINED: return "SDL_WINDOWEVENT_FOCUS_GAINED";
    case SDL_WINDOWEVENT_FOCUS_LOST: return "SDL_WINDOWEVENT_FOCUS_LOST";
    case SDL_WINDOWEVENT_CLOSE: return "SDL_WINDOWEVENT_CLOSE";
    #if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS: return "SDL_WINDOWEVENT_TAKE_FOCUS";
    case SDL_WINDOWEVENT_HIT_TEST: return "SDL_WINDOWEVENT_HIT_TEST";
    #endif
    };
    return String{};
}

void SdlApplication::handleWindowEvent(const SDL_WindowEvent *e)
{
    CC_DEBUG << windowEventToString(e);
    switch (e->event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window)) {
                int w = 0, h = 0;
                SDL_GetWindowSize(window->sdlWindow_, &w, &h);
                CC_DEBUG << Vector<int, 2>{w, h};
                window->onWindowResized(Size{double(e->data1), double(e->data2)});
            }
            break;
        }
        #if 0
        case SDL_WINDOWEVENT_RESIZED: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window)) {
                int w = 0, h = 0;
                SDL_GetWindowSize(window->sdlWindow_, &w, &h);
                CC_DEBUG << Vector<int, 2>{w, h};
                // window->onWindowResized(Size{double(e->data1), double(e->data2)});
            }
            break;
        }
        #endif
        case SDL_WINDOWEVENT_SHOWN: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window))
                window->onWindowShown();
            break;
        }
        case SDL_WINDOWEVENT_HIDDEN: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window))
                window->onWindowHidden();
            break;
        }
        case SDL_WINDOWEVENT_EXPOSED: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window))
                window->onWindowExposed();
            break;
        }
        case SDL_WINDOWEVENT_MOVED: {
            SdlWindow *window;
            if (windows_->lookup(e->windowID, &window))
                window->onWindowMoved(Point{double(e->data1), double(e->data2)});
            break;
        }
    };
}

void SdlApplication::triggerTimer(const Timer *timer)
{
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event));
    event.type = timerEvent_;
    event.user.code = 0;
    Timer *t = const_cast<Timer *>(timer);
    t->incRefCount();
    event.user.data1 = (void *)t;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
}

}} // namespace cc::ui
