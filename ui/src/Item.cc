/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Item>

namespace cc {
namespace ui {

Item::Item():
    parent_{nullptr},
    children_{Children::create()}
{}

void Item::insertAt(int i, Item *item)
{
    #ifdef NDEBUG
    if (!(0 <= i && i <= children_->count())) return;
    if (item->parent_) return;
    #endif
    children_->insertAt(i, item, 1 + item->totalCount());
    CC_ASSERT(!item->parent_);
    item->parent_ = this;

    updateExtents(1 + item->totalCount());

    delta_->indexPath_ = IndexPath::create() << i;
    delta_->removedCount_  = 0;
    delta_->insertedCount_ = 1;
    emit();
}

void Item::removeAt(int i)
{
    #ifdef NDEBUG
    if (!children_->has(i)) return;
    #endif
    Ref<Item> item = children_->at(i);
    children_->removeAt(i);
    CC_ASSERT(item->parent_ == this);
    item->parent_ = 0;

    updateExtents(-1 - item->count());

    delta_->indexPath_ = IndexPath::create() << i;
    delta_->removedCount_  = 1;
    delta_->insertedCount_ = 0;
    emit();
}

void Item::paste(int i0, int i1, const ItemList *items)
{
    if (i0 < 0) i0 = 0;
    if (children_->count() < i1) i1 = children_->count();
    if (i1 < i0) return;

    int deltaCount = 0;

    for (int i = i0; i < i1; ++i) {
        Item *child = children_->at(i0);
        deltaCount += -1 - child->count();
        CC_ASSERT(child->parent_ == this);
        child->parent_ = 0;
        children_->removeAt(i0);
    }

    if (items) {
        for (int j = 0; j < items->count(); ++j) {
            Item *child = items->at(j);
            children_->insertAt(i0 + j, child, 1 + child->count());
            CC_ASSERT(!child->parent_);
            child->parent_ = this;
            deltaCount += 1 + child->count();
        }
    }

    updateExtents(deltaCount);

    delta_->indexPath_ = IndexPath::create() << i0;
    delta_->removedCount_ = i1 - i0;
    delta_->insertedCount_ = (items) ? items->count() : 0;
    emit();
}

int Item::getLayoutIndex(int i) const
{
    int layoutIndex = (children_->count() > 0) ? children_->getPosAt(i) : 0;
    for (const Item *parent = parent_, *child = this; parent; parent = parent->parent_) {
        layoutIndex += 1 + ((parent->children_->count() > 0) ? parent->children_->getPosAt(child->getIndex()) : 0);
        child = parent;
    }
    return layoutIndex;
}

void Item::updateExtents(int deltaCount)
{
    if (deltaCount == 0) return;

    for (Item *parent = parent_, *child = this; parent; parent = parent->parent_) {
        parent->children_->updateExtentAt(child->getIndex(), [=](int extent){ return extent + deltaCount; });
        child = parent;
    }
}

void Item::emit()
{
    {
        Item *item = this;
        while (true) {
            Item *parent = item->parent_;
            if (!parent) break;
            parent->delta_->indexPath_     = item->delta_->indexPath_->copy();
            parent->delta_->indexPath_->pushFront(item->getIndex());
            parent->delta_->removedCount_  = item->delta_->removedCount_;
            parent->delta_->insertedCount_ = item->delta_->insertedCount_;
            item = parent;
        }
    }

    {
        Item *item = this;
        while (true) {
            item->changed->emit();
            if (!item->parent_) break;
            item = item->parent_;
        }
    }
}

}} // namespace cc::ui
