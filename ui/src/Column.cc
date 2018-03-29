/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Column>

namespace cc {
namespace ui {

Ref<Column> Column::create(View *parent)
{
    return fly(new Column(parent));
}

Column::Column(View *parent):
    View(parent)
{
    align->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateLayout(); });
    spacing->connect([=]{ updateLayout(); });
    indent->connect([=]{ updateLayout(); });
}

void Column::childReady(View *child)
{
    updateLayout();
    child->size->connect([=]{ updateLayout(); });
}

void Column::childDone(View *child)
{
    updateLayout();
}

void Column::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);
        if (child->size()[0] > innerSize[0]) innerSize[0] = child->size()[0];
        innerSize[1] += child->size()[1];
        if (i < n - 1) innerSize[1] += spacing();
    }

    size = innerSize + Size{ indent(), 0 } + 2 * margin();

    double y = 0;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);

        if (align() == ColumnAlign::Auto || align() == ColumnAlign::Left)
            child->pos = Point{ indent(), y } + margin();
        else if (align() == ColumnAlign::Right)
            child->pos = Point{ innerSize[0] - child->size()[0] - indent(), y } + margin();
        else
            child->pos = Point{ 0.5 * (innerSize[0] - child->size()[0]), y } + margin();

        y += child->size()[1] + spacing();
    }

    update();
}

}} // namespace cc::ui
