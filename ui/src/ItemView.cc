/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ItemCarrier>
#include <cc/ui/ItemView>

namespace cc {
namespace ui {

ItemView::ItemView(View *parent, Item *rootItem):
    ScrollView{parent},
    rootItem_{rootItem}
{}

View *ItemView::addCarrier()
{
    return add<ItemCarrier>(rootItem_);
}

}} // namespace cc::ui
