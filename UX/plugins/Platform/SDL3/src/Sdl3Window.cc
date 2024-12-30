/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sdl3Window>
#include <cc/Sdl3PlatformError>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

namespace cc {

static_assert(+WindowMode::Fullscreen        == SDL_WINDOW_FULLSCREEN);
static_assert(+WindowMode::Borderless        == SDL_WINDOW_BORDERLESS);
static_assert(+WindowMode::Resizable         == SDL_WINDOW_RESIZABLE);
static_assert(+WindowMode::Minimized         == SDL_WINDOW_MINIMIZED);
static_assert(+WindowMode::Maximized         == SDL_WINDOW_MAXIMIZED);
static_assert(+WindowMode::MouseGrabbed      == SDL_WINDOW_MOUSE_GRABBED);
static_assert(+WindowMode::KeyboardGrabbed   == SDL_WINDOW_KEYBOARD_GRABBED);
static_assert(+WindowMode::InputFocus        == SDL_WINDOW_INPUT_FOCUS);
static_assert(+WindowMode::MouseFocus        == SDL_WINDOW_MOUSE_FOCUS);
static_assert(+WindowMode::HighDpi           == SDL_WINDOW_HIGH_PIXEL_DENSITY);
static_assert(+WindowMode::MouseCapture      == SDL_WINDOW_MOUSE_CAPTURE);
static_assert(+WindowMode::MouseRelativeMode == SDL_WINDOW_MOUSE_RELATIVE_MODE);
static_assert(+WindowMode::AlwaysOnTop       == SDL_WINDOW_ALWAYS_ON_TOP);
static_assert(+WindowMode::Utility           == SDL_WINDOW_UTILITY);
static_assert(+WindowMode::Tooltip           == SDL_WINDOW_TOOLTIP);
static_assert(+WindowMode::PopupMenu         == SDL_WINDOW_POPUP_MENU);
static_assert(+WindowMode::Transparent       == SDL_WINDOW_TRANSPARENT);
static_assert(+WindowMode::Occluded          == SDL_WINDOW_OCCLUDED);
static_assert(+WindowMode::NotFocusable      == SDL_WINDOW_NOT_FOCUSABLE);
static_assert(+WindowMode::OpenGl            == SDL_WINDOW_OPENGL);
static_assert(+WindowMode::Vulkan            == SDL_WINDOW_VULKAN);
static_assert(+WindowMode::Metal             == SDL_WINDOW_METAL);

struct Sdl3Window::State final: public Window::State
{
    State(const View &view):
        Window::State{view}
    {
        minSizeMonitor([this]{
            if (visible()) setMinSize(view_.minSize());
        });

        maxSizeMonitor([this]{
            if (visible()) setMaxSize(view_.maxSize());
        });
    }

    ~State()
    {
        if (icon_) SDL_DestroySurface(icon_);
    }

    void setMinSize(Size size)
    {
        if (
            size[0] == 0 ||
            size[1] == 0
        ) {
            return;
        }
        const int minWidth = static_cast<int>(std::ceil(size[0]));
        const int minHeight = static_cast<int>(std::ceil(size[1]));
        SDL_SetWindowMinimumSize(window_, minWidth, minHeight);
    }

    void setMaxSize(Size size)
    {
        if (
            size[0] == std::numeric_limits<double>::max() ||
            size[1] == std::numeric_limits<double>::max()
        ) {
            return;
        }
        const int maxWidth = static_cast<int>(std::ceil(size[0]));
        const int maxHeight = static_cast<int>(std::ceil(size[1]));
        SDL_SetWindowMaximumSize(window_, maxWidth, maxHeight);
    }

    void createWindow()
    {
        if (!SDL_CreateWindowAndRenderer(title(), size()[0], size()[1], +mode_, &window_, &renderer_)) {
            throw Sdl3PlatformError{};
        }

        id_ = SDL_GetWindowID(window_);

        currentSize_ = size();
        {
            int x = 0, y = 0;
            SDL_GetWindowPosition(window_, &x, &y);
            pos = Point { static_cast<double>(x), static_cast<double>(y) };
        }

        title.onChanged([this]{ SDL_SetWindowTitle(window_, title()); });

        // TODO...
        // if (icon()) setWindowIcon(icon());
        // icon.onChanged([this]{ setWindowIcon(icon()); });

        size.onChanged([this]{
            if (size() != currentSize_) {
                SDL_SetWindowSize(window_, size()[0], size()[1]);
            }
        });

        pos.onChanged([this]{
            if (pos() != currentPos_) {
                SDL_SetWindowPosition(window_, pos()[0], pos()[1]);
            }
        });

        // TODO...
        // Sdl3Application{}.registerWindow(alias<SdlWindow>(this));
    }

    void show(int display) override
    {
        if (window_) SDL_ShowWindow(window_);
        else createWindow();
    }

    void hide() override
    {
        if (window_) SDL_HideWindow(window_);
    }

    void raise() override
    {
        if (window_ && visible()) SDL_RaiseWindow(window_);
    }

    void setOpacity(double opacity) override
    {
        if (window_) SDL_SetWindowOpacity(window_, opacity);
    }

    void renderFrame(const Frame &frame) override
    {
        // TODO...
    }

    void renderViewToImage(const View &view, Image &image) override
    {
        // TODO...
    }

    Property<void> minSizeMonitor;
    Property<void> maxSizeMonitor;

    SDL_Window *window_ { nullptr };
    SDL_Renderer *renderer_ { nullptr };
    SDL_Surface *icon_ { nullptr };

    SDL_WindowID id_ { 0 };

    Size currentSize_;
    Point currentPos_;
};

Sdl3Window::Sdl3Window(const View &view):
    Window{new State{view}}
{}

uint32_t Sdl3Window::id() const
{
    return me().id_;
}

SDL_Window *Sdl3Window::sdlWindow() const
{
    return me().window_;
}

Sdl3Window::State &Sdl3Window::me()
{
    return Object::me.as<State>();
}

const Sdl3Window::State &Sdl3Window::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
