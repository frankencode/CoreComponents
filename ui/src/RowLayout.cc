/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/RowLayout>
#include <cc/ui/View>

namespace cc {
namespace ui {

RowLayout::Instance::Instance(const View &view):
    view_{view}
{
    init(view_);

    align >>[=]{ updateLayout(); };
    margin >>[=]{ updateLayout(); };
    spacing >>[=]{ updateLayout(); };

    updateLayout();
}

void RowLayout::Instance::childReady(View child)
{
    if (child->visible()) {
        if (align() == RowAlign::Top) {
            Size innerSize;
            if (view_->size()[0] > 0)
                innerSize = view_->size() - 2 * margin();

            const double x = innerSize[0];

            if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
            innerSize[0] += child->padding() + child->size()[0];
            if (view_->childCount() > 0) innerSize[0] += spacing();

            view_->size = innerSize + 2 * margin();

            updateChildPos(child, innerSize, x);
            update(view_);
        }
        else
            updateLayout();
    }

    child->size >>[=]{
        if (view_->childCount() > 0) {
            if (child == view_->childAt(view_->childCount() - 1)) {
                double newWidth = child->pos()[0] + child->size()[0] + margin()[0];
                double newHeight = child->size()[1] + 2 * margin()[1];
                if (newHeight < view_->size()[1]) newHeight = view_->size()[1];
                view_->size = Size{ newWidth, newHeight };
                updateChildPos(child, view_->size() - 2 * margin(), child->pos()[0]);
                update(view_);
                return;
            }
        }
        updateLayout();
    };

    child->visible >>[=]{ updateLayout(); };
    child->padding >>[=]{ updateLayout(); };
}

void RowLayout::Instance::childDone(View child)
{
    updateLayout();
}

void RowLayout::Instance::updateChildPos(View child, Size innerSize, double x)
{
    if (align() == RowAlign::Top)
        child->pos = Point{ x, 0 } + margin();
    else if (align() == RowAlign::Bottom)
        child->pos = Point{ x, innerSize[1] - child->size()[1] } + margin();
    else
        child->pos = Point{ x, 0.5 * (innerSize[1] - child->size()[1]) } + margin();
}

void RowLayout::Instance::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = view_->childCount(); i < n; ++i)
    {
        View child = view_->childAt(i);
        if (!child->visible()) continue;
        if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
        innerSize[0] += child->padding() + child->size()[0];
        if (i < n - 1) innerSize[0] += spacing();
    }

    view_->size = innerSize + 2 * margin();

    double x = 0;

    for (int i = 0, n = view_->childCount(); i < n; ++i)
    {
        View child = view_->childAt(i);
        if (!child->visible()) continue;

        x += child->padding();

        updateChildPos(child, innerSize, x);

        x += child->size()[0] + spacing();
    }

    update(view_);
}

}} // namespace cc::ui
