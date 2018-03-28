/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/stdio>
#include <cc/debug> // DEBUG
#include <cc/Queue>
#include <cc/ui/Image>
#include <cc/ui/SdlContext>
#include <cc/ui/SdlWindow>

namespace cc {
namespace ui {

Ref<SdlWindow> SdlWindow::open(View *view, String title, WindowMode mode)
{
    return (new SdlWindow(view, title))->open(mode);
}

SdlWindow::SdlWindow(View *view, String title):
    Window(view, title)
{}

SdlWindow::~SdlWindow()
{
    SDL_DestroyRenderer(sdlRenderer_);
    SDL_DestroyWindow(sdlWindow_);
}

SdlWindow *SdlWindow::open(WindowMode mode)
{
    auto sdlWindowFlags = [](WindowMode mode) -> Uint32 {
        Uint32 flags = 0;
        if (+(mode & WindowMode::Fullscreen))        flags |= SDL_WINDOW_FULLSCREEN;
        if (+(mode & WindowMode::FullscreenDesktop)) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        if (+(mode & WindowMode::OpenGl))            flags |= SDL_WINDOW_OPENGL;
        if (+(mode & WindowMode::Shown))             flags |= SDL_WINDOW_SHOWN;
        if (+(mode & WindowMode::Hidden))            flags |= SDL_WINDOW_HIDDEN;
        if (+(mode & WindowMode::Borderless))        flags |= SDL_WINDOW_BORDERLESS;
        if (+(mode & WindowMode::Resizable))         flags |= SDL_WINDOW_RESIZABLE;
        if (+(mode & WindowMode::Minimized))         flags |= SDL_WINDOW_MINIMIZED;
        if (+(mode & WindowMode::Maximized))         flags |= SDL_WINDOW_MAXIMIZED;
        if (+(mode & WindowMode::InputGrabbed))      flags |= SDL_WINDOW_INPUT_GRABBED;
        if (+(mode & WindowMode::InputFocus))        flags |= SDL_WINDOW_INPUT_FOCUS;
        if (+(mode & WindowMode::MouseFocus))        flags |= SDL_WINDOW_MOUSE_FOCUS;
        if (+(mode & WindowMode::Foreign))           flags |= SDL_WINDOW_FOREIGN;
        #ifdef SDL_WINDOW_ALLOW_HIGHDPI
        if (+(mode & WindowMode::AllowHighDpi))      flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        #endif
        #ifdef SDL_WINDOW_MOUSE_CAPTURE
        if (+(mode & WindowMode::MouseCapture))      flags |= SDL_WINDOW_MOUSE_CAPTURE;
        #endif
        #ifdef SDL_WINDOW_ALWAYS_ON_TOP
        if (+(mode & WindowMode::AlwaysOnTop))       flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        #endif
        #ifdef SDL_WINDOW_SKIP_TASKBAR
        if (+(mode & WindowMode::SkipTaskBar))       flags |= SDL_WINDOW_SKIP_TASKBAR;
        #endif
        #ifdef SDL_WINDOW_UTILITY
        if (+(mode & WindowMode::Utility))           flags |= SDL_WINDOW_UTILITY;
        #endif
        #ifdef SDL_WINDOW_TOOLTIP
        if (+(mode & WindowMode::Tooltip))           flags |= SDL_WINDOW_TOOLTIP;
        #endif
        #ifdef SDL_WINDOW_POPUP_MENU
        if (+(mode & WindowMode::PopupMenu))         flags |= SDL_WINDOW_POPUP_MENU;
        #endif
        return flags;
    };

    const int screen = 0; // FIXME
    sdlWindow_ = SDL_CreateWindow(title(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen), SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen), size()[0], size()[1], sdlWindowFlags(mode));
    if (!sdlWindow_) CC_DEBUG_ERROR(SDL_GetError());
    id_ = SDL_GetWindowID(sdlWindow_);

    currentSize_ = size();
    {
        int x = 0, y = 0;
        SDL_GetWindowPosition(sdlWindow_, &x, &y);
        pos = Point{double(x), double(y)};
    }


    if (
        +(mode & WindowMode::Accelerated) ||
        +(mode & WindowMode::Fullscreen) ||
        +(mode & WindowMode::FullscreenDesktop)
    )
        sdlRenderer_ = SDL_CreateRenderer(sdlWindow_, -1, SDL_RENDERER_ACCELERATED | (+(mode & WindowMode::VSync) * SDL_RENDERER_PRESENTVSYNC));
    else
        sdlRenderer_ = SDL_CreateRenderer(sdlWindow_, -1, SDL_RENDERER_SOFTWARE);

    if (!sdlRenderer_) CC_DEBUG_ERROR(SDL_GetError());

    #ifndef NDEBUG
    {
        SDL_RendererInfo info_, *info = &info_;
        if (SDL_GetRendererInfo(sdlRenderer_, info) != 0) CC_DEBUG_ERROR(SDL_GetError());
        fout() << "SDL renderer: " << info->name << " (" << ((info->flags & SDL_RENDERER_ACCELERATED) ? "ACCELERATED" : "SOFTWARE") << ")" << nl;
    }
    #endif

