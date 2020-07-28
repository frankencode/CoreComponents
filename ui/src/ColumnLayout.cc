/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ColumnLayout>
#include <cc/ui/View>

namespace cc {
namespace ui {

ColumnLayout::Instance::Instance(const View &view):
    view_{view}
{
    init(view_);

    align >>[=]{ updateLayout(); };
    margin >>[=]{ updateLayout(); };
    spacing >>[=]{ updateLayout(); };
    indent >>[=]{ updateLayout(); };

    updateLayout();
}

void ColumnLayout::Instance::childReady(View child)
{
    if (child->visible())
    {
        if (align() == ColumnAlign::Left) {
            Size innerSize;
            if (view_->size()[1] > 0)
                innerSize = view_->size() - Size{ indent(), 0 } - 2 * margin();

            const double y = innerSize[1];

            if (child->size()[0] > innerSize[0]) innerSize[0] = child->size()[0];
            innerSize[1] += child->padding() + child->size()[1];
            if (view_->childCount() > 0) innerSize[1] += spacing();

            view_->size = innerSize + Size{ indent(), 0 } + 2 * margin();

            updateChildPos(child, innerSize, y);
            update(view_);
        }
        else
            updateLayout();
    }

    child->size >>[=]{
        if (view_->childCount() > 0) {
            if (child == view_->childAt(view_->childCount() - 1)) {
                double newHeight = child->pos()[1] + child->size()[1] + margin()[1];
                double newWidth = child->size()[0] + indent() + 2 * margin()[0];
                if (newWidth < view_->size()[0]) newWidth = view_->size()[0];
                view_->size = Size{ newWidth, newHeight };
                updateChildPos(child, view_->size() - Size{ indent(), 0 } - 2 * margin(), child->pos()[1]);
                update(view_);
                return;
            }
        }
        updateLayout();
    };

    child->visible >>[=]{ updateLayout(); };
    child->padding >>[=]{ updateLayout(); };
}

void ColumnLayout::Instance::childDone(View child)
{
    updateLayout();
}

void ColumnLayout::Instance::updateChildPos(View child, Size innerSize, double y)
{
    if (align() == ColumnAlign::Auto || align() == ColumnAlign::Left)
        child->pos = Point{ indent(), y } + margin();
    else if (align() == ColumnAlign::Right)
        child->pos = Point{ innerSize[0] - child->size()[0] - indent(), y } + margin();
    else
        child->pos = Point{ 0.5 * (innerSize[0] - child->size()[0]), y } + margin();
}

void ColumnLayout::Instance::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = view_->childCount(); i < n; ++i)
    {
        View child = view_->childAt(i);
        if (!child->visible()) continue;

        if (child->size()[0] > innerSize[0]) innerSize[0] = child->size()[0];
        innerSize[1] += child->padding() + child->size()[1];
        if (i < n - 1) innerSize[1] += spacing();
    }

    view_->size = innerSize + Size{ indent(), 0 } + 2 * margin();

    double y = 0;

    for (int i = 0, n = view_->childCount(); i < n; ++i)
    {
        View child = view_->childAt(i);
        if (!child->visible()) continue;

        y += child->padding();

        updateChildPos(child, innerSize, y);

        y += child->size()[1] + spacing();
    }

    update(view_);
}

}} // namespace cc::ui
