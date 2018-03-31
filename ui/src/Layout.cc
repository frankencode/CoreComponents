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
    if (view->layout_) delete view->layout_;
    view->layout_ = this;
}

void Layout::updateView()
{
    view_->update();
}

}} // namespace cc::ui
