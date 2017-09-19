/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Channel>
#include <cc/Event>
#include <cc/ui/View>

namespace cc {
namespace ui {

class Window: public Rect
{
public:
    static Ref<Window> open(String title, int width, int height, Color color = Color());

    typedef Event<Window *> WindowEvent;
    inline WindowEvent *windowMoved() const { return windowMoved_; }
    inline WindowEvent *windowResized() const { return windowResized_; }

    inline String title() const { return title_; }

    virtual void show() = 0;
    virtual void hide() = 0;

    virtual View *rootView() const = 0;

protected:
    Window(String title);
    ~Window();

private:
    Ref<WindowEvent> windowMoved_;
    Ref<WindowEvent> windowResized_;
    String title_;
};

}} // namespace cc::ui

