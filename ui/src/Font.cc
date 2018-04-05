/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/StyleManager>
#include <cc/ui/FontManager>
#include <cc/ui/Font>

namespace cc {
namespace ui {

const ScaledFont *Font::Instance::getScaledFont() const
{
    if (scaledFont_) {
        if (
            (family()->count() == 0 || scaledFont_->family()  == family()) &&
            scaledFont_->size()    == size()    &&
            scaledFont_->weight()  == weight()  &&
            scaledFont_->slant()   == slant()   &&
            scaledFont_->stretch() == stretch()
        )
            return scaledFont_;
    }

    scaledFont_ =
        FontManager::instance()->selectFont(
            (family()) ? family() : Font::defaultFamily(),
            (size() > 0) ? size() : Font::defaultSize(),
            weight(),
            slant(),
            stretch()
        );

    return scaledFont_;
}

const FontMetrics *Font::Instance::getMetrics() const
{
    return getScaledFont()->getMetrics();
}

Font &Font::operator<<(Pitch pitch)
{
    (*this)->setFamily(
        (pitch == Pitch::Fixed) ?
        StyleManager::instance()->activePlugin()->defaultFixedFontFamily() :
        StyleManager::instance()->activePlugin()->defaultFontFamily()
    );
    if ((*this)->size() <= 0 && pitch == Pitch::Fixed) {
        (*this)->setSize(
            StyleManager::instance()->activePlugin()->defaultFixedFontSize()
        );
    }
    return *this;
}

Font &Font::operator*=(double scale)
{
    double size = (*this)->size();
    if (size <= 0) size = defaultSize() * scale;
    else size = size * scale;
    (*this)->setSize(size);
    return *this;
}

String Font::defaultFamily()
{
    return StyleManager::instance()->activePlugin()->defaultFontFamily();
}

double Font::defaultSize()
{
    return StyleManager::instance()->activePlugin()->defaultFontSize();
}

bool Font::differ(const Font &a, const Font &b)
{
    return
        a->family()     != b->family()     ||
        a->size()       != b->size()       ||
        a->weight()     != b->weight()     ||
        a->slant()      != b->slant()      ||
        a->stretch()    != b->stretch()    ||
        a->decoration() != b->decoration() ||
        a->color()      != b->color();
}

}} // namespace cc::ui
