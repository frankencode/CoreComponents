/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ItemView>
#include <cc/ui/ItemCarrier>
#include <cc/ui/Label>

namespace cc {
namespace ui {

ItemView::Instance::Instance(Item *rootItem):
    rootItem_{rootItem}
{
    wheelGranularity = 0;
}

Ref<Item> ItemView::Instance::initItemModel()
{
    return Item::create();
}

void ItemView::Instance::resetItemModel()
{
    resetCarrier();
}

int ItemView::Instance::currentRow() const
{
    return currentRow_();
}

int ItemView::Instance::Instance::rowCount() const
{
    return itemCarrier()->layout_->count();
}

Item *ItemView::Instance::currentItem() const
{
    return itemCarrier()->layout_->at(currentRow_())->item_;
}

Item *ItemView::Instance::itemAt(int row) const
{
    return itemCarrier()->layout_->at(row)->item_;
}

View ItemView::Instance::createDelegate(Item *item)
{
    int depth = 0;
    for (const Item *p = item->parent(); p != rootItem_; p = p->parent())
        ++depth;

    Label label{Format{"Item %%"} << item->getIndex() + 1};
    label->pos = Point{depth * dp(16), 0};
    label->margin = dp(12);
    return label;
}

View ItemView::Instance::delegateAt(int row) const
{
    return itemCarrier()->layout_->at(row)->delegate_;
}

View ItemView::Instance::createCarrier()
{
    if (!rootItem_) rootItem_ = initItemModel();
    return ItemCarrier{rootItem_};
}

void ItemView::Instance::preheat()
{
    if (cacheRatio() > 0)
        itemCarrier()->updateView(true);
}

bool ItemView::Instance::onPointerClicked(const PointerEvent *event)
{
    if (carrierAtRest() && 0 <= currentRow_()) clicked();
    currentDelegate_ = nullptr;
    currentRow_ = -1;
    return true;
}

bool ItemView::Instance::onPointerPressed(const PointerEvent *event)
{
    View highlight = itemCarrier()->highlight_;
    Ref<ItemCarrier::LayoutItem> layoutItem;
    int row = 0;
    if (itemCarrier()->layout_->lookup(event->pos()[1] - carrier()->pos()[1], &layoutItem, &row)) {
        highlight->pos = Point{ 0, layoutItem->delegate_->pos()[1] };
        highlight->size = Size{ size()[0], layoutItem->delegate_->size()[1] };
        highlight->visible = true;
        currentDelegate_ = layoutItem->delegate_;
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
    return ScrollView::Instance::onPointerPressed(event);
}

bool ItemView::Instance::onPointerReleased(const PointerEvent *event)
{
    ScrollView::Instance::onPointerReleased(event);

    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }

    return true;
}

bool ItemView::Instance::onWheelMoved(const WheelEvent *event)
{
    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }
    currentDelegate_ = nullptr;
    currentRow_ = -1;

    return ScrollView::Instance::onWheelMoved(event);
}

bool ItemView::Instance::onWindowLeft()
{
    itemCarrier()->highlight_->visible = false;
    if (currentDelegate_() && hasSavedDelegatePaper_) {
        currentDelegate_()->paper = delegatePaperSaved_;
        hasSavedDelegatePaper_ = false;
    }
    currentDelegate_ = nullptr;
    currentRow_ = -1;

    return ScrollView::Instance::onWindowLeft();
}

ItemCarrier ItemView::Instance::itemCarrier() const
{
    return carrier()->as<ItemCarrier>();
}

}} // namespace cc::ui
