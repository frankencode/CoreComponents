/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Font>
#include <cc/ui/FtScaledFont>

namespace cc {
namespace ui {

FtScaledFont::FtScaledFont(const FtFontFace *ftFontFace, const Font &font):
    font_(font),
    ftFontFace_(ftFontFace)
{
    cairoFontFace_ = cairo_ft_font_face_create_for_ft_face(ftFontFace->ftFace(), 0);

    double size = font->size();

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

    switch (font->smoothing()) {
        case FontSmoothing::Default:
        case FontSmoothing::Grayscale:
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_GRAY);
            break;
        case FontSmoothing::RgbSubpixel:
            cairo_font_options_set_subpixel_order(fontOptions, CAIRO_SUBPIXEL_ORDER_RGB);
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
            break;
        case FontSmoothing::BgrSubpixel:
            cairo_font_options_set_subpixel_order(fontOptions, CAIRO_SUBPIXEL_ORDER_BGR);
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
            break;
        case FontSmoothing::VrgbSubpixel:
            cairo_font_options_set_subpixel_order(fontOptions, CAIRO_SUBPIXEL_ORDER_VRGB);
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
            break;
        case FontSmoothing::VbgrSubpixel:
            cairo_font_options_set_subpixel_order(fontOptions, CAIRO_SUBPIXEL_ORDER_VBGR);
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
            break;
        case FontSmoothing::None:
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_NONE);
            break;
    }

    cairo_font_options_set_hint_style(fontOptions,
        [=](OutlineHinting h) -> cairo_hint_style_t {
            switch (h) {
                case OutlineHinting::Default: return CAIRO_HINT_STYLE_DEFAULT;
                case OutlineHinting::None   : return CAIRO_HINT_STYLE_NONE;
                case OutlineHinting::Slight : return CAIRO_HINT_STYLE_SLIGHT;
                case OutlineHinting::Medium : return CAIRO_HINT_STYLE_MEDIUM;
                case OutlineHinting::Full   : return CAIRO_HINT_STYLE_FULL;
            }
            return CAIRO_HINT_STYLE_DEFAULT;
        }(font->outlineHinting())
    );

    cairo_font_options_set_hint_metrics(fontOptions,
        [=](MetricsHinting h) -> cairo_hint_metrics_t {
            switch (h) {
                case MetricsHinting::Default: return CAIRO_HINT_METRICS_DEFAULT;
                case MetricsHinting::On     : return CAIRO_HINT_METRICS_ON;
                case MetricsHinting::Off    : return CAIRO_HINT_METRICS_OFF;
            }
            return CAIRO_HINT_METRICS_DEFAULT;
        }(font->metricsHinting())
    );

    cairoScaledFont_ = cairo_scaled_font_create(cairoFontFace_, &fontMatrix, &userToDeviceMatrix, fontOptions);

    cairo_font_options_destroy(fontOptions);
}

FtScaledFont::~FtScaledFont()
{
    cairo_scaled_font_destroy(cairoScaledFont_);
    cairo_font_face_destroy(cairoFontFace_);
}

const FontMetrics *FtScaledFont::getMetrics() const
{
    if (!ftFontMetrics_) {
        auto metrics = Object::create<FtFontMetrics>();

        FtFaceGuard guard(this);
        FT_Face ftFace = guard->ftFace();
        metrics->fontSize_ = font_->size();
        metrics->unitsPerEm_ = ftFace->units_per_EM;
        metrics->ascender_ = ftFace->ascender;
        metrics->descender_ = ftFace->descender;
        metrics->lineHeight_ = ftFace->height;
        metrics->underlinePosition_ = ftFace->underline_position;
        metrics->underlineThickness_ = ftFace->underline_thickness;

        ftFontMetrics_ = metrics;
    }

    return ftFontMetrics_;
}

Step FtScaledFont::getGlyphAdvance(unsigned int glyphIndex) const
{
    FtFaceGuard guard(this);
    FT_Face ftFace = guard->ftFace();

    if (FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT) != 0)
        return Step{};

    return Step {
        double(ftFace->glyph->advance.x),
        double(ftFace->glyph->advance.y)
    } / 64.;
}

}} // namespace cc::ui
