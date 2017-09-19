/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/List>
#include <cc/Event>
#include <cc/ui/Rect>
#include <cc/ui/Painter>

namespace cc {
namespace ui {

class Window;
class Frame;

class View: public Rect
{
public:
    Ref<Painter> createPainter();

    typedef Event<View *> ViewEvent;
    inline ViewEvent *viewMoved() const { return viewMoved_; }
    inline ViewEvent *viewResized() const { return viewResized_; }

    inline Window *window() const { return window_; }
    inline View *parent() const { return parent_; }

    inline int childCount() const { return children_ ? children_->count() : 0; }
    inline View *childAt(int i) const { return children_->at(i); }

    inline Color color() const { return color_; }

    void mapLocalToGlobal(int lx, int ly, int *gx, int *gy);

protected:
    View();
    View(int x, int y, int w, int h, Color color, Window *window, View *parent);
    ~View();

    Rect *fullRegion();

    typedef List< Ref<View> > Children;

    Children *children() {
        if (!children_) children_ = Children::create();
        return children_;
    }

    Ref<ViewEvent> viewMoved_;
    Ref<ViewEvent> viewResized_;
    Ref<Window> window_;
    View *parent_;
    Ref<Children> children_;
    Ref<Rect> fullRegion_;
    Color color_;
};

}} // namespace cc::ui

