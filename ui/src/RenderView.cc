/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/RenderView>

namespace cc {

struct RenderView::State final: public View::State
{
    bool isPainted() const override { return width() * height() > 0; }
    bool isStatic() const override { return false; }
    void update() { View::State::update(UpdateReason::Changed); }
};

RenderView::RenderView():
    View{onDemand<State>}
{}

RenderView::RenderView(Out<RenderView> self):
    View{new State}
{
    self = *this;
}

Image &RenderView::image()
{
    return me().image();
}

void RenderView::update()
{
    me().update();
}

RenderView::State &RenderView::me()
{
    return View::me().as<State>();
}

} // namespace cc
