/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <SDL2/SDL.h>
#include <cc/ui/View>

namespace cc {
    class Mutex;
    template<class> class Guard;
}

namespace cc {
namespace ui {

class Image;
class SdlWindow;
class SdlRenderer;

class SdlView: public View
{
public:
    static Ref<SdlView> create(int x, int y, int w, int h, Color color, Window *window, View *parent = 0);

    // virtual void moveTo(int x, int y);
    virtual void resize(int w, int h);

private:
    friend class SdlWindow;
    friend class SdlApplication;
    friend class SdlRenderer;
    friend class Guard<SdlView>;

    SdlView(int x, int y, int w, int h, Color color, Window *window, View *parent);
    ~SdlView();

    void acquire();
    void release();

    void establish();

    Ref<Mutex> mutex_;
    Ref<Image> image_;
    SDL_Texture *texture_;
};

}} // namespace cc::ui

