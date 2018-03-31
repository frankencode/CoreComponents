/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/RowLayout>

namespace cc {
namespace ui {

RowLayout *RowLayout::setup(View *view)
{
    return new RowLayout(view);
}

RowLayout::RowLayout(View *view):
    Layout(view)
{}

void RowLayout::childReady(View *child)
{
    updateLayout();
    child->size->connect([=]{ updateLayout(); });
}

void RowLayout::childDone(View *child)
{
    updateLayout();
}

void RowLayout::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;
        if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
        innerSize[0] += child->size()[0];
        if (i < n - 1) innerSize[0] += spacing();
    }

    view()->size = innerSize + 2 * margin();

    double x = 0;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;

        if (align() == RowAlign::Top)
            child->pos = Point{ x, 0 } + margin();
        else if (align() == RowAlign::Bottom)
            child->pos = Point{ x, innerSize[1] - child->size()[1] } + margin();
        else
            child->pos = Point{ x, 0.5 * (innerSize[1] - child->size()[1]) } + margin();

        x += child->size()[0] + spacing();
    }

    updateView();
}

}} // namespace cc::ui
