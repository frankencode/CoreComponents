/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ItemCarrier>
#include <cc/ui/Label>
#include <cc/ui/ItemView>

namespace cc {
namespace ui {

ItemView::ItemView(View *parent, Item *rootItem):
    ScrollView{parent},
    rootItem_{rootItem}
{
    wheelGranularity = 0;
}

View *ItemView::addCarrier()
{
    return add<ItemCarrier>(rootItem_);
}

void ItemView::preheat()
{
    if (cacheRatio() > 0)
        static_cast<ItemCarrier *>(carrier())->updateView(true);
}

View *ItemView::addDelegate(View *parent, Item *item)
{
    int depth = 0;
    for (const Item *p = item->parent(); p != rootItem_; p = p->parent())
        ++depth;

    auto label = parent->add<Label>(
        Format("Item %%") << item->getIndex() + 1
    );

    label->pos = Point{ depth * dp(16), 0 };
    label->margin = dp(12);

    return label;
}

}} // namespace cc::ui
