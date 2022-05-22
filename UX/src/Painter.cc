/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Painter>
#include <cc/FtScaledFont>
#include <cc/FtGlyphRun>
#include <cc/FtTextRun>
#include <cc/Surface>
#include <cairo/cairo.h>
#include <cmath>

namespace cc {

static_assert(+LineJoin::Miter == CAIRO_LINE_JOIN_MITER);
static_assert(+LineJoin::Round == CAIRO_LINE_JOIN_ROUND);
static_assert(+LineJoin::Bevel == CAIRO_LINE_JOIN_BEVEL);

static_assert(+LineCap::Butt == CAIRO_LINE_CAP_BUTT);
static_assert(+LineCap::Round == CAIRO_LINE_CAP_ROUND);
static_assert(+LineCap::Square == CAIRO_LINE_CAP_SQUARE);

Painter::Painter(const Surface &surface):
    Painter{&const_cast<Surface &>(surface).me()}
{}

Painter::Painter(Surface::State *state):
    state_{state}
{
    state_->polish();
    cr_ = cairo_create(state->cairoSurface());
}

Painter::~Painter()
{
    state_->finish();
}

Painter &Painter::save()
{
    cairo_save(cr_);
    return *this;
}

Painter &Painter::restore()
{
    cairo_restore(cr_);
    return *this;
}

Painter &Painter::nextPage(bool clear)
{
    if (clear)
        cairo_show_page(cr_);
    else
        cairo_copy_page(cr_);

    return *this;
}

Painter &Painter::newPath()
{
    cairo_new_path(cr_);
    return *this;
}

Painter &Painter::newSubPath()
{
    cairo_new_sub_path(cr_);
    return *this;
}

Painter &Painter::closePath()
{
    cairo_close_path(cr_);
    return *this;
}

bool Painter::hasCurrentPoint() const
{
    return cairo_has_current_point(cr_);
}

Point Painter::currentPoint() const
{
    Point c;
    cairo_get_current_point(cr_, &c[0], &c[1]);
    return c;
}

Painter &Painter::moveTo(Point c)
{
    cairo_move_to(cr_, c[0], c[1]);
    return *this;
}

Painter &Painter::lineTo(Point c)
{
    cairo_line_to(cr_, c[0], c[1]);
    return *this;
}

Painter &Painter::curveTo(Point a, Point b, Point c)
{
    cairo_curve_to(
        cr_,
        a[0], a[1],
        b[0], b[1],
        c[0], c[1]
    );
    return *this;
}

Painter &Painter::arc(Point m, double r, double alpha0, double alpha1)
{
    cairo_arc(cr_, m[0], m[1], r, alpha0, alpha1);
    return *this;
}

Painter &Painter::arcNegative(Point m, double r, double alpha0, double alpha1)
{
    cairo_arc_negative(cr_, m[0], m[1], r, alpha0, alpha1);
    return *this;
}

Painter &Painter::rectangle(Point pos, Size size)
{
    cairo_rectangle(cr_, pos[0], pos[1], size[0], size[1]);
    return *this;
}

Rect Painter::pathExtents() const
{
    double x0 = 0;
    double y0 = 0;
    double x1 = 0;
    double y1 = 0;
    cairo_path_extents(cr_, &x0, &y0, &x1, &y1);
    return Rect{Point{x0, y0}, Size{x1-x0, y1-y0}};
}

Rect Painter::fillExtents() const
{
    double x0 = 0;
    double y0 = 0;
    double x1 = 0;
    double y1 = 0;
    cairo_fill_extents(cr_, &x0, &y0, &x1, &y1);
    return Rect{Point{x0, y0}, Size{x1-x0, y1-y0}};
}

Painter &Painter::showGlyphRun(Point pos, const GlyphRun &glyphRun)
{
    FtGlyphRun ftGlyphRun = glyphRun.as<FtGlyphRun>();
    if (!glyphRun.scaledFont()) return *this;
    auto ftScaledFont = glyphRun.scaledFont().as<FtScaledFont>();

    if (ftGlyphRun.text().count() == 0) return *this;
    if (ftGlyphRun.cairoGlyphs().count() == 0) return *this;

    cairo_set_scaled_font(cr_, ftScaledFont.cairoScaledFont());

    cairo_matrix_t matrixSaved;
    cairo_get_matrix(cr_, &matrixSaved);
    cairo_translate(cr_, pos[0], pos[1]);

    cairo_pattern_t *sourceSaved = 0;

    if (glyphRun.font().paper().isValid()) {
        sourceSaved = cairo_pattern_reference(cairo_get_source(cr_));
        fillGlyphRunBackground(ftGlyphRun);
        cairo_set_source(cr_, sourceSaved);
    }

    if (glyphRun.font().color().isValid()) {
        if (!sourceSaved) sourceSaved = cairo_pattern_reference(cairo_get_source(cr_));
        setPen(glyphRun.font().color());
    }

    Decoration decoration = ftGlyphRun.font().decoration();
    if (decoration != Decoration::None)
    {
        double u = 0;
        double v = 0; {
            FontMetrics metrics = ftGlyphRun.scaledFont().metrics();
            v = metrics.underlineThickness();
            if (decoration & Decoration::Underline)
                u = -metrics.underlinePosition() + v / 2;
            else if (decoration & Decoration::StrikeOut)
                u = -2 * metrics.ascender() / 5 + v / 2;
        }

        String text = ftGlyphRun.text();

        const cairo_glyph_t *glyph0 = &ftGlyphRun.cairoGlyphs()[0];
        char ch0 = text[0];

        int byteOffset = 0;
        int glyphOffset = 0;

        for (
            int clusterIndex = 1, clusterCount = ftGlyphRun.cairoTextClusters().count();
            clusterIndex < clusterCount;
            ++clusterIndex
        ) {
            {
                const cairo_text_cluster_t *cluster = &ftGlyphRun.cairoTextClusters()[clusterIndex - 1];
                byteOffset += cluster->num_bytes;
                glyphOffset += cluster->num_glyphs;
            }
            const cairo_glyph_t *glyph1 = &ftGlyphRun.cairoGlyphs()[glyphOffset];
            char ch1 = text.at(byteOffset);
            if (ch0 > 0x20 && ch1 <= 0x20) {
                rectangle(
                    Point{ glyph0->x            , std::round(glyph0->y + u) },
                    Size { glyph1->x - glyph0->x, v                         }
                );
                fill();
                glyph0 = glyph1;
            }
            else if (glyph0->y != glyph1->y) {
                glyph0 = glyph1;
            }
            ch0 = ch1;
        }

        if (ftGlyphRun.advance()[0] - glyph0->x > 0) {
            rectangle(
                Point{ glyph0->x                          , std::round(glyph0->y + u) },
                Size { ftGlyphRun.advance()[0] - glyph0->x, v                         }
            );
            fill();
        }
    }

    int byteCheck = 0;
    for (int i = 0; i < ftGlyphRun.cairoTextClusters().count(); ++i)
        byteCheck += ftGlyphRun.cairoTextClusters()[i].num_bytes;

    if (byteCheck == ftGlyphRun.text().count()) {
        cairo_show_text_glyphs(
            cr_,
            ftGlyphRun.text().chars(),
            ftGlyphRun.text().count(),
            ftGlyphRun.cairoGlyphs().items(),
            ftGlyphRun.cairoGlyphs().count(),
            ftGlyphRun.cairoTextClusters().items(),
            ftGlyphRun.cairoTextClusters().count(),
            (cairo_text_cluster_flags_t)0
        );
    }

    if (sourceSaved) {
        cairo_set_source(cr_, sourceSaved);
        cairo_pattern_destroy(sourceSaved);
    }

    cairo_set_matrix(cr_, &matrixSaved);
    return *this;
}

Painter &Painter::showGlyphRun(Point pos, const GlyphRun &run, const GetColor &ink, const GetColor &paper)
{
    auto ftGlyphRun = run.as<FtGlyphRun>();
    auto ftScaledFont = run.scaledFont().as<FtScaledFont>();

    if (ftGlyphRun.text().count() == 0) return *this;
    if (ftGlyphRun.cairoGlyphs().count() == 0) return *this;

    cairo_set_scaled_font(cr_, ftScaledFont.cairoScaledFont());

    cairo_matrix_t matrixSaved;
    cairo_get_matrix(cr_, &matrixSaved);
    cairo_translate(cr_, pos[0], pos[1]);

    cairo_pattern_t *sourceSaved = cairo_pattern_reference(cairo_get_source(cr_));

    if (ftGlyphRun.font().paper().isValid())
        fillGlyphRunBackground(ftGlyphRun);

    if (paper)
    {
        FontMetrics metrics = ftScaledFont.metrics();
        double dy0 = metrics.ascender() + (metrics.lineHeight() - (metrics.ascender() - metrics.descender())) / 2;

        int byteOffset = 0;
        int glyphOffset = 0;

        Color bgColor0 = paper(0);
        const cairo_glyph_t *glyph0 = &ftGlyphRun.cairoGlyphs()[0];

        for (int clusterIndex = 0, clusterCount = ftGlyphRun.cairoTextClusters().count(); clusterIndex < clusterCount + 1; ++clusterIndex)
        {
            const cairo_text_cluster_t *cluster = nullptr;
            if (clusterIndex < clusterCount) cluster = &ftGlyphRun.cairoTextClusters()[clusterIndex];

            Color bgColor;
            const cairo_glyph_t *glyph = nullptr;

            bgColor = paper(byteOffset);
            if (glyphOffset < ftGlyphRun.cairoGlyphs().count()) glyph = &ftGlyphRun.cairoGlyphs()[glyphOffset];

            if (bgColor != bgColor0 || !glyph || glyph->y != glyph0->y)
            {
                double x1 = (!glyph || glyph->y != glyph0->y) ?
                    ftGlyphRun.cairoGlyphs()[glyphOffset - 1].x + ftGlyphRun.glyphAdvances()[glyphOffset - 1] :
                    glyph->x;

                if (bgColor0) {
                    setPen(bgColor0);
                    rectangle(
                        Point{glyph0->x, glyph0->y - dy0},
                        Size{x1 - glyph0->x, metrics.lineHeight()}
                    );
                    fill();
                }

                bgColor0 = bgColor;
                glyph0 = glyph;
            }

            if (cluster) {
                byteOffset += cluster->num_bytes;
                glyphOffset += cluster->num_glyphs;
            }
        }
    }

    if (ink)
    {
        int byteOffset = 0;
        int glyphOffset = 0;

        Color fgColor0 = ink(0);

        int byteOffset0 = 0;
        int glyphOffset0 = 0;
        int clusterIndex0 = 0;

        for (int clusterIndex = 0, clusterCount = ftGlyphRun.cairoTextClusters().count(); clusterIndex < clusterCount;)
        {
            const cairo_text_cluster_t *cluster = &ftGlyphRun.cairoTextClusters()[clusterIndex];

            byteOffset += cluster->num_bytes;
            glyphOffset += cluster->num_glyphs;
            ++clusterIndex;

            Color fgColor = ink(byteOffset);

            if (fgColor0 != fgColor || clusterIndex == clusterCount)
            {
                if (fgColor0) setPen(fgColor0);
                else if (ftGlyphRun.font().color()) setPen(ftGlyphRun.font().color());
                else cairo_set_source(cr_, sourceSaved);

                cairo_show_text_glyphs(
                    cr_,
                    &ftGlyphRun.text()[byteOffset0],
                    byteOffset - byteOffset0,
                    &ftGlyphRun.cairoGlyphs()[glyphOffset0],
                    glyphOffset - glyphOffset0,
                    &ftGlyphRun.cairoTextClusters()[clusterIndex0],
                    clusterIndex - clusterIndex0,
                    (cairo_text_cluster_flags_t)0
                );

                fgColor0 = fgColor;
                byteOffset0 = byteOffset;
                glyphOffset0 = glyphOffset;
                clusterIndex0 = clusterIndex;
            }
        }

        cairo_set_source(cr_, sourceSaved);
    }
    else {
        cairo_set_source(cr_, sourceSaved);

        cairo_show_text_glyphs(
            cr_,
            ftGlyphRun.text().chars(),
            ftGlyphRun.text().count(),
            ftGlyphRun.cairoGlyphs().items(),
            ftGlyphRun.cairoGlyphs().count(),
            ftGlyphRun.cairoTextClusters().items(),
            ftGlyphRun.cairoTextClusters().count(),
            (cairo_text_cluster_flags_t)0
        );
    }

    cairo_pattern_destroy(sourceSaved);

    cairo_set_matrix(cr_, &matrixSaved);
    return *this;
}

Painter &Painter::showTextRun(Point pos, const TextRun &run)
{
    for (const GlyphRun &glyphRun: run.as<FtTextRun>().glyphRuns()) {
        showGlyphRun(pos, glyphRun);
    }
    return *this;
}

Painter &Painter::showTextRun(Point pos, const TextRun &run, const GetColor &ink, const GetColor &paper)
{
    FtTextRun ftTextRun = run.as<FtTextRun>();
    int i0 = 0;
    GetColor ink_;
    GetColor paper_;
    if (ink) ink_ = [&](int i) { return ink(i + i0); };
    if (paper) paper_ = [&](int i) { return paper(i + i0); };
    for (const GlyphRun &glyphRun: ftTextRun.glyphRuns()) {
        showGlyphRun(pos, glyphRun, ink_, paper_);
        i0 += glyphRun.text().count();
    }
    return *this;
}

void Painter::fillGlyphRunBackground(const FtGlyphRun &ftGlyphRun)
{
    Color color = ftGlyphRun.font().paper();
    if (!color.isValid()) return;

    setPen(color);

    FontMetrics metrics = ftGlyphRun.scaledFont().metrics();
    double dy0 = metrics.ascender() + (metrics.lineHeight() - (metrics.ascender() - metrics.descender())) / 2;

    int byteOffset = 0;
    int glyphOffset = 0;

    const cairo_glyph_t *glyph0 = &ftGlyphRun.cairoGlyphs()[0];

    for (int clusterIndex = 0, clusterCount = ftGlyphRun.cairoTextClusters().count(); clusterIndex < clusterCount + 1; ++clusterIndex)
    {
        const cairo_text_cluster_t *cluster = nullptr;
        if (clusterIndex < clusterCount) cluster = &ftGlyphRun.cairoTextClusters()[clusterIndex];

        const cairo_glyph_t *glyph = nullptr;

        if (glyphOffset < ftGlyphRun.cairoGlyphs().count()) glyph = &ftGlyphRun.cairoGlyphs()[glyphOffset];

        if (!glyph || glyph->y != glyph0->y) {
            double x1 = (!glyph || glyph->y != glyph0->y) ? ftGlyphRun.size()[0] : glyph->x;
            rectangle(
                Point{glyph0->x, glyph0->y - dy0},
                Size{x1 - glyph0->x, metrics.lineHeight()}
            );
            fill();

            glyph0 = glyph;
        }

        if (cluster) {
            byteOffset += cluster->num_bytes;
            glyphOffset += cluster->num_glyphs;
        }
    }
}

Painter &Painter::translate(Step step)
{
    cairo_translate(cr_, step[0], step[1]);
    return *this;
}

Painter &Painter::scale(Scale ratio)
{
    cairo_scale(cr_, ratio[0], ratio[1]);
    return *this;
}

Painter &Painter::rotate(double angle)
{
    cairo_rotate(cr_, angle);
    return *this;
}

Painter &Painter::setPen(Color color)
{
    if (!color) return *this;

    if (color.isOpaque()) {
        cairo_set_source_rgb(
            cr_,
            color.red()   / 255.,
            color.green() / 255.,
            color.blue()  / 255.
        );
    }
    else {
        cairo_set_source_rgba(
            cr_,
            color.red()   / 255.,
            color.green() / 255.,
            color.blue()  / 255.,
            color.alpha() / 255.
        );
    }

    return *this;
}

Painter &Painter::setPen(const Pen &pen)
{
    setPen(pen.color());
    setLineWidth(pen.lineWidth());
    setLineCap(pen.lineCap());
    setLineJoin(pen.lineJoin());
    setMiterLimit(pen.miterLimit());
    setLineDash(pen.lineDash(), pen.lineDashOffset());
    return *this;
}

void Painter::setLineWidth(double width)
{
    if (width > 0) cairo_set_line_width(cr_, width);
}

void Painter::setLineCap(LineCap style)
{
    if (style != LineCap::Undef) cairo_set_line_cap(cr_, static_cast<cairo_line_cap_t>(+style));
}

void Painter::setLineJoin(LineJoin style)
{
    if (style != LineJoin::Undef) cairo_set_line_join(cr_, static_cast<cairo_line_join_t>(+style));
}

void Painter::setMiterLimit(double limit)
{
    if (limit > 0) cairo_set_miter_limit(cr_, limit);
}

void Painter::setLineDash(const Array<double> &pattern, double offset)
{
    if (pattern.count() > 0) {
        cairo_set_dash(cr_, pattern.items(), pattern.count(), offset);

        cairo_status_t status = cairo_status(cr_);
        if (status != CAIRO_STATUS_SUCCESS)
            throw PainterError{int(status)};
    }
    else if (cairo_get_dash_count(cr_) > 0) {
        cairo_set_dash(cr_, pattern.items(), pattern.count(), offset);
    }
}

Painter &Painter::fill(CurrentPath mode)
{
    if (mode == CurrentPath::Clear)
        cairo_fill(cr_);
    else
        cairo_fill_preserve(cr_);

    cairo_status_t status = cairo_status(cr_);
    if (status != CAIRO_STATUS_SUCCESS) throw PainterError{int(status)};

    return *this;
}

Painter &Painter::stroke(CurrentPath mode)
{
    if (mode == CurrentPath::Clear)
        cairo_stroke(cr_);
    else
        cairo_stroke_preserve(cr_);

    cairo_status_t status = cairo_status(cr_);
    if (status != CAIRO_STATUS_SUCCESS) throw PainterError{int(status)};

    return *this;
}

PainterError::PainterError(int errorStatus):
    errorStatus_{errorStatus}
{}

String PainterError::message() const
{
    return cairo_status_to_string(cairo_status_t(errorStatus_));
}

} // namespace cc
