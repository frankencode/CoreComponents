/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <SDL2/SDL.h>
#include <cc/types>
#include <cc/String>
#include <cc/ui/Window>

namespace cc {
namespace ui {

class SdlApplication;
class SdlView;
class SdlRenderer;

class SdlWindow: public Window
{
public:
    static Ref<SdlWindow> open(String title, int width, int height, Color color = Color());

    inline Uint32 id() const { return id_; }

    virtual void moveTo(int x, int y);
    virtual void resize(int w, int h);

    virtual void show();
    virtual void hide();

    virtual View *rootView() const;

protected:
    friend class SdlView;
    friend class SdlApplication;
    friend class SdlRenderer;

    SdlWindow(String title, int width, int height);
    ~SdlWindow();

    void establishRootView(Color color);

    void windowMovedEvent(int newX, int newY);
    void windowSizeChangedEvent(int newWidth, int newHeight);

    SDL_Window *window_;
    SDL_Renderer *renderer_;
    Uint32 id_;
    Ref<SdlView> rootView_;
};

}} // namespace cc::ui

