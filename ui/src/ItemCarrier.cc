/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ItemCarrier>
#include <cc/ui/ItemView>
#include <cc/ui/Pile>

namespace cc {
namespace ui {

ItemCarrier::Instance::Instance(Item *rootItem):
    rootItem_{rootItem},
    layout_{ItemLayout::create()}
{
    build >>[=]{
        highlight_ = View{};
        highlight_->visible = false;
        highlight_->paper <<[=]{ return theme()->itemHighlightColor(); };
        (*this) << highlight_;

        generateLayout(rootItem_, 0, rootItem_->count());
        layoutExtent = layout_->extent();
        cacheMargin <<[=]{ return parent()->size()[1] * parent()->as<ItemView>()->cacheRatio(); };

        rootItem_->changed >>[=]{
            const ItemDelta *delta = rootItem_->delta();
            IndexPath indexPath = delta->indexPath();

            Item *item = rootItem_;
            for (int k = 0; k < indexPath->count() - 1; ++k)
                item = item->at(indexPath->at(k));

            int itemIndex = indexPath->at(indexPath->count() - 1);
            int layoutIndex = item->getLayoutIndex(itemIndex);

            for (int j = 0; j < delta->removedCount(); ++j) {
                LayoutItem *layoutItem = layout_->at(layoutIndex);
                removeChild(layoutItem->delegate_);
                layout_->removeAt(layoutIndex);
            }

            generateLayout(item, itemIndex, itemIndex + delta->insertedCount(), layoutIndex);
            layoutExtent = layout_->extent();

            updateView(true);
        };

        size <<[=]{ return Size { parent()->size()[0], layoutExtent() }; };

        updateView(true);
        pos >>[=]{ updateView(false); };
        size >>[=]{ updateView(true); };
    };
}

int ItemCarrier::Instance::generateLayout(Item *item, int itemIndex0, int itemIndex1, int layoutIndex)
{
    for (int i = itemIndex0; i < itemIndex1; ++i)
    {
        Item *child = item->at(i);
        ItemView itemView = parent()->as<ItemView>();
        View delegate = itemView->createDelegate(child);
        if (itemView->wheelGranularity() == 0)
            itemView->wheelGranularity = 3 * delegate->size()[1];
        Ref<LayoutItem> layoutItem = Object::create<LayoutItem>(child, delegate);
        double extent = 0.;
        if (delegate) {
            extent = delegate->size()[1];
            delegate->size >>[=]{
                if (layout_->updateExtentAt(layoutItem->getIndex(), delegate->size()[1]))
                    layoutExtent = layout_->extent();
            };
        }
        layout_->insertAt(layoutIndex, layoutItem, extent);
        delegate->pos = Point{ delegate->pos()[0], layout_->getPosAt(layoutIndex) };
        (*this) << delegate;
        ++layoutIndex;
        layoutIndex = generateLayout(child, 0, child->count(), layoutIndex);
    }

    return layoutIndex;
}

void ItemCarrier::Instance::updateView(bool preheat)
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
        View delegate = visibleChildAt(i);
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

        ItemView itemView = parent()->as<ItemView>();

        for (int i = i0; i < i1; ++i) {
            View delegate = layout_->at(i)->delegate_;
            delegate->pos = Point{ delegate->pos()[0], y };
            delegate->visible = true;
            itemView->updateDelegateInView(delegate, i);
            y += delegate->size()[1];
        }
    }
}

}} // namespace cc::ui
