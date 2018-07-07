/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pile>
#include <cc/ui/Label>
#include <cc/ui/ItemView>

namespace cc {
namespace ui {

ItemView::ItemView(View *parent, Item *root):
    View(parent),
    root_{root},
    layout_(Layout::create())
{
    generateLayout(root, 0, root->count());

    root->changed->connect([=]{
        const ItemDelta *delta = root->delta();
        const IndexPath *indexPath = delta->indexPath();

        Item *item = root;
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

        updateView();
    });

    size->bind([=]{ return parent->size()[0], layoutExtent(); });

    updateView();
    pos->connect([=]{ updateView(); });
    parent->size->connect([=]{ updateView(); });
}

View *ItemView::addDelegate(Item *item)
{
    int depth = 0;
    for (const Item *p = item->parent(); p; p = p->parent())
        ++depth;

    auto label = add<Label>(
        Format("%%Item %%")
            << String::create(2 * depth, ' ')
            << item->getIndex() + 1
    );

    return label;
}

int ItemView::generateLayout(Item *item, int itemIndex0, int itemIndex1, int layoutIndex)
{
    for (int i = itemIndex0; i < itemIndex1; ++i)
    {
        Item *child = item->at(i);
        View *view = addDelegate(item);
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
        view->pos = Point{ 0, layout_->getPosAt(layoutIndex) };
        ++layoutIndex;
        layoutIndex = generateLayout(child, 0, child->count(), layoutIndex);
    }

    return layoutIndex;
}

void ItemView::updateView()
{
    // determine the visible range (r0, r1)
    //
    double r0 = -pos()[1];
    if (r0 < 0) r0 = 0;

    double r1 = r0 + parent()->size()[1];
    if (r1 > size()[1]) r1 = size()[1];

    // iterate through all visible children and make them invisible if out of view
    //
    for (int i = 0; i < visibleChildCount();) {
        View *delegate = visibleChildAt(i);
        double d0 = delegate->pos()[1];
        double d1 = delegate->pos()[1] + delegate->size()[1];
        if (
            d1 - d0 > 0 && (
                (r0 <= d0 && d0 < r1) ||
                (r0 < d1 && d1 <= r1) ||
                (d0 < r0 && r1 < d1)
            )
        )
            ++i;
        else
            delegate->visible = false;
    }

    // iterate the visible range (on the layout) and make all invisible delegates in range visible
    //
    {
        int i0 = 0, i1 = 0;
        double y0 = 0;
        layout_->getView(r0, r1, &i0, &i1, &y0);
        for (int i = i0; i < i1; ++i) {
            View *delegate = layout_->at(i)->delegate();
            delegate->pos = Point{ 0, layout_->getPosAt(i) };
            delegate->visible = true;
        }
    }
}

}} // namespace cc::ui
