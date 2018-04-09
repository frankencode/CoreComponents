/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cairo/cairo.h>
#include <cc/ui/Surface>
#include <cc/ui/FtGlyphRun>
#include <cc/ui/FtTextRun>
#include <cc/ui/Painter>

namespace cc {
namespace ui {

Painter::Instance::Instance(Surface *surface):
    cr_(cairo_create(surface->cairoSurface()))
{}

Painter::Instance::~Instance()
{
    cairo_destroy(cr_);
}

void Painter::Instance::newPath()
{
    cairo_new_path(cr_);
}

void Painter::Instance::newSubPath()
{
    cairo_new_sub_path(cr_);
}

void Painter::Instance::closePath()
{
    cairo_close_path(cr_);
}

bool Painter::Instance::hasCurrentPoint() const
{
    return cairo_has_current_point(cr_);
}

Point Painter::Instance::currentPoint() const
{
    Point p;
    cairo_get_current_point(cr_, &p[0], &p[1]);
    return p;
}

void Painter::Instance::moveTo(Point nextPoint)
{
    cairo_move_to(cr_, nextPoint[0], nextPoint[1]);
}

void Painter::Instance::lineTo(Point nextPoint)
{
    cairo_line_to(cr_, nextPoint[0], nextPoint[1]);
}

void Painter::Instance::curveTo(Point controlPointA, Point controlPointB, Point endPoint)
{
    cairo_curve_to(
        cr_,
        controlPointA[0], controlPointA[1],
        controlPointB[0], controlPointB[1],
        endPoint[0], endPoint[1]
    );
}

void Painter::Instance::arc(Point center, double radius, double startAngle, double stopAngle)
{
    cairo_arc(cr_, center[0], center[1], radius, startAngle, stopAngle);
}

void Painter::Instance::arcNegative(Point center, double radius, double startAngle, double stopAngle)
{
    cairo_arc_negative(cr_, center[0], center[1], radius, startAngle, stopAngle);
}

void Painter::Instance::relMoveTo(Step delta)
{
    cairo_rel_move_to(cr_, delta[0], delta[1]);
}

void Painter::Instance::relLineTo(Step delta)
{
    cairo_rel_line_to(cr_, delta[0], delta[1]);
}

void Painter::Instance::relCurveTo(Step deltaA, Step deltaB, Step deltaE)
{
    cairo_curve_to(
        cr_,
        deltaA[0], deltaA[1],
        deltaB[0], deltaB[1],
        deltaE[0], deltaE[1]
    );
}

void Painter::Instance::rectangle(Point position, Size size)
{
    cairo_rectangle(cr_, position[0], position[1], size[0], size[1]);
}

void Painter::Instance::circle(Point center, double radius)
{
    cairo_arc(cr_, center[0], center[1], radius, 0, 2 * M_PI);
}

void Painter::Instance::setSource(Color color)
{
    cairo_set_source_rgba(
        cr_,
        Color::redComponent(color)/255.,
        Color::greenComponent(color)/255.,
        Color::blueComponent(color)/255.,
        Color::alphaComponent(color)/255.
    );
}

void Painter::Instance::fill()
{
    cairo_fill(cr_);
    checkForError();
}

void Painter::Instance::fillPreserve()
{
    cairo_fill_preserve(cr_);
    checkForError();
}

void Painter::Instance::showGlyphRun(Point pos, const GlyphRun *glyphRun)
{
    if (glyphRun->text()->count() == 0) return;

    const FtGlyphRun *ftGlyphRun = Object::cast<const FtGlyphRun *>(glyphRun);
    cairo_set_scaled_font(cr_, ftGlyphRun->ftFont()->cairoScaledFont());

    cairo_matrix_t matrixSaved;
    cairo_get_matrix(cr_, &matrixSaved);
    cairo_translate(cr_, pos[0], pos[1]);

    cairo_pattern_t *sourceSaved = 0;
    if (Color::isValid(glyphRun->font()->color())) {
        sourceSaved = cairo_pattern_reference(cairo_get_source(cr_));
        setSource(glyphRun->font()->color());
    }

    Decoration decoration = ftGlyphRun->font()->decoration();
    if (decoration != Decoration::None)
    {
        double u = 0;
        double v = 0; {
            Ref<const FontMetrics> metrics = ftGlyphRun->font()->getScaledFont()->getMetrics();
            v = metrics->underlineThickness();
            if (+(decoration & Decoration::Underline))
                u = -metrics->underlinePosition() + v / 2;
            else if (+(decoration & Decoration::StrikeOut))
                u = std::floor(-2 * metrics->ascender() / 5 + v / 2);
        }

        const CharArray *text = ftGlyphRun->text();

        const cairo_glyph_t *glyph0 = &ftGlyphRun->cairoGlyphs()->at(0);
        char ch0 = text->at(0);

        int byteOffset = 0;
        int glyphOffset = 0;

        for (
            int clusterIndex = 1, clusterCount = ftGlyphRun->cairoTextClusters()->count();
            clusterIndex < clusterCount;
            ++clusterIndex
        ) {
            {
                const cairo_text_cluster_t *cluster = &ftGlyphRun->cairoTextClusters()->at(clusterIndex - 1);
                byteOffset += cluster->num_bytes;
                glyphOffset += cluster->num_glyphs;
            }
            const cairo_glyph_t *glyph1 = &ftGlyphRun->cairoGlyphs()->at(glyphOffset);
            char ch1 = text->at(byteOffset);
            if (ch0 > 0x20 && ch1 <= 0x20) {
                rectangle(
                    Point{ glyph0->x            , glyph0->y + u },
                    Size { glyph1->x - glyph0->x, v             }
                );
                fill();
                glyph0 = glyph1;
            }
            else if (glyph0->y != glyph1->y) {
                glyph0 = glyph1;
            }
            ch0 = ch1;
        }

        if (ftGlyphRun->advance()[0] - glyph0->x > 0) {
            rectangle(
                Point{ glyph0->x                           , glyph0->y + u },
                Size { ftGlyphRun->advance()[0] - glyph0->x, v             }
            );
            fill();
        }
    }

    cairo_show_text_glyphs(
        cr_,
        ftGlyphRun->text()->chars(),
        ftGlyphRun->text()->count(),
        ftGlyphRun->cairoGlyphs()->data(),
        ftGlyphRun->cairoGlyphs()->count(),
        ftGlyphRun->cairoTextClusters()->data(),
        ftGlyphRun->cairoTextClusters()->count(),
        (cairo_text_cluster_flags_t)0
    );

    if (sourceSaved) {
        cairo_set_source(cr_, sourceSaved);
        cairo_pattern_destroy(sourceSaved);
    }

    cairo_set_matrix(cr_, &matrixSaved);
}

void Painter::Instance::showGlyphRun(Point pos, const GlyphRun *glyphRun, const Array<Color> *fgColors, const Array<Color> *bgColors)
{
    if (glyphRun->text()->count() == 0) return;

    const FtGlyphRun *ftGlyphRun = Object::cast<const FtGlyphRun *>(glyphRun);
    cairo_set_scaled_font(cr_, ftGlyphRun->ftFont()->cairoScaledFont());

    cairo_matrix_t matrixSaved;
    cairo_get_matrix(cr_, &matrixSaved);
    cairo_translate(cr_, pos[0], pos[1]);

    cairo_pattern_t *sourceSaved = cairo_pattern_reference(cairo_get_source(cr_));

    if (bgColors && bgColors->count() > 0)
    {
        Ref<const FontMetrics> metrics = glyphRun->font()->getScaledFont()->getMetrics();
        double dy0 = metrics->ascender() + (metrics->lineHeight() - (metrics->ascender() - metrics->descender())) / 2;

        int byteOffset = 0;
        int glyphOffset = 0;

        Color bgColor0 = bgColors->at(0);
        const cairo_glyph_t *glyph0 = &ftGlyphRun->cairoGlyphs()->at(0);

        for (int clusterIndex = 0, clusterCount = ftGlyphRun->cairoTextClusters()->count(); clusterIndex < clusterCount + 1; ++clusterIndex)
        {
            const cairo_text_cluster_t *cluster = 0;
            if (clusterIndex < clusterCount) cluster = &ftGlyphRun->cairoTextClusters()->at(clusterIndex);

            Color bgColor;
            const cairo_glyph_t *glyph = 0;

            if (byteOffset < bgColors->count()) bgColor = bgColors->at(byteOffset);
            if (glyphOffset < ftGlyphRun->cairoGlyphs()->count()) glyph = &ftGlyphRun->cairoGlyphs()->at(glyphOffset);

            if (bgColor != bgColor0 || !glyph || glyph->y != glyph0->y) {
                double x1 = (!glyph || glyph->y != glyph0->y) ? ftGlyphRun->size()[0] : glyph->x;
                rectangle(
                    Point{glyph0->x, glyph0->y - dy0},
                    Size{x1 - glyph0->x, metrics->lineHeight()}
                );
                setSource(bgColor0);
                fill();

                bgColor0 = bgColor;
                glyph0 = glyph;
            }

            if (cluster) {
                byteOffset += cluster->num_bytes;
                glyphOffset += cluster->num_glyphs;
            }
        }
    }

    if (fgColors && fgColors->count() > 0)
    {
        int byteOffset = 0;
        int glyphOffset = 0;

        Color fgDefaultColor = ftGlyphRun->font()->color();
        Color fgColor0 = fgColors->at(0);
        if (!Color::isValid(fgColor0)) fgColor0 = fgDefaultColor;
        int byteOffset0 = 0;
        int glyphOffset0 = 0;
        int clusterIndex0 = 0;

        for (int clusterIndex = 0; clusterIndex < ftGlyphRun->cairoTextClusters()->count();)
        {
            const cairo_text_cluster_t *cluster = &ftGlyphRun->cairoTextClusters()->at(clusterIndex);

            byteOffset += cluster->num_bytes;
            glyphOffset += cluster->num_glyphs;
            ++clusterIndex;

            Color fgColor;
            if (fgColors->has(byteOffset)) {
                fgColor = fgColors->at(byteOffset);
                if (!Color::isValid(fgColor)) fgColor = fgDefaultColor;
            }

            if (fgColor0 != fgColor) {
                setSource(fgColor0);
                cairo_show_text_glyphs(
                    cr_,
                    ftGlyphRun->text()->chars() + byteOffset0,
                    byteOffset - byteOffset0,
                    ftGlyphRun->cairoGlyphs()->data() + glyphOffset0,
                    glyphOffset - glyphOffset0,
                    ftGlyphRun->cairoTextClusters()->data() + clusterIndex0,
                    clusterIndex - clusterIndex0,
                    (cairo_text_cluster_flags_t)0
                );

                fgColor0 = fgColor;
                byteOffset0 = byteOffset;
                glyphOffset0 = glyphOffset;
                clusterIndex0 = clusterIndex;
            }
        }
    }
    else {
        cairo_show_text_glyphs(
            cr_,
            ftGlyphRun->text()->chars(),
            ftGlyphRun->text()->count(),
            ftGlyphRun->cairoGlyphs()->data(),
            ftGlyphRun->cairoGlyphs()->count(),
            ftGlyphRun->cairoTextClusters()->data(),
            ftGlyphRun->cairoTextClusters()->count(),
            (cairo_text_cluster_flags_t)0
        );
    }

    cairo_set_source(cr_, sourceSaved);
    cairo_pattern_destroy(sourceSaved);

    cairo_set_matrix(cr_, &matrixSaved);
}

void Painter::Instance::showTextRun(Point pos, const TextRun *textRun)
{
    const FtTextRun *ftTextRun = Object::cast<const FtTextRun *>(textRun);
    for (const GlyphRun *glyphRun: ftTextRun->glyphRuns_)
        showGlyphRun(pos, glyphRun);
}

void Painter::Instance::translate(Step step)
{
    cairo_translate(cr_, step[0], step[1]);
}

void Painter::Instance::scale(Scale ratio)
{
    cairo_scale(cr_, ratio[0], ratio[1]);
}

void Painter::Instance::rotate(double angle)
{
    cairo_rotate(cr_, angle);
}

void Painter::Instance::save()
{
    cairo_save(cr_);
}

void Painter::Instance::restore()
{
    cairo_restore(cr_);
}

void Painter::Instance::nextPage(bool clear)
{
    if (clear)
        cairo_show_page(cr_);
    else
        cairo_copy_page(cr_);
}

void Painter::Instance::checkForError()
{
    cairo_status_t status = cairo_status(cr_);
    if (status != CAIRO_STATUS_SUCCESS)
        throw PainterError(int(status));
}

PainterError::PainterError(int errorStatus):
    errorStatus_(errorStatus)
{}

String PainterError::message() const
{
    return cairo_status_to_string(cairo_status_t(errorStatus_));
}

}} // namespace cc::ui
