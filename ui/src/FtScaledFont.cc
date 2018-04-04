/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtFontMetrics>
#include <cc/ui/FtScaledFont>

namespace cc {
namespace ui {

FtScaledFont::FtScaledFont(const FtFontFace *ftFontFace, double size):
    ftFontFace_(ftFontFace),
    size_(size)
{
    cairoFontFace_ = cairo_ft_font_face_create_for_ft_face(ftFontFace->ftFace(), 0);

    cairo_matrix_t fontMatrix = {
        size, 0,
        0,    size,
        0,    0
    };

    cairo_matrix_t userToDeviceMatrix = {
        1, 0,
        0, 1,
        0, 0
    };

    cairo_font_options_t *fontOptions = cairo_font_options_create();
    cairo_font_options_set_subpixel_order(fontOptions, CAIRO_SUBPIXEL_ORDER_RGB);
    cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_DEFAULT);

    cairoScaledFont_ = cairo_scaled_font_create(cairoFontFace_, &fontMatrix, &userToDeviceMatrix, fontOptions);

    cairo_font_options_destroy(fontOptions);
}

FtScaledFont::~FtScaledFont()
{
    cairo_scaled_font_destroy(cairoScaledFont_);
    cairo_font_face_destroy(cairoFontFace_);
}

Ref<FontMetrics> FtScaledFont::getMetrics() const
{
    FtFaceGuard guard(this);
    FT_Face ftFace = guard->ftFace();

    Ref<FtFontMetrics> metrics = Object::create<FtFontMetrics>();
    metrics->fontSize_ = size_;
    metrics->unitsPerEm_ = ftFace->units_per_EM;
    metrics->ascender_ = ftFace->ascender;
    metrics->descender_ = ftFace->descender;
    metrics->lineHeight_ = ftFace->height;
    metrics->underlinePosition_ = ftFace->underline_position;
    metrics->underlineThickness_ = ftFace->underline_thickness;
    return metrics;
}

}} // namespace cc::ui
