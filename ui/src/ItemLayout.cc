/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pile>
#include <cc/ui/Item>
#include <cc/ui/ItemLayout>

namespace cc {
namespace ui {

Ref<ItemLayout> ItemLayout::create(Item *root, double maxWidth)
{
    return new ItemLayout(root, maxWidth);
}

ItemLayout::ItemLayout(Item *root, double maxWidth):
    root_(root),
    maxWidth_(maxWidth),
    visuals_(Visuals::create())
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

        for (int j = 0; j < delta->removedCount(); ++j)
            visuals_->removeAt(layoutIndex);

        generateLayout(item, itemIndex, itemIndex + delta->insertedCount(), layoutIndex);

        changed->emit();
    });
}

int ItemLayout::generateLayout(Item *item, int itemIndex0, int itemIndex1, int layoutIndex)
{
    for (int i = itemIndex0; i < itemIndex1; ++i)
    {
        Item *child = item->at(i);
        Ref<ItemVisual> visual = child->createVisual(this);
        double extent = 0.;
        if (visual) {
            if (visual->size() == Size{})
                visual->update();
            extent = visual->size()[1];
            visual->size->connect([=]{
                if (visuals_->updateExtentAt(visual->getIndex(), visual->size()[1]))
                    changed->emit();
            });
        }
        visuals_->insertAt(layoutIndex, visual, extent);
        ++layoutIndex;
        layoutIndex = generateLayout(child, 0, child->count(), layoutIndex);
    }
    return layoutIndex;
}

}} // namespace cc::ui
