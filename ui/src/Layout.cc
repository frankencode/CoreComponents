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

void Layout::Instance::init(View &view)
{
    view->layout_ = this;
}

void Layout::Instance::update(View &view)
{}

}} // namespace cc::ui
