/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FtScaledFont>

namespace cc {

FtScaledFont::State::State(const FtFontFace &fontFace, const Font &font):
    ScaledFont::State{font},
    fontFace_{fontFace}
{
    cairoFontFace_ = cairo_ft_font_face_create_for_ft_face(fontFace.ftFace(), fontFace.pitch() == Pitch::Fixed ? FT_LOAD_FORCE_AUTOHINT : 0);

    double size = font.size();

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

    switch (font.smoothing()) {
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
        default:
            cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_NONE);
            break;
    };

    switch (font.outlineHinting()) {
        case OutlineHinting::Default:
            cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_DEFAULT);
            break;
        case OutlineHinting::None:
            cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_NONE);
            break;
        case OutlineHinting::Slight:
            cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_SLIGHT);
            break;
        case OutlineHinting::Medium:
            cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_MEDIUM);
            break;
        case OutlineHinting::Full:
        default:
            cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_FULL);
            break;
    };

    switch (font.metricsHinting()) {
        case MetricsHinting::Default:
            cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_DEFAULT);
            break;
        case MetricsHinting::On:
            cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_ON);
            break;
        case MetricsHinting::Off:
        default:
            cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_OFF);
            break;
    };

    cairoScaledFont_ = cairo_scaled_font_create(cairoFontFace_, &fontMatrix, &userToDeviceMatrix, fontOptions);

    cairo_font_options_destroy(fontOptions);
}

FtScaledFont::State::~State()
{
    cairo_scaled_font_destroy(cairoScaledFont_);
    cairo_font_face_destroy(cairoFontFace_);
}

FtScaledFont::FtScaledFont(const FtFontFace &fontFace, const Font &font):
    ScaledFont{new State{fontFace, font}}
{
    FtFaceGuard ftFaceGuard{*this};
    FT_Face ftFace = ftFaceGuard.ftFace();

    double scale = font.size() / ftFace->units_per_EM;
    auto &state = metricsState();
    state.fontSize_ = font.size();
    state.ascender_ = scale * ftFace->ascender;
    state.descender_ = scale * ftFace->descender;
    state.lineHeight_ = scale * ftFace->height;
    state.underlinePosition_ = scale * ftFace->underline_position;
    state.underlineThickness_ = scale * ftFace->underline_thickness;
}

} // namespace cc
