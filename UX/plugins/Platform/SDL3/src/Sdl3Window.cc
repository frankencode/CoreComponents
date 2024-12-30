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

class Sdl3Context final: public Object
{
public:
    struct State final: public Object::State
    {
        ~State() {
            if (texture_) SDL_DestroyTexture(texture_);
        }

        SDL_Texture *texture_ { nullptr };
        int textureWidth_ { 0 };
        int textureHeight_ { 0 };
    };

    Sdl3Context():
        Object{new State}
    {}

    State &me() { return Object::me.as<State>(); }
    const State &me() const { return Object::me.as<State>(); }
};

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

    void setWindowIcon(const Image &image)
    {
        SDL_Surface *oldIcon = icon_;

        iconImage_ = image;
        icon_ = SDL_CreateSurfaceFrom(
            image.width(),
            image.height(),
            SDL_PIXELFORMAT_ARGB8888,
            iconImage_.data().bytes(),
            image.pitch()
        );

        if (!icon_) throw Sdl3PlatformError{};

        SDL_SetWindowIcon(window_, icon_);

        if (oldIcon) SDL_DestroySurface(oldIcon);
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

    void createWindow()
    {
        if (!SDL_CreateWindowAndRenderer(title(), size()[0], size()[1], +mode_, &window_, &renderer_)) {
            throw Sdl3PlatformError{};
        }

        waitVSync_ =
            SDL_SetRenderVSync(renderer_, SDL_RENDERER_VSYNC_ADAPTIVE) ||
            SDL_SetRenderVSync(renderer_, 1);

        id_ = SDL_GetWindowID(window_);

        currentSize_ = size();
        {
            int x = 0, y = 0;
            SDL_GetWindowPosition(window_, &x, &y);
            pos = Point { static_cast<double>(x), static_cast<double>(y) };
        }

        title.onChanged([this]{ SDL_SetWindowTitle(window_, title()); });

        if (icon()) setWindowIcon(icon());
        icon.onChanged([this]{ setWindowIcon(icon()); });

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

    void renderViewToImage(const View &view, Image &image) override
    {
        assert(!image.isNull());

        renderCascade(renderer_, view_);

        Point origin = view.mapToGlobal(Point{});
        SDL_Rect rect;
        rect.x = std::floor(origin[0]);
        rect.y = std::floor(origin[1]);
        rect.w = image.width();
        rect.h = image.height();

        SDL_Surface *surface = SDL_RenderReadPixels(renderer_,  &rect);
        if (!surface) throw Sdl3PlatformError{};

        if (surface->format != SDL_PIXELFORMAT_ARGB8888) {
            SDL_Surface *oldSurface = surface;
            surface = SDL_ConvertSurface(oldSurface, SDL_PIXELFORMAT_ARGB8888);
            SDL_DestroySurface(oldSurface);
        }

        if (image.width() == surface->w && image.height() == surface->h && image.pitch() == surface->pitch) {
            std::memcpy(image.data().bytes(), surface->pixels, image.pitch() * image.height());
        }
        else {
            const uint8_t *src = static_cast<const uint8_t*>(surface->pixels);
            uint8_t *dst = image.data().bytes();
            for (int y = 0; y < image.height(); ++y) {
                std::memcpy(dst, src, image.pitch());
                src += surface->pitch;
                dst += image.pitch();
            }
        }

        SDL_DestroySurface(surface);
    }

    void renderFrame(const Frame &frame) override
    {
        for (const UpdateRequest &request: frame)
        {
            switch (request.reason()) {
                case UpdateReason::Changed:
                case UpdateReason::Resized:
                    updateTexture(renderer_, request.view());
                    break;
                case UpdateReason::Faded:
                    updateOpacity(request.view());
                    break;
                default:;
            };
        }

        renderCascade(renderer_, view_);

        SDL_RenderPresent(renderer_);
    }

    static void updateOpacity(View view)
    {
        if (!viewContext(view)) viewContext(view) = Sdl3Context{};
        Sdl3Context::State &context = viewContext(view).as<Sdl3Context>().me();

        if (context.texture_) {
            if (!SDL_SetTextureAlphaMod(context.texture_, 0xFF * view.opacity())) {
                throw Sdl3PlatformError{};
            }
        }
    }

    static void renderCascade(SDL_Renderer *renderer, const View &view)
    {
        if (!view.visible() || view.opacity() <= 0) return;

        {
            View decoration = viewState(view).decoration();
            if (decoration) renderCascade(renderer, decoration);
        }

        renderTexture(renderer, view);

        bool clip = view.clip() && view.childrenCount() > 0;
        if (clip) {
            SDL_Rect clipRect;
            Point pos = view.mapToGlobal(Point{0, 0});
            clipRect.x = std::floor(pos[0]);
            clipRect.y = std::floor(pos[1]);
            Size size = view.size();
            size *= view.scale();
            clipRect.w = std::ceil(size[0]);
            clipRect.h = std::ceil(size[1]);
            SDL_SetRenderClipRect(renderer, &clipRect);
        }

        for (const View &child: view.visibleChildren()) {
            renderCascade(renderer, child);
        }

        if (clip) {
            SDL_SetRenderClipRect(renderer, nullptr);
        }
    }

    static void renderTexture(SDL_Renderer *renderer, const View &view)
    {
        if (!viewContext(view)) return;

        auto sdlContext = viewContext(view).as<Sdl3Context>();
        const Sdl3Context::State &context = sdlContext.me();
        SDL_Texture *texture = context.texture_;

        if (!texture) return;

        SDL_FRect destRect;
        destRect.w = context.textureWidth_;
        destRect.h = context.textureHeight_;
        if (view.scale() != 1) {
            destRect.w *= view.scale();
            destRect.h *= view.scale();
        }
        destRect.w = std::ceil(destRect.w);
        destRect.h = std::ceil(destRect.h);

        if (viewState(view).hasParent())
        {
            if (view.moving()) {
                destRect.x = view.pos()[0];
                destRect.y = view.pos()[1];
            }
            else {
                destRect.x = std::round(view.pos()[0]);
                destRect.y = std::round(view.pos()[1]);
            }

            for (auto ancestor = parent(view); ancestor; ancestor = parent(ancestor))
            {
                if (ancestor->moving()) {
                    destRect.x += ancestor->pos()[0];
                    destRect.y += ancestor->pos()[1];
                }
                else {
                    destRect.x += std::round(ancestor->pos()[0]);
                    destRect.y += std::round(ancestor->pos()[1]);
                }
            }
        }
        else {
            destRect.x = 0;
            destRect.y = 0;
        }

        double angle = view.angle();
        if (angle != 0) angle = std::fmod(angle, 360);
        if (angle == 0) {
            SDL_RenderTexture(renderer, texture, nullptr, &destRect);
        }
        else {
            Point pivot = view.pivot();
            SDL_FPoint sdlPivot{ float(pivot[0]), float(pivot[1]) };
            SDL_RenderTextureRotated(renderer, texture, nullptr, &destRect, view.angle(), &sdlPivot, SDL_FLIP_NONE);
        }
    }

    void updateTexture(SDL_Renderer *sdlRenderer, View view)
    {
        if (!viewContext(view)) viewContext(view) = Sdl3Context{};
        Sdl3Context::State &context = viewContext(view).as<Sdl3Context>().me();

        Image image = viewState(view).image();
        bool hasPixels = viewState(view).isPainted() && image.count() > 0;

        if (
            (
                !hasPixels || (
                    context.textureWidth_ != image.width() ||
                    context.textureHeight_ != image.height()
                )
            ) && context.texture_
        ) {
            SDL_DestroyTexture(context.texture_);
            context.texture_ = nullptr;
        }

        if (!hasPixels) return;

        if (!context.texture_) {
            context.texture_ = SDL_CreateTexture(
                sdlRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                viewState(view).isStatic() ? SDL_TEXTUREACCESS_STATIC : SDL_TEXTUREACCESS_STREAMING,
                image.width(),
                image.height()
            );
            if (!context.texture_) throw Sdl3PlatformError{};
            context.textureWidth_ = image.width();
            context.textureHeight_ = image.height();
            double opacity = viewState(view).opacity();
            SDL_SetTextureBlendMode(
                context.texture_,
                (viewState(view).isOpaque() && opacity == 1.) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND
            );

            if (opacity != 1.) {
                if (!SDL_SetTextureAlphaMod(context.texture_, 0xFF * opacity)) {
                    throw Sdl3PlatformError{};
                }
            }
        }

        if (viewState(view).isStatic()) {
            if (!SDL_UpdateTexture(context.texture_, nullptr, image.data(), image.pitch())) {
                throw Sdl3PlatformError{};
            }
        }
        else {
            void *dstData = nullptr;
            int pitch = 0;
            if (!SDL_LockTexture(context.texture_, 0, &dstData, &pitch)) throw Sdl3PlatformError{};
            assert(pitch == image.pitch());
            ::memcpy(dstData, image.data(), image.data().count());
            SDL_UnlockTexture(context.texture_);
        }
    }

    Property<void> minSizeMonitor;
    Property<void> maxSizeMonitor;

    SDL_Window *window_ { nullptr };
    SDL_Renderer *renderer_ { nullptr };
    SDL_Surface *icon_ { nullptr };
    Image iconImage_;

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
