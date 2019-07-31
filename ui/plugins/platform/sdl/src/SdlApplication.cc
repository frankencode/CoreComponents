/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/exceptions>
#include <cc/Singleton>
#include <cc/Format>
#include <cc/ui/SdlWindow>
#include <cc/ui/SdlCursor>
#include <cc/ui/Timer>
#include <cc/ui/TimeMaster>
#include <cc/ui/FingerEvent>
#include <cc/ui/MouseEvent>
#include <cc/ui/PointerEvent>
#include <cc/ui/WheelEvent>
#include <cc/ui/KeyEvent>
#include <cc/ui/SdlApplication>

namespace cc {
namespace ui {

SdlApplication *SdlApplication::instance()
{
    return Singleton<SdlApplication>::instance();
}

SdlApplication::SdlApplication():
    windows_{Windows::create()},
    event_{new SDL_Event}
{
    cursorVisible->connect([=]{
        if (cursorVisible())
            SDL_ShowCursor(SDL_ENABLE);
        else
            SDL_ShowCursor(SDL_DISABLE);
    });

    screenSaverEnabled->connect([=]{
        if (screenSaverEnabled())
            SDL_EnableScreenSaver();
        else
            SDL_DisableScreenSaver();
    });

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        // linear filtering causes artefacts with moving objects with OpenGL acceleration
        // therefore we might force nearest-neighbor filtering, which looks quite OK at the higher framerate

    // SDL_SetHint(SDL_HINT_RENDER_BATCHING, "0");
    // SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
}

SdlApplication::~SdlApplication()
{
    fin_ = true;
    delete event_;
    windows_->deplete();
    SDL_Quit();
}

void SdlApplication::init(int argc, char **argv)
{
    timerEvent_ = SDL_RegisterEvents(1);
}

Window *SdlApplication::openWindow(View *view, const String &title, WindowMode mode)
{
    auto window = SdlWindow::open(view, title, mode);
    windows_->insert(window->id_, window);
    return window;
}

Ref<Cursor> SdlApplication::createCursor(CursorShape shape)
{
    return SdlCursor::create(shape);
}

void SdlApplication::setCursor(const Cursor *cursor)
{
    SDL_SetCursor(Object::cast<const SdlCursor *>(cursor)->sdlCursor_);
}

void SdlApplication::unsetCursor()
{
    SDL_SetCursor(SDL_GetDefaultCursor());
}

String SdlApplication::getClipboardText() const
{
    if (!SDL_HasClipboardText()) return String{};
    return SDL_GetClipboardText();
}

void SdlApplication::setClipboardText(const String &text)
{
    /*int ret =*/ SDL_SetClipboardText(text);
    // TODO: error handling?
}

int SdlApplication::run()
{
    while (true) {
        if (!SDL_WaitEvent(event_))
            CC_DEBUG_ERROR(SDL_GetError());

        // CC_INSPECT(event_->type);

        if (event_->type == timerEvent_) {
            Timer *t = (Timer *)event_->user.data1;
            notifyTimer(t);
            t->decRefCount();
        }
        else switch(event_->type) {
            case SDL_FINGERMOTION:
                while (
                    SDL_PeepEvents(
                        event_, 1, SDL_GETEVENT,
                        SDL_FINGERMOTION, SDL_FINGERMOTION
                    ) > 0
                );
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
                handleFingerEvent(&event_->tfinger);
                break;
            case SDL_MOUSEMOTION:
                while (
                    SDL_PeepEvents(
                        event_, 1, SDL_GETEVENT,
                        SDL_MOUSEMOTION, SDL_MOUSEMOTION
                    ) > 0
                );
                handleMouseMotionEvent(&event_->motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                handleMouseButtonEvent(&event_->button);
                break;
            case SDL_MOUSEWHEEL:
                handleMouseWheelEvent(&event_->wheel);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handleKeyboardEvent(&event_->key);
                break;
            case SDL_WINDOWEVENT:
                handleWindowEvent(&event_->window);
                break;
            case SDL_TEXTEDITING:
                handleTextEditingEvent(&event_->edit);
                break;
            case SDL_TEXTINPUT:
                handleTextInputEvent(&event_->text);
                break;
            case SDL_QUIT:
                return 0;
        };

        for (int i = 0; i < windows_->count(); ++i)
            windows_->valueAt(i)->commitFrame();
    }

    return 0;
}

void SdlApplication::handleFingerEvent(const SDL_TouchFingerEvent *e)
{
    PointerAction action; {
        switch (e->type) {
            case SDL_FINGERMOTION: action = PointerAction::Moved; break;
            case SDL_FINGERDOWN  : action = PointerAction::Pressed; break;
            default              : action = PointerAction::Released; break;
        };
    }

    auto event =
        Object::create<FingerEvent>(
            action,
            e->timestamp / 1000.,
            e->touchId,
            e->fingerId,
            Point { double(e->x),  double(e->y)  },
            Step  { double(e->dx), double(e->dy) },
            e->pressure
        );

    for (auto pair: windows_) {
        Window *window = pair->value();
        feedFingerEvent(window, event);
    }
}

void SdlApplication::handleMouseMotionEvent(const SDL_MouseMotionEvent *e)
{
    if (e->which == SDL_TOUCH_MOUSEID) return;

    MouseButton button = MouseButton::None; {
        if (e->state & SDL_BUTTON_LMASK)  button |= MouseButton::Left;
        if (e->state & SDL_BUTTON_RMASK)  button |= MouseButton::Right;
        if (e->state & SDL_BUTTON_MIDDLE) button |= MouseButton::Middle;
        if (e->state & SDL_BUTTON_X1MASK) button |= MouseButton::X1;
        if (e->state & SDL_BUTTON_X2MASK) button |= MouseButton::X2;
    }

    auto event =
        Object::create<MouseEvent>(
            PointerAction::Moved,
            e->timestamp / 1000.,
            button,
            0,
            Point { double(e->x),    double(e->y)    },
            Step  { double(e->xrel), double(e->yrel) }
        );

    for (auto pair: windows_) {
        SdlWindow *window = pair->value();
        if (window->id_ == e->windowID)
            feedMouseEvent(window, event);
    }
}

void SdlApplication::handleMouseButtonEvent(const SDL_MouseButtonEvent *e)
{
    if (e->which == SDL_TOUCH_MOUSEID) return;

    PointerAction action =
        (e->type == SDL_MOUSEBUTTONDOWN) ?
        PointerAction::Pressed :
        PointerAction::Released;

    MouseButton button; {
        switch (e->button) {
            case SDL_BUTTON_LEFT  : button = MouseButton::Left; break;
            case SDL_BUTTON_RIGHT : button = MouseButton::Right; break;
            case SDL_BUTTON_MIDDLE: button = MouseButton::Middle; break;
            case SDL_BUTTON_X1    : button = MouseButton::X1; break;
            default               : button = MouseButton::X2; break;
        };
    }

    auto event =
        Object::create<MouseEvent>(
            action,
            e->timestamp / 1000.,
            button,
            e->clicks,
            Point { double(e->x), double(e->y) },
            Step  {}
        );

    for (auto pair: windows_) {
        SdlWindow *window = pair->value();
        if (window->id_ == e->windowID)
            feedMouseEvent(window, event);
    }
}

void SdlApplication::handleMouseWheelEvent(const SDL_MouseWheelEvent *e)
{
    int mx = 0, my = 0;
    SDL_GetMouseState(&mx, &my);

    auto event =
        Object::create<WheelEvent>(
            e->timestamp / 1000.,
            Step  { double(e->x), double(e->y) },
            Point { double(mx),   double(my)   }
        );

    for (auto pair: windows_) {
        SdlWindow *window = pair->value();
        if (window->id_ == e->windowID)
            feedWheelEvent(window, event);
    }
}

void SdlApplication::handleKeyboardEvent(const SDL_KeyboardEvent *e)
{
    auto event =
        Object::create<KeyEvent>(
            (e->type == SDL_KEYDOWN) ? KeyAction::Pressed : KeyAction::Released,
            e->timestamp / 1000.,
            e->repeat,
            static_cast<ScanCode>(e->keysym.scancode),
            static_cast<KeyCode>(e->keysym.sym),
            static_cast<KeyModifier>(e->keysym.mod)
        );

    for (auto pair: windows_) {
        SdlWindow *window = pair->value();
        feedKeyEvent(window, event);
    }
}

void SdlApplication::handleTextEditingEvent(const SDL_TextEditingEvent *e)
{
    feedTextEditingEvent(e->text, e->start, e->length);
}

void SdlApplication::handleTextInputEvent(const SDL_TextInputEvent *e)
{
    feedTextInputEvent(e->text);
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
    // CC_DEBUG << windowEventToString(e);
    switch (e->event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window)) {
                int w = 0, h = 0;
                SDL_GetWindowSize(window->sdlWindow_, &w, &h);
                // CC_DEBUG << Vector<int, 2>{w, h};
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
                feedExposedEvent(window);
            break;
        }
        case SDL_WINDOWEVENT_MOVED: {
            SdlWindow *window;
            if (windows_->lookup(e->windowID, &window))
                window->onWindowMoved(Point{double(e->data1), double(e->data2)});
            break;
        }
        case SDL_WINDOWEVENT_ENTER: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window))
                feedEnterEvent(window);
            break;
        }
        case SDL_WINDOWEVENT_LEAVE: {
            SdlWindow *window = 0;
            if (windows_->lookup(e->windowID, &window))
                feedLeaveEvent(window);
            break;
        }
    };
}

void SdlApplication::startTextInput(const Window *window)
{
    const SdlWindow *w = Object::cast<const SdlWindow *>(window);

    if (SDL_IsTextInputActive())
        SDL_StopTextInput();

    if (SDL_GetKeyboardFocus() != w->sdlWindow_)
        SDL_RaiseWindow(w->sdlWindow_);

    SDL_StartTextInput();
}

void SdlApplication::setTextInputArea(const Rect &inputArea)
{
    SDL_Rect rect;
    rect.x = inputArea->pos()[0];
    rect.y = inputArea->pos()[1];
    rect.w = inputArea->size()[0];
    rect.h = inputArea->size()[1];
    SDL_SetTextInputRect(&rect);
}

void SdlApplication::stopTextInput()
{
    SDL_StopTextInput();
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
