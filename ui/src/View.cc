/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Window>
#include <cc/ui/Frame>
#include <cc/ui/View>

namespace cc {
namespace ui {

Ref<Painter> View::createPainter()
{
    return Painter::create(this);
}

View::View():
    parent_(0)
{}

View::View(int x, int y, int w, int h, Color color, Window *window, View *parent):
    Rect(x, y, w, h),
    viewMoved_(ViewEvent::create()),
    viewResized_(ViewEvent::create()),
    window_(window),
    parent_(parent),
    color_(color)
{}

View::~View()
{}

void View::mapLocalToGlobal(int lx, int ly, int *gx, int *gy)
{
    if (!parent_) {
        *gx = lx;
        *gy = ly;
    }
    else {
        parent_->mapLocalToGlobal(x_ + lx, y_ + ly, gx, gy);
    }
}

Rect *View::fullRegion()
{
    if (fullRegion_) {
        if (fullRegion_->w() != w() || fullRegion_->h() != h())
            fullRegion_->resize(w(), h());
    }
    else fullRegion_ = Rect::copy(this);
    return fullRegion_;
}

}} // namespace cc::ui
