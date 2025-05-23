/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SdlApplication>
#include <cc/SdlWindow>
#include <cc/SdlCursor>
#include <cc/SdlPlatformError>
#include <cc/SdlTimeMaster>
#include <cc/DEBUG>
#include <SDL2/SDL.h>

namespace cc {

struct SdlApplication::State: public Application::State
{
    State():
        timerEvent_{SDL_RegisterEvents(2)},
        userEvent_{timerEvent_ + 1},
        cursorVisible_{SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE}
    {}

    ~State()
    {
        SDL_Quit();
    }

    void quit() override
    {
        SDL_Quit();
    }

    Window createWindow(const View &view) override
    {
        return SdlWindow{view};
    }

    Window appWindow() const override
    {
        Window window = focusWindow();
        if (!window) {
            for (const auto &pair: windows_) {
                if (pair.value().visible()) {
                    window = pair.value();
                    break;
                }
            }
        }
        return window;
    }

    void captureMouse(bool on) override
    {
        SDL_CaptureMouse(on ? SDL_TRUE : SDL_FALSE);
    }

    Cursor createCursor(CursorShape shape) override
    {
        return SdlCursor{shape};
    }

    Cursor createCursor(const Image &image, const Point &hotspot) override
    {
        return SdlCursor{image, hotspot};
    }

    void setCursor(const Cursor &cursor) override
    {
        SDL_SetCursor(SdlCursor{cursor}.sdlCursor());
    }

    void unsetCursor() override
    {
        SDL_SetCursor(SDL_GetDefaultCursor());
    }

    void showCursor(bool on) override
    {
        if (cursorVisible_ != on) {
            SDL_ShowCursor(on ? SDL_ENABLE : SDL_DISABLE);
            cursorVisible_ = on;
        }
    }

    String getClipboardText() const override
    {
        if (!SDL_HasClipboardText()) return String{};
        return SDL_GetClipboardText();
    }

    void setClipboardText(const String &text) override
    {
        if (SDL_SetClipboardText(text) != 0)
            throw SdlPlatformError{};
    }

    void startTextInput(const Window &window) override
    {
        SDL_Window *sdlWindow = window.as<SdlWindow>().sdlWindow();

        if (SDL_IsTextInputActive())
            SDL_StopTextInput();

        if (SDL_GetKeyboardFocus() != sdlWindow)
            SDL_RaiseWindow(sdlWindow);

        SDL_StartTextInput();
    }

    void setTextInputArea(const Rect &inputArea) override
    {
        SDL_Rect rect;
        rect.x = inputArea.pos()[0];
        rect.y = inputArea.pos()[1];
        rect.w = inputArea.size()[0];
        rect.h = inputArea.size()[1];
        SDL_SetTextInputRect(&rect);
    }

    void stopTextInput() override
    {
        SDL_StopTextInput();
    }

    void postEvent(Function<void()> &&doNext) override
    {
        SDL_Event event;
        ::memset(&event, 0, sizeof(SDL_Event));
        event.type = userEvent_;
        event.user.data1 = new Function<void()>{move(doNext)};
        SDL_PushEvent(&event);
    }

