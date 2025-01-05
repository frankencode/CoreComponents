/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3Application>
#include <cc/Sdl3Window>
#include <cc/Sdl3WindowState>
#include <cc/Sdl3Cursor>
#include <cc/Sdl3PlatformError>
#include <cc/Sdl3TimeMaster>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pen.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_filesystem.h>

namespace cc {

struct Sdl3Application::State: public Application::State
{
    State():
        timerEvent_{SDL_RegisterEvents(2)},
        userEvent_{timerEvent_ + 1},
        cursorVisible_{SDL_CursorVisible()}
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
        return Sdl3Window{view};
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
        SDL_CaptureMouse(on);
    }

    Cursor createCursor(CursorShape shape) override
    {
        return Sdl3Cursor{shape};
    }

    Cursor createCursor(const Image &image, const Point &hotspot) override
    {
        return Sdl3Cursor{image, hotspot};
    }

    void setCursor(const Cursor &cursor) override
    {
        SDL_SetCursor(Sdl3Cursor{cursor}.sdlCursor());
    }

    void unsetCursor() override
    {
        SDL_SetCursor(SDL_GetDefaultCursor());
    }

    void showCursor(bool on) override
    {
        if (cursorVisible_ != on) {
            if (on) SDL_ShowCursor();
            else SDL_HideCursor();
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
        if (!SDL_SetClipboardText(text)) {
            throw Sdl3PlatformError{};
        }
     }

    void startTextInput(const Window &window) override
    {
        textInputWindow_ = window.as<Sdl3Window>().sdlWindow();

        if (SDL_TextInputActive(textInputWindow_)) {
            SDL_StopTextInput(textInputWindow_);
        }
        if (SDL_GetKeyboardFocus() != textInputWindow_) { // FIXME
            SDL_RaiseWindow(textInputWindow_);
        }

        SDL_StartTextInput(textInputWindow_);
    }

    /** \todo input are logic is still incomplete
      */
    void setTextInputArea(const Rect &inputArea) override
    {
        SDL_Rect rect;
        rect.x = inputArea.pos()[0];
        rect.y = inputArea.pos()[1];
        rect.w = inputArea.size()[0];
        rect.h = inputArea.size()[1];
        SDL_SetTextInputArea(textInputWindow_, &rect, 0/*FIXME*/);
    }

    void stopTextInput() override
    {
        SDL_StopTextInput(textInputWindow_);
    }

    String getUserDataPath() const override
    {
        char *s = SDL_GetPrefPath(orgName(), appName());
        String path{s};
        SDL_free(s);
        return path;
    }

    void postEvent(Function<void()> &&doNext) override
    {
        SDL_Event event;
        ::memset(&event, 0, sizeof(SDL_Event));
        event.type = userEvent_;
        event.user.data1 = new Function<void()>{move(doNext)};
        SDL_PushEvent(&event);
    }

    void handlePenMotionEvent(const SDL_PenMotionEvent &e)
    {
        PenState &pen = pens_(e.which);

        PenEvent event {
            pen.down_ ? PenAction::Moved : PenAction::Hovered,
            e.which,
            e.timestamp / 1e9,
            static_cast<PenButton>(e.pen_state),
            PenButton::None,
            Point {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            pen.down_ ? pen.pressure_ : 0.
        };

        for (const auto &pair: windows_) {
            feedPenEvent(pair.value(), event);
        }
    }

    void handlePenTouchEvent(const SDL_PenTouchEvent &e)
    {
        const bool down = (e.type == SDL_EVENT_PEN_DOWN);
        penDown_ += 2 * down - 1;

        PenState &pen = pens_(e.which);
        pen.down_ = down;

        if (!down) {
            PenEvent event {
                down ? PenAction::Moved : PenAction::Hovered,
                e.which,
                e.timestamp / 1e9,
                static_cast<PenButton>(e.pen_state),
                PenButton::None,
                Point {
                    static_cast<double>(e.x),
                    static_cast<double>(e.y)
                },
                down ? pen.pressure_ : 0.
            };

            for (const auto &pair: windows_) {
                feedPenEvent(pair.value(), event);
            }
        }
    }

    void handlePenButtonEvent(const SDL_PenButtonEvent &e)
    {
        PenEvent event {
            (e.type == SDL_EVENT_PEN_BUTTON_DOWN) ? PenAction::Pressed : PenAction::Released,
            e.which,
            e.timestamp / 1e9,
            static_cast<PenButton>(e.pen_state),
            static_cast<PenButton>(e.button),
            Point {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            pens_(e.which).pressure_
        };

        for (const auto &pair: windows_) {
            feedPenEvent(pair.value(), event);
        }
    }

    void handlePenAxisEvent(const SDL_PenAxisEvent &e)
    {
        if (e.axis == SDL_PEN_AXIS_PRESSURE) {
            pens_(e.which).pressure_ = static_cast<double>(e.value);
        }
    }

    void handleFingerEvent(const SDL_TouchFingerEvent &e)
    {
        if (penDown_) return;

        PointerAction action; {
            switch (e.type) {
                case SDL_EVENT_FINGER_MOTION: action = PointerAction::Moved; break;
                case SDL_EVENT_FINGER_DOWN  : action = PointerAction::Pressed; break;
                default                     : action = PointerAction::Released; break;
            };
        }

        FingerEvent event {
            action,
            e.timestamp / 1e9,
            e.touchID,
            e.fingerID,
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

        for (const auto &pair: windows_) {
            feedFingerEvent(pair.value(), event);
        }
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
            e.timestamp / 1e9,
            button,
            0,
            Point {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Step {
                static_cast<double>(e.xrel),
                static_cast<double>(e.yrel)
            }
        };

        if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
            feedMouseEvent(window, event);
        }
    }

    void handleMouseButtonEvent(const SDL_MouseButtonEvent &e)
    {
        if (e.which == SDL_TOUCH_MOUSEID) return;

        PointerAction action =
            (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ?
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
            e.timestamp / 1e9,
            button,
            e.clicks,
            Point {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Step{}
        };

        if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
            feedMouseEvent(window, event);
        }
    }

    void handleMouseWheelEvent(const SDL_MouseWheelEvent &e)
    {
        float mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);

        WheelEvent event {
            e.timestamp / 1e9,
            Step {
                static_cast<double>(e.x),
                static_cast<double>(e.y)
            },
            Point {
                static_cast<double>(mx),
                static_cast<double>(my)
            }
        };

        if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
            feedWheelEvent(window, event);
        }
    }

    void handleKeyboardEvent(const SDL_KeyboardEvent &e)
    {
        KeyEvent event {
            e.type == SDL_EVENT_KEY_DOWN ? KeyAction::Pressed : KeyAction::Released,
            e.timestamp / 1e9,
            e.repeat,
            static_cast<ScanCode>(e.scancode),
            static_cast<KeyCode>(e.key),
            static_cast<KeyModifier>(e.mod)
        };

        keyModifiers(event.modifiers());

        if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
            if (e.timestamp - window.me().gainFocusTime_ > 100) // workaround: SDL sometimes slips window change keyboard combinations
                feedKeyEvent(window, event);
        }
    }

