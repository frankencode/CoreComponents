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
    });

    size->bind([=]{ return parent->size()[0], layoutExtent(); });
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

}} // namespace cc::ui
