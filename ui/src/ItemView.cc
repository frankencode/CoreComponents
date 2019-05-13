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

Ref<Item> ItemView::initItemModel()
{
    return Item::create();
}

void ItemView::resetItemModel()
{
    resetCarrier();
}

int ItemView::currentRow() const
{
    return currentRow_();
}

int ItemView::rowCount() const
{
    return itemCarrier()->layout_->count();
}

Item *ItemView::currentItem() const
{
    return itemCarrier()->layout_->at(currentRow_())->item();
}

Item *ItemView::itemAt(int row) const
{
    return itemCarrier()->layout_->at(row)->item();
}

View *ItemView::addDelegate(View *parent, Item *item)
{
    int depth = 0;
    for (const Item *p = item->parent(); p != rootItem_; p = p->parent())
        ++depth;

    auto label = parent->add<Label>(
        Format{"Item %%"} << item->getIndex() + 1
    );

    label->pos = Point{ depth * dp(16), 0 };
    label->margin = dp(12);

    return label;
}

View *ItemView::delegateAt(int row) const
{
    return itemCarrier()->layout_->at(row)->delegate();
}

View *ItemView::addCarrier()
{
    if (!rootItem_) rootItem_ = initItemModel();
    return add<ItemCarrier>(rootItem_);
}

void ItemView::preheat()
{
    if (cacheRatio() > 0)
        itemCarrier()->updateView(true);
}

bool ItemView::onPointerClicked(const PointerEvent *event)
{
    if (carrierAtRest() && 0 <= currentRow_()) clicked();
    currentDelegate_ = nullptr;
    currentRow_ = -1;
    return true;
}

bool ItemView::onPointerPressed(const PointerEvent *event)
{
    View *highlight = itemCarrier()->highlight_;
    Ref<ItemCarrier::LayoutItem> layoutItem;
    int row = 0;
    if (itemCarrier()->layout_->lookup(event->pos()[1] - carrier()->pos()[1], &layoutItem, &row)) {
        highlight->pos = Point{ 0, layoutItem->delegate()->pos()[1] };
        highlight->size = Size{ size()[0], layoutItem->delegate()->size()[1] };
        highlight->visible = true;
        currentDelegate_ = layoutItem->delegate();
        if (currentDelegate_()->paper() != highlight->paper()) {
            delegatePaperSaved_ = currentDelegate_()->paper();
            hasSavedDelegatePaper_ = true;
            currentDelegate_()->paper = highlight->paper();
        }
        currentRow_ = row;
    }
    else {
        highlight->visible = false;
        currentDelegate_ = nullptr;
        currentRow_ = -1;
    }
    return ScrollView::onPointerPressed(event);
}

bool ItemView::onPointerReleased(const PointerEvent *event)
{
    ScrollView::onPointerReleased(event);

    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }

    return true;
}

bool ItemView::onWheelMoved(const WheelEvent *event)
{
    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }
    currentDelegate_ = nullptr;
    currentRow_ = -1;

    return ScrollView::onWheelMoved(event);
}

bool ItemView::onWindowLeft()
{
    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }
    currentDelegate_ = nullptr;
    currentRow_ = -1;

    return ScrollView::onWindowLeft();
}

ItemCarrier *ItemView::itemCarrier() const
{
    return static_cast<ItemCarrier *>(carrier());
}

}} // namespace cc::ui
