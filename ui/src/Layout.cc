/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Layout>

namespace cc {
namespace ui {

Layout::Layout(View *view):
    view_(view)
{
    view->layout_ = this;
}

Layout::~Layout()
{
    if (view_->layout_ == this)
        view_->layout_ = 0;
}

void Layout::updateView()
{
    view_->update();
}

}} // namespace cc::ui
