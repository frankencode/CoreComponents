/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Row>

namespace cc {
namespace ui {

Row::Row(View *view):
    Organizer(view)
{
    align->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateLayout(); });
    spacing->connect([=]{ updateLayout(); });

    updateLayout();
}

void Row::childReady(View *child)
{
    if (child->visible()) {
        if (align() == RowAlign::Top) {
            Size innerSize;
            if (view()->size()[0] > 0)
                innerSize = view()->size() - 2 * margin();

            const double x = innerSize[0];

            if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
            innerSize[0] += child->padding() + child->size()[0];
            if (view()->childCount() > 1) innerSize[0] += spacing();

            view()->size = innerSize + 2 * margin();

            updateChildPos(child, innerSize, x);
            updateView();
        }
        else
            updateLayout();
    }

    child->size->connect([=]{
        if (view()->childCount() > 0) {
            if (child == view()->childAt(view()->childCount() - 1)) {
                double newWidth = child->pos()[0] + child->size()[0] + margin()[0];
                double newHeight = child->size()[1] + 2 * margin()[1];
                if (newHeight < view()->size()[1]) newHeight = view()->size()[1];
                view()->size = Size{ newWidth, newHeight };
                updateChildPos(child, view()->size() - 2 * margin(), child->pos()[0]);
                updateView();
                return;
            }
        }
        updateLayout();
    });

    child->visible->connect([=]{ updateLayout(); });
    child->padding->connect([=]{ updateLayout(); });
}

void Row::childDone(View *child)
{
    updateLayout();
}

void Row::updateChildPos(View *child, Size innerSize, double x)
{
    if (align() == RowAlign::Top)
        child->pos = Point{ x, 0 } + margin();
    else if (align() == RowAlign::Bottom)
        child->pos = Point{ x, innerSize[1] - child->size()[1] } + margin();
    else
        child->pos = Point{ x, 0.5 * (innerSize[1] - child->size()[1]) } + margin();
}

void Row::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;
        if (child->size()[1] > innerSize[1]) innerSize[1] = child->size()[1];
        innerSize[0] += child->padding() + child->size()[0];
        if (i < n - 1) innerSize[0] += spacing();
    }

    view()->size = innerSize + 2 * margin();

    double x = 0;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;

        x += child->padding();

        updateChildPos(child, innerSize, x);

        x += child->size()[0] + spacing();
    }

    updateView();
}

}} // namespace cc::ui