    int run() override
    {
        SDL_Event event;

        do {
            if (!SDL_WaitEvent(&event)) {
                // throw SdlPlatformError{};
                break;
            }

            if (event.type == timerEvent_) {
                Timer *p = reinterpret_cast<Timer *>(&event.user.data1);
                notifyTimer(*p);
                p->~Timer();
            }
            else if (event.type == userEvent_) {
                Function<void()> *f = static_cast<Function<void()> *>(event.user.data1);
                (*f)();
                delete f;
            }
            else switch(event.type) {
                case SDL_FINGERMOTION:
                    do {
                        handleFingerEvent(event.tfinger);
                    } while (
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_FINGERMOTION, SDL_FINGERMOTION
                        ) > 0
                    );
                    break;
                case SDL_MOUSEMOTION: {
                    do {
                        handleMouseMotionEvent(event.motion);
                    } while (
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_MOUSEMOTION, SDL_MOUSEMOTION
                        ) > 0
                    );
                    break;
                }
                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                    handleFingerEvent(event.tfinger);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    handleMouseButtonEvent(event.button);
                    break;
                case SDL_MOUSEWHEEL:
                    handleMouseWheelEvent(event.wheel);
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    handleKeyboardEvent(event.key);
                    break;
                case SDL_WINDOWEVENT:
                    handleWindowEvent(event.window);
                    break;
                case SDL_TEXTEDITING:
                    handleTextEditingEvent(event.edit);
                    break;
                case SDL_TEXTINPUT:
                    handleTextInputEvent(event.text);
                    break;
                /*case SDL_RENDER_TARGETS_RESET:
                    break;*/
                default:
                    break;
            };

            for (const auto &pair: windows_) {
                SdlWindow{pair.value()}.me().commitFrame();
            }
        }
        while (event.type != SDL_QUIT);


        SdlTimeMaster::shutdown();
        Singleton::destroy<State>();

