/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Item>
#include <cc/ui/ItemLayout>
#include <cc/ui/ItemVisual>

namespace cc {
namespace ui {

ItemVisual::ItemVisual(Item *item, const ItemLayout *layout):
    item_(item),
    layout_(layout)
{
    item->changed->connect([this]{ update(); });
}

}} // namespace cc::ui
