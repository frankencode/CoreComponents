/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
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
    Size newSize;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);
        if (child->size()[0] > newSize[0]) newSize[0] = child->size()[0];
        newSize[1] += child->size()[1];
        if (i < n - 1) newSize[1] += spacing();
    }

    size = newSize + Size{ indent(), 0 };

    double y = 0;

    for (int i = 0, n = childCount(); i < n; ++i)
    {
        View *child = childAt(i);

        if (align() == BoxAlign::Auto || align() == BoxAlign::Left)
            child->pos = Point{ indent(), y };
        else if (align() == BoxAlign::Right)
            child->pos = Point{ size()[0] - child->size()[0] - indent(), y };
        else
            child->pos = Point{ 0.5 * (size()[0] - child->size()[0]), y };;

        y += child->size()[1] + spacing();
    }

    update();
}

}} // namespace cc::ui
