/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/System>
#include <cc/ui/ScaledFont>
#include <cc/ui/StyleManager>
#include <cc/ui/FontManager>
#include <cc/ui/Application>
#include <cc/ui/Font>

namespace cc {
namespace ui {

Font::Instance::Instance():
    size_{-1},
    stretch_ { uint8_t(Stretch::Normal) },
    decoration_ { uint8_t(Decoration::None) },
    smoothing_ { uint8_t(FontSmoothing::Default) },
    outlineHinting_ { uint8_t(OutlineHinting::Default) },
    metricsHinting_ { uint8_t(MetricsHinting::Default) }
{}

Font::Instance::Instance(double size):
    size_{float(size)},
    stretch_ { uint8_t(Stretch::Normal) },
    decoration_ { uint8_t(Decoration::None) },
    smoothing_ { uint8_t(FontSmoothing::Default) },
    outlineHinting_ { uint8_t(OutlineHinting::Default) },
    metricsHinting_ { uint8_t(MetricsHinting::Default) }
{}

Font::Instance::Instance(String family, double size):
    family_{family},
    size_{float(size)},
    stretch_ { uint8_t(Stretch::Normal) },
    decoration_ { uint8_t(Decoration::None) },
    smoothing_ { uint8_t(FontSmoothing::Default) },
    outlineHinting_ { uint8_t(OutlineHinting::Default) },
    metricsHinting_ { uint8_t(MetricsHinting::Default) }
{}

Font::Instance::~Instance()
{}

Font::Instance::Instance(const Instance &b)
{
    *this = b;
}

Ref<const FontMetrics> Font::Instance::getMetrics() const
{
    return getScaledFont()->metrics();
}

Ref<const ScaledFont> Font::Instance::getScaledFont() const
{
    return FontManager::instance()->selectFont(*this);
}

StylePlugin *Font::style()
{
    return StyleManager::instance()->activePlugin();
}

Font &Font::operator<<(Pitch pitch)
{
    (*this)->setFamily(
        (pitch == Pitch::Fixed) ?
        style()->defaultFixedFont()->family() :
        style()->defaultFont()->family()
    );
    if ((*this)->size() <= 0 && pitch == Pitch::Fixed)
        (*this)->setSize(style()->defaultFixedFont()->size());
    return *this;
}

Font &Font::operator*=(double scale)
{
    double size = (*this)->size();
    if (size <= 0) size = style()->defaultFixedFont()->size() * scale;
    else size = size * scale;
    (*this)->setSize(size);
    return *this;
}

bool Font::differ(const Font &a, const Font &b)
{
    return
        a->family_         != b->family_         ||
        a->size_           != b->size_           ||
        a->weight_         != b->weight_         ||
        a->slant_          != b->slant_          ||
        a->stretch_        != b->stretch_        ||
        a->decoration_     != b->decoration_     ||
        a->smoothing_      != b->smoothing_      ||
        a->outlineHinting_ != b->outlineHinting_ ||
        a->metricsHinting_ != b->metricsHinting_ ||
        a->ink_            != b->ink_            ||
        a->paper_          != b->paper_;
}

String str(const Font &font)
{
    return Format()
        << "Font {" << nl
        << "  family: " << font->family() << nl
        << "  size: " << font->size() << nl
        << "  weight: " << font->weight() << nl
        << "  slant: " << font->slant() << nl
        << "  stretch: " << font->stretch() << nl
        << "  decoration: " << font->decoration() << nl
        << "  smoothing: " << font->smoothing() << nl
        << "  outlineHinting: " << font->outlineHinting() << nl
        << "  metricsHinting: " << font->metricsHinting() << nl
        << "  ink: " << font->ink() << nl
        << "  paper: " << font->paper() << nl
        << "}";
}

}} // namespace cc::ui
