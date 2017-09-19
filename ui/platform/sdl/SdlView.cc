/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/Guard>
#include <cc/Mutex>
#include <cc/ui/Image>
#include <cc/ui/sdl/SdlWindow>
#include <cc/ui/sdl/SdlView>

namespace cc {
namespace ui {

Ref<SdlView> SdlView::create(int x, int y, int w, int h, Color color, Window *window, View *parent)
{
    return new SdlView(x, y, w, h, color, window, parent);
}

SdlView::SdlView(int x, int y, int w, int h, Color color, Window *window, View *parent):
    View(x, y, w, h, color, window, parent),
    mutex_(Mutex::create()),
    texture_(0)
{
    establish();
}

SdlView::~SdlView()
{
    SDL_DestroyTexture(texture_);
}

void SdlView::acquire()
{
    mutex_->acquire();
}

void SdlView::release()
{
    mutex_->release();
}

/*void SdlView::moveTo(int x, int y)
{
    Guard<SdlView> guard(this);
    if (x_ != x || y_ != y) {
        x_ = x;
        y_ = y;
        cast<SdlApplication>(Application::instance())->pushUpdateEvent(cast<SdlWindow>(window()));
        viewMoved()->notify(this);
    }
}*/ // FIXME

void SdlView::resize(int w, int h)
{
    if (w_ != w || h_ != h) {
        {
            Guard<SdlView> guard(this);
            w_ = w;
            h_ = h;
            establish();
        }
        viewResized()->notify(this);
    }
}

void SdlView::establish()
{
    image_ = Image::create(w_, h_);
    if (texture_) SDL_DestroyTexture(texture_);
    texture_ = SDL_CreateTexture(Object::cast<SdlWindow *>(window())->renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w_, h_);
    SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

    if (!texture_) CC_DEBUG_ERROR(SDL_GetError());
}

}} // namespace cc::ui
