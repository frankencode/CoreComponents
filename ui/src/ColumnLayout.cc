/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/ColumnLayout>

namespace cc {
namespace ui {

ColumnLayout *ColumnLayout::setup(View *view)
{
    return new ColumnLayout(view);
}

ColumnLayout::ColumnLayout(View *view):
    Layout(view)
{
    align->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateLayout(); });
    spacing->connect([=]{ updateLayout(); });
    indent->connect([=]{ updateLayout(); });
}

void ColumnLayout::childReady(View *child)
{
    if (child->visible())
    {
        if (align() == ColumnAlign::Left) {
            Size innerSize;
            if (view()->size()[1] > 0)
                innerSize = view()->size() - Size{ indent(), 0 } - 2 * margin();

            const double y = innerSize[1];

            if (child->size()[0] > innerSize[0]) innerSize[0] = child->size()[0];
            innerSize[1] += child->size()[1];
            if (view()->childCount() > 1) innerSize[1] += spacing();

            view()->size = innerSize + Size{ indent(), 0 } + 2 * margin();

            updateChildPos(child, innerSize, y);
            updateView();
        }
        else
            updateLayout();
    }

    child->size->connect([=]{
        if (view()->childCount() > 0) {
            if (child == view()->childAt(view()->childCount() - 1)) {
                double newHeight = child->pos()[1] + child->size()[1] + margin()[1];
                double newWidth = child->size()[0] + indent() + 2 * margin()[0];
                if (newWidth < view()->size()[0]) newWidth = view()->size()[0];
                view()->size = Size{ newWidth, newHeight };
                updateChildPos(child, view()->size() - Size{ indent(), 0 } - 2 * margin(), child->pos()[1]);
                updateView();
                return;
            }
        }
        updateLayout();
    });

    child->visible->connect([=]{ updateLayout(); });
}

void ColumnLayout::childDone(View *child)
{
    updateLayout();
}

void ColumnLayout::updateChildPos(View *child, Size innerSize, double y)
{
    if (align() == ColumnAlign::Auto || align() == ColumnAlign::Left)
        child->pos = Point{ indent(), y } + margin();
    else if (align() == ColumnAlign::Right)
        child->pos = Point{ innerSize[0] - child->size()[0] - indent(), y } + margin();
    else
        child->pos = Point{ 0.5 * (innerSize[0] - child->size()[0]), y } + margin();
}

void ColumnLayout::updateLayout()
{
    Size innerSize;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;

        if (child->size()[0] > innerSize[0]) innerSize[0] = child->size()[0];
        innerSize[1] += child->size()[1];
        if (i < n - 1) innerSize[1] += spacing();
    }

    view()->size = innerSize + Size{ indent(), 0 } + 2 * margin();

    double y = 0;

    for (int i = 0, n = view()->childCount(); i < n; ++i)
    {
        View *child = view()->childAt(i);
        if (!child->visible()) continue;

        updateChildPos(child, innerSize, y);

        y += child->size()[1] + spacing();
    }

    updateView();
}

}} // namespace cc::ui