    void handleWindowEvent(const SDL_WindowEvent &e)
    {
        switch (e.type) {
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    window.me().onWindowResized({
                        static_cast<double>(e.data1),
                        static_cast<double>(e.data2)
                    });
                }
                break;
            }
            case SDL_EVENT_WINDOW_SHOWN: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    window.me().onWindowShown();
                }
                break;
            }
            case SDL_EVENT_WINDOW_HIDDEN: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    window.me().exposed_ = false;
                    window.me().onWindowHidden();
                }
                break;
            }
            case SDL_EVENT_WINDOW_EXPOSED: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
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
            case SDL_EVENT_WINDOW_MOVED: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    window.me().onWindowMoved({
                        static_cast<double>(e.data1),
                        static_cast<double>(e.data2)
                    });
                }
                break;
            }
            case SDL_EVENT_WINDOW_MOUSE_ENTER: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    feedEnterEvent(window);
                }
                break;
            }
            case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    feedLeaveEvent(window);
                }
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                if (Sdl3Window window; windows_.lookup(e.windowID, &window)) {
                    focusWindow = window;
                    window.me().gainFocusTime_ = e.timestamp;
                }
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_LOST: {
                focusWindow = Window{};
                break;
            }
            default: {
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

    int run() override
    {
        for (SDL_Event event; SDL_WaitEvent(&event) && event.type != SDL_EVENT_QUIT;)
        {
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
                case SDL_EVENT_PEN_MOTION:
                case SDL_EVENT_MOUSE_MOTION:
                case SDL_EVENT_FINGER_MOTION:
                case SDL_EVENT_PEN_AXIS:
                    do {
                        if (event.type == SDL_EVENT_PEN_MOTION)
                            handlePenMotionEvent(event.pmotion);
                        else if (event.type == SDL_EVENT_MOUSE_MOTION)
                            handleMouseMotionEvent(event.motion);
                        else if (event.type == SDL_EVENT_FINGER_MOTION)
                            handleFingerEvent(event.tfinger);
                        else
                            handlePenAxisEvent(event.paxis);
                    } while (
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_EVENT_PEN_MOTION, SDL_EVENT_PEN_MOTION
                        ) > 0 ||
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_EVENT_MOUSE_MOTION, SDL_EVENT_MOUSE_MOTION
                        ) > 0 ||
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_EVENT_FINGER_MOTION, SDL_EVENT_FINGER_MOTION
                        ) > 0 ||
                        SDL_PeepEvents(
                            &event, 1, SDL_GETEVENT,
                            SDL_EVENT_PEN_AXIS, SDL_EVENT_PEN_AXIS
                        ) > 0
                    );
                    break;
                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                    handlePenTouchEvent(event.ptouch);
                    break;
                case SDL_EVENT_PEN_BUTTON_DOWN:
                case SDL_EVENT_PEN_BUTTON_UP:
                    handlePenButtonEvent(event.pbutton);
                    break;
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                    handleFingerEvent(event.tfinger);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    handleMouseButtonEvent(event.button);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    handleMouseWheelEvent(event.wheel);
                    break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    handleKeyboardEvent(event.key);
                    break;
                case SDL_EVENT_TEXT_EDITING:
                    handleTextEditingEvent(event.edit);
                    break;
                case SDL_EVENT_TEXT_INPUT:
                    handleTextInputEvent(event.text);
                    break;
                default:
                    if (SDL_EVENT_WINDOW_FIRST <= event.type && event.type <= SDL_EVENT_WINDOW_LAST) {
                        handleWindowEvent(event.window);
                    }
                    break;
            };

            for (const auto &pair: windows_) {
                Sdl3Window{pair.value()}.me().commitFrame();
            }
        }

        Sdl3TimeMaster::shutdown();
        Singleton::destroy<State>();

        return 0;
    }

    uint32_t timerEvent_ { 0 };
    uint32_t userEvent_ { 0 };

    Map<uint32_t, Sdl3Window> windows_;

    struct PenState {
        double pressure_ { 0 };
        bool down_ { false };
    };

    Map<uint32_t, PenState> pens_;
    int penDown_ { 0 };

    SDL_Window *textInputWindow_ { nullptr };

    KeyModifier modifiers_ { KeyModifier::None };

    bool cursorVisible_ { true };
};

Sdl3Application::Sdl3Application():
    Application{instance<State>()}
{}

void Sdl3Application::triggerTimer(const Timer &timer)
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

void Sdl3Application::registerWindow(const Sdl3Window &window)
{
    me().windows_.insert(window.id(), window);
}

const Sdl3Application::State &Sdl3Application::me() const
{
    return Object::me.as<State>();
}

Sdl3Application::State &Sdl3Application::me()
{
    return Object::me.as<State>();
}

} // namespace cc
