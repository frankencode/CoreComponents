/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SdlWindow>
#include <cc/SdlApplication>
#include <cc/SdlPlatformError>
#include <cc/DEBUG>
#ifndef NDEBUG
#include <cc/stdio>
#endif
#include <cassert>

namespace cc {

class SdlContext final: public Object
{
public:
    struct State final: public Object::State
    {
        ~State() {
            if (sdlTexture_) SDL_DestroyTexture(sdlTexture_);
        }

        SDL_Texture *sdlTexture_ { nullptr };
        int sdlTextureWidth_ { 0 };
        int sdlTextureHeight_ { 0 };
    };

    SdlContext():
        Object{new State}
    {}

    State &me() { return Object::me.as<State>(); }
    const State &me() const { return Object::me.as<State>(); }
};

SdlWindow::State::State(const View &view):
    Window::State{view}
{
    minSizeMonitor([this]{
        if (visible()) setMinSize(view_.minSize());
    });

    maxSizeMonitor([this]{
        if (visible()) setMaxSize(view_.maxSize());
    });
}

SdlWindow::State::~State()
{
    if (sdlIcon_) {
        SDL_FreeSurface(sdlIcon_);
    }
}

void SdlWindow::State::onWindowResized(Size newSize)
{
    sizing = true;
    currentSize_ = newSize;
    size = newSize;
    sizing = false;
}

void SdlWindow::State::onWindowMoved(Point newPos)
{
    currentPos_ = newPos;
    pos = newPos;
}

void SdlWindow::State::onWindowShown()
{
    visible = true;
}

void SdlWindow::State::onWindowHidden()
{
    visible = false;
}

void SdlWindow::State::redraw()
{
    renderCascade(sdlRenderer_, view_);
    SDL_RenderPresent(sdlRenderer_);
}

void SdlWindow::State::show(int display)
{
    if (primordial_) {
        Uint32 flags = 0;
        if (mode_ & WindowMode::Fullscreen)        flags |= SDL_WINDOW_FULLSCREEN;
        if (mode_ & WindowMode::FullscreenDesktop) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        if (mode_ & WindowMode::OpenGl)            flags |= SDL_WINDOW_OPENGL;
        if (mode_ & WindowMode::Vulkan)            flags |= SDL_WINDOW_VULKAN;
        if (mode_ & WindowMode::Borderless)        flags |= SDL_WINDOW_BORDERLESS;
        if (!(mode_ & WindowMode::FixedSize))      flags |= SDL_WINDOW_RESIZABLE;
        if (mode_ & WindowMode::Minimized)         flags |= SDL_WINDOW_MINIMIZED;
        if (mode_ & WindowMode::Maximized)         flags |= SDL_WINDOW_MAXIMIZED;
        if (mode_ & WindowMode::InputGrabbed)      flags |= SDL_WINDOW_INPUT_GRABBED;
        if (mode_ & WindowMode::InputFocus)        flags |= SDL_WINDOW_INPUT_FOCUS;
        if (mode_ & WindowMode::MouseFocus)        flags |= SDL_WINDOW_MOUSE_FOCUS;
        #ifdef SDL_WINDOW_ALLOW_HIGHDPI
        if (mode_ & WindowMode::AllowHighDpi)      flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        #endif
        #ifdef SDL_WINDOW_MOUSE_CAPTURE
        if (mode_ & WindowMode::MouseCapture)      flags |= SDL_WINDOW_MOUSE_CAPTURE;
        #endif
        #ifdef SDL_WINDOW_ALWAYS_ON_TOP
        if (mode_ & WindowMode::AlwaysOnTop)       flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        #endif
        #ifdef SDL_WINDOW_SKIP_TASKBAR
        if (mode_ & WindowMode::SkipTaskBar)       flags |= SDL_WINDOW_SKIP_TASKBAR;
        #endif
        #ifdef SDL_WINDOW_UTILITY
        if (mode_ & WindowMode::Utility)           flags |= SDL_WINDOW_UTILITY;
        #endif
        #ifdef SDL_WINDOW_TOOLTIP
        if (mode_ & WindowMode::Tooltip)           flags |= SDL_WINDOW_TOOLTIP;
        #endif
        #ifdef SDL_WINDOW_POPUP_MENU
        if (mode_ & WindowMode::PopupMenu)         flags |= SDL_WINDOW_POPUP_MENU;
        #endif

        {
            int x = pos()[0], y = pos()[1], w = size()[0], h = size()[1];
            if (pos() == Point{}) {
                x = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
                y = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
            }
            sdlWindow_ = SDL_CreateWindow(title(), x, y, w, h, flags);
            if (!sdlWindow_) throw SdlPlatformError{};
        }

        id_ = SDL_GetWindowID(sdlWindow_);

        currentSize_ = size();
        {
            int x = 0, y = 0;
            SDL_GetWindowPosition(sdlWindow_, &x, &y);
            pos = Point{double(x), double(y)};
        }

        if (
            (mode_ & WindowMode::Accelerated) ||
            (mode_ & WindowMode::Fullscreen)  ||
            (mode_ & WindowMode::FullscreenDesktop)
        )
            sdlRenderer_ = SDL_CreateRenderer(
                sdlWindow_,
                -1,
                ((mode_ & WindowMode::Accelerated) ? SDL_RENDERER_ACCELERATED  : 0) |
                ((mode_ & WindowMode::VSync      ) ? SDL_RENDERER_PRESENTVSYNC : 0)
            );
        else
            sdlRenderer_ = SDL_CreateRenderer(sdlWindow_, -1, 0 /*SDL_RENDERER_SOFTWARE*/);

        if (!sdlRenderer_) throw SdlPlatformError{};

        #ifndef NDEBUG
        {
            SDL_RendererInfo info_, *info = &info_;
            if (SDL_GetRendererInfo(sdlRenderer_, info) != 0) throw SdlPlatformError{};
            fout() << "SDL renderer: " << info->name << " (" << ((info->flags & SDL_RENDERER_ACCELERATED) ? "ACCELERATED" : "SOFTWARE") << ")" << nl;
        }
        #endif

        title.onChanged([this]{ SDL_SetWindowTitle(sdlWindow_, title()); });

        if (icon()) setWindowIcon(icon());
        icon.onChanged([this]{ setWindowIcon(icon()); });

        size.onChanged([this]{
            if (size() != currentSize_) {
                SDL_SetWindowSize(sdlWindow_, size()[0], size()[1]);
            }
        });

        pos.onChanged([this]{
            if (pos() != currentPos_) {
                SDL_SetWindowPosition(sdlWindow_, pos()[0], pos()[1]);
            }
        });

        primordial_ = false;
        SdlApplication{}.registerWindow(alias<SdlWindow>(this));
    }
    else {
        SDL_ShowWindow(sdlWindow_);
    }
}

void SdlWindow::State::hide()
{
    if (!primordial_)
        SDL_HideWindow(sdlWindow_);
}

void SdlWindow::State::raise()
{
    if (!primordial_ && visible())
        SDL_RaiseWindow(sdlWindow_);
}

void SdlWindow::State::setWindowIcon(const Image &image)
{
    SDL_Surface *oldIcon = sdlIcon_;

    sdlIcon_ = SDL_CreateRGBSurfaceFrom(
        const_cast<uint8_t *>(image.data().bytes()),
        image.width(),
        image.height(),
        sizeof(Color) * 8,
        image.pitch(),
        Color::RedMask,
        Color::GreenMask,
        Color::BlueMask,
        Color::AlphaMask
    );

    if (!sdlIcon_) throw SdlPlatformError{};

    SDL_SetWindowIcon(sdlWindow_, sdlIcon_);

    if (oldIcon) SDL_FreeSurface(oldIcon);
}

void SdlWindow::State::setOpacity(double opacity)
{
    SDL_SetWindowOpacity(sdlWindow_, opacity);
}

void SdlWindow::State::setMinSize(Size size)
{
    if (
        size[0] == 0 ||
        size[1] == 0
    ) {
        return;
    }
    const int minWidth = static_cast<int>(std::ceil(size[0]));
    const int minHeight = static_cast<int>(std::ceil(size[1]));
    SDL_SetWindowMinimumSize(sdlWindow_, minWidth, minHeight);
}

void SdlWindow::State::setMaxSize(Size size)
{
    if (
        size[0] == std::numeric_limits<double>::max() ||
        size[1] == std::numeric_limits<double>::max()
    ) {
        return;
    }
    const int maxWidth = static_cast<int>(std::ceil(size[0]));
    const int maxHeight = static_cast<int>(std::ceil(size[1]));
    SDL_SetWindowMaximumSize(sdlWindow_, maxWidth, maxHeight);
}

void SdlWindow::State::renderFrame(const Frame &frame)
{
    for (const UpdateRequest &request: frame)
    {
        switch (request.reason()) {
            case UpdateReason::Changed:
            case UpdateReason::Resized:
                updateTexture(sdlRenderer_, request.view());
                break;
            case UpdateReason::Faded:
                updateOpacity(request.view());
                break;
            default:;
        };
    }

    renderCascade(sdlRenderer_, view_);

    SDL_RenderPresent(sdlRenderer_);
}

void SdlWindow::State::renderViewToImage(const View &view, Image &image)
{
    assert(!image.isNull());

    renderCascade(sdlRenderer_, view_);

    Point origin = view.mapToGlobal(Point{});
    SDL_Rect rect;
    rect.x = std::floor(origin[0]);
    rect.y = std::floor(origin[1]);
    rect.w = image.width();
    rect.h = image.height();
    if (
        SDL_RenderReadPixels(
            sdlRenderer_,
            &rect,
            SDL_PIXELFORMAT_ARGB8888,
            image.data().bytes(),
            image.pitch()
        ) != 0
    ) {
        throw SdlPlatformError{};
    }

    SDL_RenderPresent(sdlRenderer_);
}

void SdlWindow::State::updateTexture(SDL_Renderer *sdlRenderer, View view)
{
    if (!viewContext(view)) viewContext(view) = SdlContext{};
    SdlContext::State &context = viewContext(view).as<SdlContext>().me();

    Image image = viewState(view).image();
    bool hasPixels = viewState(view).isPainted() && image.count() > 0;

    if (
        (
            !hasPixels || (
                context.sdlTextureWidth_ != image.width() ||
                context.sdlTextureHeight_ != image.height()
            )
        ) && context.sdlTexture_
    ) {
        SDL_DestroyTexture(context.sdlTexture_);
        context.sdlTexture_ = nullptr;
    }

    if (!hasPixels) return;

    if (!context.sdlTexture_) {
        context.sdlTexture_ = SDL_CreateTexture(
            sdlRenderer,
            SDL_PIXELFORMAT_ARGB8888,
            viewState(view).isStatic() ? SDL_TEXTUREACCESS_STATIC : SDL_TEXTUREACCESS_STREAMING,
            image.width(),
            image.height()
        );
        if (!context.sdlTexture_) throw SdlPlatformError{};
        context.sdlTextureWidth_ = image.width();
        context.sdlTextureHeight_ = image.height();
        double opacity = viewState(view).opacity();
        SDL_SetTextureBlendMode(
            context.sdlTexture_,
            (viewState(view).isOpaque() && opacity == 1.) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND
        );

        if (opacity != 1.) {
            if (SDL_SetTextureAlphaMod(context.sdlTexture_, 0xFF * opacity))
                throw SdlPlatformError{};
        }
    }

    if (viewState(view).isStatic()) {
        if (SDL_UpdateTexture(context.sdlTexture_, 0, image.data(), image.pitch()) != 0)
            throw SdlPlatformError{};
    }
    else {
        void *dstData = 0;
        int pitch = 0;
        if (SDL_LockTexture(context.sdlTexture_, 0, &dstData, &pitch) != 0) throw SdlPlatformError{};
        assert(pitch == image.pitch());
        ::memcpy(dstData, image.data(), image.data().count());
        SDL_UnlockTexture(context.sdlTexture_);
    }
}

void SdlWindow::State::updateOpacity(View view)
{
    if (!viewContext(view)) viewContext(view) = SdlContext{};
    SdlContext::State &context = viewContext(view).as<SdlContext>().me();

    if (context.sdlTexture_) {
        if (SDL_SetTextureAlphaMod(context.sdlTexture_, 0xFF * view.opacity()) < 0)
            throw SdlPlatformError{};
    }
}

void SdlWindow::State::renderCascade(SDL_Renderer *sdlRenderer, View &view)
{
    if (!view.visible() || view.opacity() <= 0) return;

    {
        View decoration = viewState(view).decoration();
        if (decoration) renderCascade(sdlRenderer, decoration);
    }

    renderTexture(sdlRenderer, view);

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
        SDL_RenderSetClipRect(sdlRenderer_, &clipRect);
    }

    for (View child: view.visibleChildren()) // FIXME: performance
        renderCascade(sdlRenderer, child);

    if (clip)
        SDL_RenderSetClipRect(sdlRenderer_, nullptr);
}

void SdlWindow::State::renderTexture(SDL_Renderer *sdlRenderer, const View &view)
{
    if (!viewContext(view)) return;

    auto sdlContext = viewContext(view).as<SdlContext>(); // FIXME: slightly costly;)
    const SdlContext::State &context = sdlContext.me();
    SDL_Texture *sdlTexture = context.sdlTexture_;

    if (!sdlTexture) return;

    SDL_FRect destRect;
    destRect.w = context.sdlTextureWidth_;
    destRect.h = context.sdlTextureHeight_;
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
        SDL_RenderCopyF(sdlRenderer, sdlTexture, nullptr, &destRect);
    }
    else {
        Point pivot = view.pivot();
        SDL_FPoint sdlPivot{ float(pivot[0]), float(pivot[1]) };
        SDL_RenderCopyExF(sdlRenderer, sdlTexture, nullptr, &destRect, view.angle(), &sdlPivot, SDL_FLIP_NONE);
    }
}

} // namespace cc
