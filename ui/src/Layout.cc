/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Layout>
#include <cc/ui/View>

namespace cc {
namespace ui {

Layout::Instance::Instance(View *view):
    view_{view}
{
    view->layout_ = this;
}

void Layout::Instance::updateView()
{
    view_->update();
}

}} // namespace cc::ui
