/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <cc/assert>
#include <cc/ui/Application>
#include <cc/ui/TypeSetter>

namespace cc {
namespace ui {

Ref<TypeSetter> TypeSetter::create()
{
    return Application::instance()->createTypeSetter();
}

void TypeSetter::layout(TextBlock *block, double *penX, double *penY, Image *image) const
{
    Ref<GlyphRun> run;
    if (block->interactive())
        run = GlyphRun::create();

    if (block->style()->boundary() <= 0) {
        stage(block, penX, penY, image, run);
    }
    else {
        if (!run) run = GlyphRun::create();

        stage(block, 0, 0, 0, run);

        if (run->count() > 0) {
            TextStyle::Bounding bounding = block->style()->bounding();
            if ((bounding == TextStyle::ElideBack) || (bounding == TextStyle::ElideFront))
                elide(block, run);
            else if (bounding == TextStyle::Wrap || bounding == TextStyle::Justify)
                wrap(block, run);
            stage(block, penX, penY, image, run);
        }
    }

    if (block->interactive())
        block->run_ = run;
}

/** \brief measure the size by simulating the layout process
  *
  * The pace() method measures the size of the text block by performing a simulated layout run.
  * This method is also useful to preheat the layout cache.
  * \arg width returns the width
  * \arg height returns the height
  */
void TypeSetter::pace(TextBlock *block, double *width, double *height) const
{
    double penX = 0, penY = 0;
    layout(block, &penX, &penY);
    if (width) *width = penX;
    if (height) *height = FontMetrics::get(block->style()->font())->capitalHeight() + penY;
}

double TypeSetter::extent(GlyphRun *run)
{
    if (run->count() == 0) return 0;
    GlyphStop *first = run->at(0);
    GlyphStop *last = run->at(run->count() - 1);
    return extent(first, last);
}

double TypeSetter::extent(GlyphStop *first, GlyphStop *last)
{
    if (last->penX < first->penX) {
        GlyphStop *h = first;
        first = last;
        last = h;
    }
    return last->penX + last->advanceX - first->penX;
}

void TypeSetter::elide(TextBlock *block, GlyphRun *run) const
{
    TextStyle::Bounding bounding = block->style()->bounding();
    double boundary = block->style()->boundary();
    double x = extent(run);
    if (x <= boundary) return;

    Ref<GlyphRun> ellipsis = GlyphRun::create();
    double g = 0;
    stage(TextBlock::create("...", block->style()), &g, 0, 0, ellipsis);

    if (bounding == TextStyle::ElideBack) {
        x += g;
        while (boundary < x && run->count() > 0)
            x -= fabs(run->popBack()->advanceX);
        double x1 = 0, y1 = 0;
        if (run->count() > 0) {
            GlyphStop *last = run->last();
            x1 = last->penX + last->advanceX;
            y1 = last->penY + last->advanceY;
        }
        for (int i = 0; i < ellipsis->count(); ++i) {
            GlyphStop *stop = ellipsis->at(i);
            stop->penX += x1;
            stop->penY += y1;
            run->pushBack(stop);
        }
    }
    else if (bounding == TextStyle::ElideFront) {
        double x0 = 0, y0 = 0;
        if (run->count() > 0) {
            GlyphStop *first = run->first();
            x0 = first->penX;
            y0 = first->penY;
        }
        x += g;
        double d = 0;
        while (boundary < x && run->count() > 0) {
            double ax = fabs(run->popFront()->advanceX);
            d += ax;
            x -= ax;
        }
        for (int i = 0; i < run->count(); ++i)
            run->at(i)->penX += g - d;
        for (int i = 0; i < ellipsis->count(); ++i) {
            GlyphStop *step = ellipsis->at(i);
            step->penX += x0;
            step->penY += y0;
            run->push(i, step);
        }
    }
}

void TypeSetter::wrap(TextBlock *block, GlyphRun *run) const
{
    if (run->count() == 0) return;
    String text = block->text();
    double boundary = block->style()->boundary();
    int bounding = block->style()->bounding();
    TextStyle::WrapBehind wrapBehind = block->style()->wrapBehind();

    GlyphStop *last = run->last();
    if (last->penX + last->advanceX <= boundary) return;

    GlyphStop *wrapPoint = 0;
    double wrapX = 0;
    int j = -1; // wrap candidate position
    int k = -1; // previous wrap position
    int l = 0; // line start position
    int m = 0; // previous line start position
    double deltaX = 0;
    double deltaY = 0;
    double lineHeight = FontMetrics::get(block->style()->font())->lineHeight();

    for (int i = 0; i < run->count(); ++i) {
        GlyphStop *stop = run->at(i);
        double origX = stop->penX;
        stop->penX += deltaX;
        stop->penY += deltaY;
        if (stop->penX + stop->advanceX >= boundary) {
            if (bounding == TextStyle::Justify)
                spread(block, run, m, l);
            if (j < 0) {
                if (i == k) break;
                k = i;
                stop->penX -= deltaX;
                stop->penY -= deltaY;
                --i;
                deltaX = -origX;
            }
            else {
                for(; i > j; --i) {
                    stop = run->at(i);
                    stop->penX -= deltaX;
                    stop->penY -= deltaY;
                }
                i = j;
                deltaX = -wrapX;
                j = -1;
            }
            m = l;
            l = i + 1;
            deltaY += lineHeight;
        }
        else if (wrapBehind(text, stop->offset)) {
            wrapPoint = stop;
            wrapX = origX + wrapPoint->advanceX;
            j = i;
            continue;
        }
    }

    if (bounding == TextStyle::Justify) {
        if (m < l && l < run->count())
            spread(block, run, m, l);
    }
}

void TypeSetter::spread(TextBlock *block, GlyphRun *run, int i0, int i1) const
{
    double boundary = block->style()->boundary();
    if (boundary <= 0 || i1 <= i0) return;
    int n = 0;
    for (int i = i0; i < i1 - 1; ++i)
        n += (run->at(i)->ch == ' ');
    double g = extent(run->at(i0), run->at(i1 - 1));
    if (boundary <= g) return;
    double h = (boundary - g) / n;
    double dx = 0;
    for (int i = i0; i < i1; ++i) {
        GlyphStop *stop = run->at(i);
        if (dx > 0) stop->penX += dx;
        if (stop->ch == ' ' && i < i1 - 1) {
            stop->advanceX += h;
            dx += h;
        }
    }
}

}} // namespace cc::ui
