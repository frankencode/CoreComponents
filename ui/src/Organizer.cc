/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Organizer>

namespace cc {
namespace ui {

Organizer::Organizer(View *view):
    view_(view)
{
    if (view->organizer_) delete view->organizer_;
    view->organizer_ = this;
}

void Organizer::updateView()
{
    view_->update();
}

}} // namespace cc::ui
