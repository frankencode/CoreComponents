/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Row>

namespace cc {
namespace ui {

Ref<Row> Row::create(View *parent)
{
    return fly(new Row(parent));
}

Row::Row(View *parent):
    View(parent)
{
    align->connect([=]{ updateLayout(); });
    spacing->connect([=]{ updateLayout(); });
}

void Row::childReady(View *child)
{
    updateLayout();
    child->size->connect([=]{ updateLayout(); });
}

void Row::childDone(View *child)
{
    updateLayout();
}

void Row::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);
        if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
        innerSize[0] += child->size()[0];
        if (i < n - 1) innerSize[0] += spacing();
    }

    size = innerSize + 2 * margin();

    double x = 0;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);

        if (align() == RowAlign::Top)
            child->pos = Point{ x, 0 } + margin();
        else if (align() == RowAlign::Bottom)
            child->pos = Point{ x, innerSize[1] - child->size()[1] } + margin();
        else
            child->pos = Point{ x, 0.5 * (innerSize[1] - child->size()[1]) } + margin();

        x += child->size()[0] + spacing();
    }

    update();
}

}} // namespace cc::ui