    title->connect([=]{
        SDL_SetWindowTitle(sdlWindow_, title());
    });

    size->connect([=]{
        if (size() != currentSize_)
            SDL_SetWindowSize(sdlWindow_, size()[0], size()[1]);
    });

    pos->connect([=]{
        if (pos() != currentPos_)
            SDL_SetWindowPosition(sdlWindow_, pos()[0], pos()[1]);
    });

    return this;
}

void SdlWindow::onWindowResized(Size newSize)
{
    currentSize_ = newSize;
    size = newSize;
}

void SdlWindow::onWindowMoved(Point newPos)
{
    currentPos_ = newPos;
    pos = newPos;
}

void SdlWindow::onWindowShown()
{
    visible = true;
}

void SdlWindow::onWindowHidden()
{
    visible = false;
}

void SdlWindow::onWindowExposed()
{
    view()->exposed();
}

void SdlWindow::renderFrame(Frame *frame)
{
    while (frame->count() > 0) {
        Ref<const UpdateRequest> request = frame->popFront();
        if (
            request->reason() == UpdateReason::Changed ||
            request->reason() == UpdateReason::Resized
        )
            updateTexture(sdlRenderer_, request->view());
    }

    renderCascade(sdlRenderer_, view_);

    SDL_RenderPresent(sdlRenderer_);
}

void SdlWindow::renderCascade(SDL_Renderer *sdlRenderer, View *view)
{
    if (!view->visible()) return;

    renderTexture(sdlRenderer, view);

    for (int i = 0, n = view->childCount(); i < n; ++i)
        renderCascade(sdlRenderer, view->childAt(i));
}

void SdlWindow::updateTexture(SDL_Renderer *sdlRenderer, View *view)
{
    SdlContext *context = sdlContext(view);
    bool hasPixels = isPainted(view) && view->size() != Size{};

    if ((context->sdlTextureSize_ != view->size() || !hasPixels) && context->sdlTexture_) {
        SDL_DestroyTexture(context->sdlTexture_);
        context->sdlTexture_ = 0;
    }

    if (!hasPixels) return;

    if (!context->sdlTexture_) {
        context->sdlTexture_ = SDL_CreateTexture(
            sdlRenderer,
            #ifdef SDL_PIXELFORMAT_ARGB32
            SDL_PIXELFORMAT_ARGB32,
            #else
            SDL_PIXELFORMAT_ARGB8888,
            #endif
            isStatic(view) ? SDL_TEXTUREACCESS_STATIC : SDL_TEXTUREACCESS_STREAMING,
            view->size()[0],
            view->size()[1]
        );
        context->sdlTextureSize_ = view->size();
        SDL_SetTextureBlendMode(context->sdlTexture_, /*isOpaque(view) ? SDL_BLENDMODE_NONE :*/ SDL_BLENDMODE_BLEND); // TODO: SDL_BLENDMODE_NONE is buggy (see exampleLabel)
        if (!context->sdlTexture_) CC_DEBUG_ERROR(SDL_GetError());
    }

    Image *image = Window::image(view);
    if (!isOpaque(view)) image->normalize();

    if (isStatic(view)) {
        if (SDL_UpdateTexture(context->sdlTexture_, 0, image->data()->bytes(), image->pitch()) != 0)
            CC_DEBUG_ERROR(SDL_GetError());
    }
    else {
        void *dstData = 0;
        int pitch = 0;
        if (SDL_LockTexture(context->sdlTexture_, 0, &dstData, &pitch) != 0) CC_DEBUG_ERROR(SDL_GetError());
        if (pitch != image->pitch()) CC_DEBUG_ERROR(Format("Failed to upload texture: pitch mismatch (%% != %%)") << image->pitch() << pitch);
        ::memcpy(dstData, image->data()->bytes(), image->data()->count());
        SDL_UnlockTexture(context->sdlTexture_);
    }
}

void SdlWindow::renderTexture(SDL_Renderer *sdlRenderer, View *view)
{
    SDL_Texture *sdlTexture = sdlContext(view)->sdlTexture_;
    if (!sdlTexture) return;

    SDL_Rect destRect;
    destRect.w = view->size()[0];
    destRect.h = view->size()[1];
    if (view->parent()) {
        Point p = view->mapToGlobal(Point{0, 0});
        destRect.x = p[0];
        destRect.y = p[1];
    }
    else {
        destRect.x = 0;
        destRect.y = 0;
    }

    if (view->angle() == 0) {
        SDL_RenderCopy(sdlRenderer, sdlTexture, 0, &destRect);
    }
    else {
        Point center = view->center();
        SDL_Point sdlCenter{
            int(center[0]),
            int(center[1])
        };
        SDL_RenderCopyEx(sdlRenderer, sdlTexture, 0, &destRect, view->angle(), &sdlCenter, SDL_FLIP_NONE);
    }
}

SdlContext *SdlWindow::sdlContext(View *view)
{
    SdlContext *context = Object::cast<SdlContext *>(getContext(view));
    if (!context) {
        Ref<SdlContext> newContext = SdlContext::create();
        setContext(view, newContext);
        context = newContext;
    }
    return context;
}

}} // namespace cc::ui
