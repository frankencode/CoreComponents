/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Item>
#include <cc/ui/VisualLayout>
#include <cc/ui/Visual>

namespace cc {
namespace ui {

Visual::Visual(Item *item, const VisualLayout *layout):
    item_(item),
    layout_(layout)
{
    item->changed->connect([this]{ update(); });
}

}} // namespace cc::ui
