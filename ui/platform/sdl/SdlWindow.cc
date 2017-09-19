/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <SDL2/SDL_video.h>
#include <cc/exceptions>
#include <cc/stdio>
#include <cc/Guard>
#include <cc/ui/sdl/SdlView>
#include <cc/ui/sdl/SdlWindow>

namespace cc {
namespace ui {

Ref<SdlWindow> SdlWindow::open(String title, int width, int height, Color color)
{
    Ref<SdlWindow> window = new SdlWindow(title, width, height);
    window->establishRootView(color);
    return window;
}

SdlWindow::SdlWindow(String title, int width, int height):
    Window(title)
{
    w_ = width;
    h_ = height;
    int screen = 0;
    #if 0 // FIXME: window placed more intelligently starting with SDL v2.0.4?
    {
        int screenCount = SDL_GetNumVideoDisplays();
        if (screenCount > 1) {
            int mx = 0, my = 0;
            SDL_GetGlobalMouseState(&mx, &my);
            for (int i = 0; i < screenCount; ++i) {
                SDL_Rect r;
                if (SDL_GetDisplayBounds(i, &r) != 0) {
                    ferr() << SDL_GetError() << nl;
                    break;
                }
                if (
                    (r.x <= mx && mx < r.x + r.w) &&
                    (r.y <= my && my < r.y + r.w)
                ) {
                    screen = i;
                    break;
                }
            }
        }
    }
    #endif
    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen), SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen), width, height, SDL_WINDOW_RESIZABLE);
    if (!window_) CC_DEBUG_ERROR(SDL_GetError());
    id_ = SDL_GetWindowID(window_);
    SDL_GetWindowPosition(window_, &x_, &y_);

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED); // SDL_RENDERER_SOFTWARE, SDL_RENDERER_PRESENTVSYNC
    if (!renderer_) CC_DEBUG_ERROR(SDL_GetError());
    #ifndef NDEBUG
    {
        SDL_RendererInfo info_, *info = &info_;
        if (SDL_GetRendererInfo(renderer_, info) != 0) CC_DEBUG_ERROR(SDL_GetError());
        fout() << "SDL renderer: " << info->name << nl;
        if (info->flags & SDL_RENDERER_SOFTWARE) fout() << "  software renderer" << nl;
        if (info->flags & SDL_RENDERER_ACCELERATED) fout() << "  accelerated renderer" << nl;
    }
    #endif
}

SdlWindow::~SdlWindow()
{
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}

void SdlWindow::establishRootView(Color color)
{
    rootView_ = SdlView::create(0, 0, w_, h_, color, this, 0);
    rootView_->w_ = w_;
    rootView_->h_ = h_;
}

void SdlWindow::moveTo(int x, int y)
{
    SDL_SetWindowPosition(window_, x, y);
}

void SdlWindow::resize(int w, int h)
{
    SDL_SetWindowSize(window_, w, h);
}

void SdlWindow::windowMovedEvent(int newX, int newY)
{
    if (x_ != newX || y_ != newY) {
        x_ = newX;
        y_ = newY;
        windowMoved()->notify(this);
    }
}

void SdlWindow::windowSizeChangedEvent(int newWidth, int newHeight)
{
    if (w_ != newWidth || h_ != newHeight) {
        w_ = newWidth;
        h_ = newHeight;
        /*{
            Color c = rootView_->color();
            // SDL_SetRenderDrawColor(renderer_, Color::red(c), Color::green(c), Color::blue(c), 0xFF);
            SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderFillRect(renderer_, 0);
            SDL_RenderPresent(renderer_);
        }*/
        rootView_->resize(newWidth, newHeight);
        windowResized()->notify(this);
    }
}

void SdlWindow::show()
{
    SDL_ShowWindow(window_);
}

void SdlWindow::hide()
{
    SDL_HideWindow(window_);
}

View *SdlWindow::rootView() const
{
    return rootView_;
}

}} // namespace cc::ui
