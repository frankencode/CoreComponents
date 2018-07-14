/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pile>
#include <cc/ui/ItemView>
#include <cc/ui/ItemCarrier>

namespace cc {
namespace ui {

ItemCarrier::ItemCarrier(View *parent, Item *rootItem):
    View{parent},
    rootItem_{rootItem},
    layout_(Layout::create())
{
    generateLayout(rootItem_, 0, rootItem_->count());
    layoutExtent = layout_->extent();
    cacheMargin->bind([=]{ return parent->size()[1] * static_cast<ItemView *>(parent)->cacheRatio(); });

    rootItem_->changed->connect([=]{
        const ItemDelta *delta = rootItem_->delta();
        const IndexPath *indexPath = delta->indexPath();

        Item *item = rootItem_;
        for (int k = 0; k < indexPath->count() - 1; ++k)
            item = item->at(indexPath->at(k));

        int itemIndex = indexPath->at(indexPath->count() - 1);
        int layoutIndex = item->getLayoutIndex(itemIndex);

        for (int j = 0; j < delta->removedCount(); ++j) {
            LayoutItem *layoutItem = layout_->at(layoutIndex);
            removeChild(layoutItem->delegate());
            layout_->removeAt(layoutIndex);
        }

        generateLayout(item, itemIndex, itemIndex + delta->insertedCount(), layoutIndex);
        layoutExtent = layout_->extent();

        updateView(true);
    });

    size->bind([=]{ return Size{ parent->size()[0], layoutExtent() }; });

    updateView(true);
    pos->connect([=]{ updateView(false); });
    parent->size->connect([=]{ updateView(true); });
}

int ItemCarrier::generateLayout(Item *item, int itemIndex0, int itemIndex1, int layoutIndex)
{
    for (int i = itemIndex0; i < itemIndex1; ++i)
    {
        Item *child = item->at(i);
        ItemView *itemView = static_cast<ItemView *>(parent());
        View *view = itemView->addDelegate(this, child);
        if (itemView->wheelGranularity() == 0)
            itemView->wheelGranularity = 2 * view->size()[1];
        Ref<LayoutItem> layoutItem = Object::create<LayoutItem>(child, view);
        double extent = 0.;
        if (view) {
            extent = view->size()[1];
            view->size->connect([=]{
                if (layout_->updateExtentAt(layoutItem->getIndex(), view->size()[1]))
                    layoutExtent = layout_->extent();
            });
        }
        layout_->insertAt(layoutIndex, layoutItem, extent);
        view->pos = Point{ view->pos()[0], layout_->getPosAt(layoutIndex) };
        ++layoutIndex;
        layoutIndex = generateLayout(child, 0, child->count(), layoutIndex);
    }

    return layoutIndex;
}

void ItemCarrier::updateView(bool preheat)
{
    // determine the visible range (r0, r1)
    //
    const double r0 = [=]{
        double h = -pos()[1];
        if (h < 0) h = 0;
        return h;
    }();

    const double r1 = [=]{
        double h = r0 + parent()->size()[1];
        if (h > size()[1]) h = size()[1];
        return h;
    }();

    // determine extended visible range (s0, s1)
    //
    const double m = cacheMargin();
    const double s0 = r0 - m;
    const double s1 = r1 + m;

    // iterate through all visible children and make them invisible if out of view
    //
    for (int i = 0; i < visibleChildCount();) {
        View *delegate = visibleChildAt(i);
        double d0 = delegate->pos()[1];
        double d1 = delegate->pos()[1] + delegate->size()[1];
        if (
            d1 - d0 > 0 && (
                (s0 <= d0 && d0 < s1) ||
                (s0 < d1 && d1 <= s1) ||
                (d0 < s0 && s1 < d1)
            )
        )
            ++i;
        else
            delegate->visible = false;
    }

    // iterate over the visible range (on the layout) and make all invisible delegates in range visible
    //
    {
        int i0 = 0, i1 = 0;
        double y = 0;

        if (preheat)
            layout_->getView(s0, s1, &i0, &i1, &y);
        else
            layout_->getView(r0, r1, &i0, &i1, &y);

        for (int i = i0; i < i1; ++i) {
            View *delegate = layout_->at(i)->delegate();
            delegate->pos = Point{ delegate->pos()[0], y };
            delegate->visible = true;
            y += delegate->size()[1];
        }
    }
}

}} // namespace cc::ui