        return 0;
    }

    void handleFingerEvent(const SDL_TouchFingerEvent &e)
    {
        PointerAction action; {
            switch (e.type) {
                case SDL_FINGERMOTION: action = PointerAction::Moved; break;
                case SDL_FINGERDOWN  : action = PointerAction::Pressed; break;
                default              : action = PointerAction::Released; break;
            };
        }

        FingerEvent event {
            action,
            e.timestamp / 1000.,
            static_cast<uint64_t>(e.touchId),
            static_cast<uint64_t>(e.fingerId),
            Point {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Step {
                static_cast<double>(e.dx),
                static_cast<double>(e.dy)
            },
            e.pressure
        };

        for (const auto &pair: windows_)
            feedFingerEvent(pair.value(), event);
    }

    void handleMouseMotionEvent(const SDL_MouseMotionEvent &e)
    {
        if (e.which == SDL_TOUCH_MOUSEID) return;

        MouseButton button = MouseButton::None; {
            if (e.state & SDL_BUTTON_LMASK)  button |= MouseButton::Left;
            if (e.state & SDL_BUTTON_RMASK)  button |= MouseButton::Right;
            if (e.state & SDL_BUTTON_MIDDLE) button |= MouseButton::Middle;
            if (e.state & SDL_BUTTON_X1MASK) button |= MouseButton::X1;
            if (e.state & SDL_BUTTON_X2MASK) button |= MouseButton::X2;
        }

        MouseEvent event {
            PointerAction::Moved,
            e.timestamp / 1000.,
            button,
            0,
            Point{
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Step{
                static_cast<double>(e.xrel),
                static_cast<double>(e.yrel)
            }
        };

        SdlWindow window;
        if (windows_.lookup(e.windowID, &window))
            feedMouseEvent(window, event);
    }

    void handleMouseButtonEvent(const SDL_MouseButtonEvent &e)
    {
        if (e.which == SDL_TOUCH_MOUSEID) return;

        PointerAction action =
            (e.type == SDL_MOUSEBUTTONDOWN) ?
            PointerAction::Pressed :
            PointerAction::Released;

        MouseButton button; {
            switch (e.button) {
                case SDL_BUTTON_LEFT  : button = MouseButton::Left; break;
                case SDL_BUTTON_RIGHT : button = MouseButton::Right; break;
                case SDL_BUTTON_MIDDLE: button = MouseButton::Middle; break;
                case SDL_BUTTON_X1    : button = MouseButton::X1; break;
                default               : button = MouseButton::X2; break;
            };
        }

        MouseEvent event {
            action,
            e.timestamp / 1000.,
            button,
            e.clicks,
            Point{
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Step{}
        };

        SdlWindow window;
        if (windows_.lookup(e.windowID, &window)) {
            feedMouseEvent(window, event);
        }
    }

    void handleMouseWheelEvent(const SDL_MouseWheelEvent &e)
    {
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);

        WheelEvent event {
            e.timestamp / 1000.,
            Step{
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Point{
                static_cast<double>(mx),
                static_cast<double>(my)
            }
        };

        SdlWindow window;
        if (windows_.lookup(e.windowID, &window))
            feedWheelEvent(window, event);
    }

    void handleKeyboardEvent(const SDL_KeyboardEvent &e)
    {
        KeyEvent event {
            e.type == SDL_KEYDOWN ? KeyAction::Pressed : KeyAction::Released,
            e.timestamp / 1000.,
            e.repeat,
            static_cast<ScanCode>(e.keysym.scancode),
            static_cast<KeyCode>(e.keysym.sym),
            static_cast<KeyModifier>(e.keysym.mod)
        };

        keyModifiers(event.modifiers());

        SdlWindow window;
        if (windows_.lookup(e.windowID, &window)) {
            if (e.timestamp - window.me().gainFocusTime_ > 100) // workaround: SDL sometimes slips window change keyboard combinations
                feedKeyEvent(window, event);
        }
    }

    void handleWindowEvent(const SDL_WindowEvent &e)
    {
        switch (e.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window)) {
                    window.me().onWindowResized(Size{double(e.data1), double(e.data2)});
                }
                break;
            }
            case SDL_WINDOWEVENT_SHOWN: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window))
                    window.me().onWindowShown();
                break;
            }
            case SDL_WINDOWEVENT_HIDDEN: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window)) {
                    window.me().exposed_ = false;
                    window.me().onWindowHidden();
                }
                break;
            }
            case SDL_WINDOWEVENT_EXPOSED: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window)) {
                    if (!window.me().exposed_) {
                        feedExposedEvent(window);
                        window.me().exposed_ = true;
                    }
                    else {
                        window.me().redraw();
                    }
                }
                break;
            }
            case SDL_WINDOWEVENT_MOVED: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window))
                    window.me().onWindowMoved(Point{double(e.data1), double(e.data2)});
                break;
            }
            case SDL_WINDOWEVENT_ENTER: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window))
                    feedEnterEvent(window);
                break;
            }
            case SDL_WINDOWEVENT_LEAVE: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window))
                    feedLeaveEvent(window);
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
                SdlWindow window;
                if (windows_.lookup(e.windowID, &window)) {
                    focusWindow = window;
                    window.me().gainFocusTime_ = e.timestamp;
                }
                break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST: {
                focusWindow = Window{};
                break;
            }
        };
    }

    void handleTextEditingEvent(const SDL_TextEditingEvent &e)
    {
        feedTextEditingEvent(e.text, e.start, e.length);
    }

    void handleTextInputEvent(const SDL_TextInputEvent &e)
    {
        feedTextInputEvent(e.text);
    }

    String getUserDataPath() const override
    {
        char *s = SDL_GetPrefPath(orgName(), appName());
        String path{s};
        SDL_free(s);
        return path;
    }

    uint32_t timerEvent_;
    uint32_t userEvent_;

    Map<uint32_t, SdlWindow> windows_;

    bool cursorVisible_ { true };
};

SdlApplication::SdlApplication():
    Application{instance<State>()}
{}

void SdlApplication::triggerTimer(const Timer &timer)
{
    if (!timer) return;

    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event));
    event.type = me().timerEvent_;
    event.user.code = 0;
    static_assert(sizeof(event.user.data1) >= sizeof(Timer));
    new(&event.user.data1)Timer{timer};
    SDL_PushEvent(&event);
}

void SdlApplication::registerWindow(const SdlWindow &window)
{
    me().windows_.insert(window.id(), window);
}

const SdlApplication::State &SdlApplication::me() const
{
    return Object::me.as<State>();
}

SdlApplication::State &SdlApplication::me()
{
    return Object::me.as<State>();
}

} // namespace cc
